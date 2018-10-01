

#include "ahci.h"
#include "../console.h"
#include "../pci.h"


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

#define	AHCI_BASE	0x400000	// 4M

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

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
	Printf("HBA_MEM->cap %d\n", abar->cap);
	Printf("HBA_MEM->ghc %d\n", abar->ghc);
	Printf("HBA_MEM->is %d\n", abar->is);
	Printf("HBA_MEM->pi %d\n", abar->pi);
	Printf("HBA_MEM->vs %d\n", abar->vs);
	Printf("HBA_MEM->ccc_ctl %d\n", abar->ccc_ctl);
	Printf("HBA_MEM->ccc_pts %d\n", abar->ccc_pts);
	Printf("HBA_MEM->em_loc %d\n", abar->em_loc);
	Printf("HBA_MEM->em_ctl %d\n", abar->em_ctl);
	Printf("HBA_MEM->cap2 %d\n", abar->cap2);
	Printf("HBA_MEM->bohc %d\n", abar->bohc);

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

				char buf[1024 * 160];

				memcpy(buf,"999999999",10);

				ahci_write(&abar->ports[i],0,0,10,buf);

				memset(buf,sizeof(buf),0);

				ahci_read(&abar->ports[i],0,0,10,buf);

				Printf("\n\n\n%s\n", buf);

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

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES   (1 << 30)       /* TFES - Task File Error Status */
#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX     0x35

// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
    DWORD slots = (port->sact | port->ci);
    int num_of_slots= (gabar->cap & 0x0f00)>>8 ; // Bit 8-12

    Printf("\n[command slot is : %d num_of_slots: %d ]", slots,num_of_slots);

    int i;
    for (i=0; i<num_of_slots; i++)
    {

               if ((slots&1) == 0)
               {
                      Printf("\n[command slot is : %d]", i);
                       return i;

               }
               slots >>= 1;
    }

    Printf("Cannot find free command list entry\n");
    return -1;

}


bool ahci_read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
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

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);

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


int ahci_write(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, QWORD buf)
{
       port->is = 0xffff;              // Clear pending interrupt bits
     // int spin = 0;           // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
                return 0;
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
        /**If the final Data FIS transfer in a command is for an odd number of 16-bit words, the transmitter占퐏
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

        cmdfis->lba0 = (BYTE)startl;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode

        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)starth;
        cmdfis->lba5 = (BYTE)(starth>>8);

        cmdfis->countl = count & 0xff;
        cmdfis->counth = count>>8;

    //    print("[slot]{%d}", slot);
        port->ci = 1;    // Issue command
    //    print("\n[Port ci ][%d]", port->ci);
   //     print("\nafter issue : %d" , port->tfd);
    //    print("\nafter issue : %d" , port->tfd);

    //    print("\nbefore while 1--> %d", slot);
        // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0)
                        break;
                if (port->is & HBA_PxIS_TFES)   // Task file error
                {
                        Printf("Read disk error\n");
                        return 0;
                }
        }
     //   print("\n after while 1");
     //   print("\nafter issue : %d" , port->tfd);
        // Check again
        if (port->is & HBA_PxIS_TFES)
        {
        	Printf("Read disk error\n");
            return 0;
        }

     //   print("\n[Port ci ][%d]", port->ci);
       // int k = 0;
        while(port->ci != 0)
        {

        }
        return 1;
}

