

#include "ahci.h"
#include "../console.h"
#include "../pci.h"
#include "../fat32/fat_access.h"
#include "../usb/devicemanager.h"
#include "../dynamicmemory.h"

static inline void sysOutLong( unsigned short port, uint32_t val ){
    __asm__ volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline uint32_t sysInLong( unsigned short port ){
    uint32_t ret;
    __asm__ volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

HBA_MEM *gabar;

unsigned long ReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset){
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    unsigned long tmp = 0;


    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    sysOutLong (0xCF8, address);
    tmp = (unsigned long)(sysInLong (0xCFC) /* & 0xffff*/);
    return (tmp);
 }

QWORD ahca_installDevice(pciDev_t* device)
{
	Printf("\nBUS[%d],DEVICE[%d],VENDOR[%d],DEVICE[%d]",device->bus,device->device,device->vendorID,device->deviceID);
	Printf("\nRead_Address Bar 5=[%x]",ReadWord(device->bus,device->device,0,(0x24|0x0)));
	Printf("\nRead_Address=[%x]",ReadWord(device->bus,device->device,0,(0x3c|0x0))& 0x000000000000ff00 );
	QWORD bar = ReadWord(device->bus,device->device,0,(0x24|0x0));

	probe_port((HBA_MEM *)(QWORD)bar);
}

// Check device type
int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void probe_port(HBA_MEM *abar)
{
	// Search disk in impelemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{

				gabar = abar;
				Printf("SATA drive found at port %d\n", i);
				port_rebase(abar->ports, i);

				identify();

				return ;

			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				Printf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				Printf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				Printf("PM drive found at port %d\n", i);
			}
			else
			{
				Printf("No drive found at port %d\n", i);
			}
		}

		pi >>= 1;
		i ++;
	}
}

void port_rebase(HBA_PORT *port, int portno)
{
	//stop_cmd(port);	// Stop command engine

	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);

	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	port->fbu = 0;
	memset((void*)(port->fb), 0, 256);

	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
	int i;
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}

	start_cmd(port);	// Start command engine
}

// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
}

// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
    DWORD slots = (port->sact | port->ci);
    int num_of_slots= (gabar->cap & 0x0f00)>>8 ; // Bit 8-12

    int i;
    for (i=0; i<num_of_slots; i++)
    {

       if ((slots&1) == 0)
       {
            return i;
       }

       slots >>= 1;
    }

    return -1;

}

#define ATA_IDENT_MAX_LBA_EXT 200

void attach_ahci_disk(QWORD size)
{

	disk_t *disk = NEW (sizeof(disk_t));
	disk->type            = &HDDDISK;
	disk->sectorSize      = 512;
	disk->data            = 0;
	disk->accessRemaining = 0;
	disk->BIOS_driveNum   = 0;
	disk->headCount       = 0;
	disk->size 			  = size;

	deviceManager_attachDisk(disk);

}

void identify(HBA_PORT *port)
{
	char buf[1024 * 16];
	memset(buf,0,sizeof(buf));

	HBA_CMD_HEADER *cmdhead = (HBA_CMD_HEADER*)(port->clb);
	cmdhead-> cfl = sizeof (FIS_REG_H2D) / 4;
	cmdhead-> w = 0;
	cmdhead-> prdtl = 1;
	cmdhead-> p = 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL*)(cmdhead->ctba);
	memset (cmdtbl, 0, sizeof (HBA_CMD_TBL));

	cmdtbl-> prdt_entry [0] .dba = buf;
	cmdtbl-> prdt_entry [0] .dbc = 511;
	cmdtbl-> prdt_entry [0] .i = 1;

	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *) cmdtbl-> cfis;
	cmdfis-> fis_type = FIS_TYPE_REG_H2D;
	cmdfis-> c = 1;
	cmdfis-> command = ATA_CMD_IDENTIFY;

	port->ci = 1;

	while (1)
	{
	     if ((port-> ci & 1) == 0)
	     {
	            break;
	     };

	};

	QWORD size = * ((QWORD *) ((char *) buf + ATA_IDENT_MAX_LBA_EXT));

	QWORD GigaSize = (size * 512) / 1024 / 1024 / 1024;

	Printf("\n[Total Sector Count : %q\t  %d GigaByte]\n", size, GigaSize);

	attach_ahci_disk(size * 512);

    fl_init();
    fl_attach_media(ahci_diskio_read,ahci_diskio_write);


}

bool ahci_read(HBA_PORT *port, QWORD startl, QWORD count, QWORD *buf)
{
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return false;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

	int i=0;
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (BYTE)startl & 0xff;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)(startl>>32);
	cmdfis->lba5 = (BYTE)(startl>>40);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		Printf("Port is hung\n");
		return FALSE;
	}

	port->ci = 1<<slot;	// Issue command

	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0)
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			Printf("Read disk error\n");
			return FALSE;
		}
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		Printf("Read disk error\n");
		return FALSE;
	}

	return true;
}


int ahci_write(HBA_PORT *port, QWORD startl, QWORD count, QWORD buf)
{
		port->is = 0xffff;              // Clear pending interrupt bits
     // int spin = 0;           // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
        {
        	Printf(" cant found cmdslot %d\n",port);
        	return 0;
        }
        uint64_t addr = 0;
     //   print("\n clb %x clbu %x", port->clb, port->clbu);
        addr = (((addr | port->clbu) << 32) | port->clb);
        HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;

        //HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
        cmdheader += slot;
        cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);     // Command FIS size
        cmdheader->w = 1;               // Read from device
        cmdheader->c = 1;               // Read from device
        cmdheader->p = 1;               // Read from device
        // 8K bytes (16 sectors) per PRDT
        cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count

        addr=0;
        addr=(((addr | cmdheader->ctbau)<<32)|cmdheader->ctba);
        HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(addr);

        //memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
        int i = 0;
    //    print("[PRDTL][%d]", cmdheader->prdtl);
        // 8K bytes (16 sectors) per PRDT
        for (i=0; i<cmdheader->prdtl-1; i++)
        {
               cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
               cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
               cmdtbl->prdt_entry[i].dbc = 8*1024-1;     // 8K bytes
               cmdtbl->prdt_entry[i].i = 0;
               buf += 4*1024;  // 4K words
               count -= 16;    // 16 sectors
       }
        /**If the final Data FIS transfer in a command is for an odd number of 16-bit words, the transmitter�뜝�릯
Transport layer is responsible for padding the final Dword of a FIS with zeros. If the HBA receives one
more word than is indicated in the PRD table due to this padding requirement, the HBA shall not signal
this as an overflow condition. In addition, if the HBA inserts padding as required in a FIS it is transmitting,
an overflow error shall not be indicated. The PRD Byte Count field shall be updated based on the
number of words specified in the PRD table, ignoring any additional padding.**/

        // Last entry

        cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbc = count << 9 ;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 0;


        // Setup command
        FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = ATA_CMD_WRITE_DMA_EX;

        cmdfis->lba0 = (BYTE)startl & 0xff;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode

        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)(startl>>32);
        cmdfis->lba5 = (BYTE)(startl>>40);

        cmdfis->countl = count & 0xff;
        cmdfis->counth = count>>8;

        port->ci = 1;    // Issue command
        // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0)
                        break;
                if (port->is & HBA_PxIS_TFES)   // Task file error
                {
                        Printf("write disk error %d\n",startl);
                        return 0;
                }
        }

        // Check again
        if (port->is & HBA_PxIS_TFES)
        {
        	Printf("write disk error %d\n",startl);
            return 0;
        }

        while(port->ci != 0)
        {

        }
        return 1;
}

