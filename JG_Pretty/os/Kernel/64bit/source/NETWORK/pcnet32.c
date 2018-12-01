/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2009  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ARCH_i386
#undef CONFIG_DRV_PCNET32
#endif

//#ifdef CONFIG_DRV_PCNET32

#include "pcnet32.h"
#include "../PCI.h"
#include "../task.h"
#include "../NETPROTOCOL/eth.h"
#include "../NETPROTOCOL/net.h"
#include "../AssemblyUtility.h"
#include "../DynamicMemory.h"
#include "../fat32/fat_filelib.h"
#include "../types.h"
#include "../consoleshell.h"
/* AMD Vendor ID */
#define AMD_VENDORID				0x1022

/* PCNet/Home Device IDs */
#define PCNET_DEVICEID				0x2000

/* I/O 32bit resources */
#define PCNET32_IO_APR				0x00
#define PCNET32_IO_RDP				0x10
#define PCNET32_IO_ADDR              		0x12
#define	PCNET32_IO_RAP				0x14
#define	PCNET32_IO_BUSIF			0x16
#define PCNET32_IO_RESET			0x18
#define PCNET32_IO_BDP				0x1C

/* Ring's sizes */
#define PCNET32_BUF_ENC_TX 0
#define PCNET32_BUF_ENC_RX 0

#define PCNET32_RING_TX_SIZE			(1 << (PCNET32_BUF_ENC_TX))
#define PCNET32_RING_TX_LEN_BITS		((PCNET32_BUF_ENC_TX) << 12)

#define PCNET32_RING_RX_SIZE			(1 << (PCNET32_BUF_ENC_RX))
#define PCNET32_RING_RX_LEN_BITS		((PCNET32_BUF_ENC_RX) << 4)

#define PCNET32_RING_TX_BUF_LEN			1024
#define PCNET32_RING_RX_BUF_LEN			2048

/* Ring entries state */
#define PCNET32_FLAG_RX_OWN 			0x8000

/* CSR port definition */
#define PCNET32_CSR_STATUS			0x0

/* BCR port definition */
#define PCNET32_BCR_BUSCTL			0x12
#define PCNET32_BCR_SWMODE			0x14

/* PCNnet32 CSR status */
#define PCNET32_STATUS_ERR			0x8000
#define PCNET32_STATUS_BABL			0x4000
#define PCNET32_STATUS_CERR			0x2000
#define PCNET32_STATUS_MISS			0x1000
#define PCNET32_STATUS_MERR			0x800
#define PCNET32_STATUS_RINT			0x400
#define PCNET32_STATUS_TINT			0x200
#define PCNET32_STATUS_IDON			0x100
#define PCNET32_STATUS_INTR			0x80
#define PCNET32_STATUS_IENA			0x40
#define PCNET32_STATUS_RXON			0x20
#define PCNET32_STATUS_TXON			0x10
#define PCNET32_STATUS_TDMD			0x8
#define PCNET32_STATUS_STOP			0x0004
#define PCNET32_STATUS_START			0x2
#define PCNET32_STATUS_INIT			0x1

/* Interrupt status definition */
#define PCNET32_INT_RST  			0x0000
#define PCNET32_INT_WINT 			0x0200
#define PCNET32_INT_RINT 			0x0400
#define PCNET32_INT_RERR 			0x1000
#define PCNET32_INT_WERR 			0x4000
#define PCNET32_INT_ERR  			0x8000

#define	LANCE_MUST_PAD		0x00000001
#define	LANCE_ENABLE_AUTOSELECT	0x00000002
#define	LANCE_SELECT_PHONELINE	0x00000004
#define	LANCE_MUST_UNRESET	0x00000008

struct pcnet32_ringtx_t* PCNET32_TXRING;//					0x151000
struct pcnet32_ringrx_t* PCNET32_RXRING;//					0x159000

static unsigned short pcnet32_ring_xlen[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

struct pcnet32_init_t *iblock = 0;

#define	__PACKED__ __attribute__ ((__packed__))

/* A mapping from the chip ID number to the part number and features.
   These are from the datasheets -- in real life the '970 version
   reportedly has the same ID as the '965. */
static const struct lance_chip_type
{
	int id_number;
	const char *name;
	int flags;
}

chip_table[] = {
	{0x0000, "LANCE 7990",			/* Ancient lance chip.  */
		LANCE_MUST_PAD + LANCE_MUST_UNRESET},
	{0x0003, "PCnet/ISA 79C960",		/* 79C960 PCnet/ISA.  */
		LANCE_ENABLE_AUTOSELECT},
	{0x2260, "PCnet/ISA+ 79C961",		/* 79C961 PCnet/ISA+, Plug-n-Play.  */
		LANCE_ENABLE_AUTOSELECT},
	{0x2420, "PCnet/PCI 79C970",		/* 79C970 or 79C974 PCnet-SCSI, PCI. */
		LANCE_ENABLE_AUTOSELECT},
	/* Bug: the PCnet/PCI actually uses the PCnet/VLB ID number, so just call
		it the PCnet32. */
	{0x2430, "PCnet32",			/* 79C965 PCnet for VL bus. */
		LANCE_ENABLE_AUTOSELECT},
        {0x2621, "PCnet/PCI-II 79C970A",        /* 79C970A PCInetPCI II. */
                LANCE_ENABLE_AUTOSELECT},
	{0x2625, "PCnet-FAST III 79C973",	/* 79C973 PCInet-FAST III. */
		LANCE_ENABLE_AUTOSELECT},
        {0x2626, "PCnet/HomePNA 79C978",
                LANCE_ENABLE_AUTOSELECT|LANCE_SELECT_PHONELINE},
	{0x0, "PCnet (unknown)",
		LANCE_ENABLE_AUTOSELECT},
};


/* init block structure */
struct pcnet32_init_t {
	unsigned short mode;    /* copied into csr15 */
   	unsigned short res1:4;  /* reserved */
   	unsigned short rlen:4;   /* number of receive descriptor ring entries */
   	unsigned short res2:4;  /* reserved */
  	unsigned short tlen:4;  /* number of transmit descriptor ring entries */
	unsigned char padr[6];  /* mac address */
 	unsigned short res3;    /* reserved */
	unsigned ladrf1;    	/* logical address filter  0..31 */
	unsigned ladrf2;    	/* logical address filter 32..63 */
	unsigned rdra;      	/* address of receive descriptor ring */
	unsigned tdra;      	/* address of transmit descriptor ring */
} __PACKED__ ;


/** Transmit Message Descriptor */
struct pcnet32_ringtx_t
{
    struct
    {
        unsigned tbadr;         /**< transmit buffer address */
    } tmd0;
    struct
    {
        unsigned bcnt:12;       /**< buffer byte count (two's complement) */
        unsigned ones:4;        /**< must be 1111b */
        unsigned res:7;         /**< reserved */
        unsigned bpe:1;         /**< bus parity error */
        unsigned enp:1;         /**< end of packet */
        unsigned stp:1;         /**< start of packet */
        unsigned def:1;         /**< deferred */
        unsigned one:1;         /**< exactly one retry was needed to transmit a frame */
        unsigned ltint:1;       /**< suppress interrupts after successful transmission */
        unsigned nofcs:1;       /**< when set, the state of DXMTFCS is ignored and
                                     transmitter FCS generation is activated. */
        unsigned err:1;         /**< error occured */
        unsigned own:1;         /**< 0=owned by guest driver, 1=owned by controller */
    } tmd1;
    struct
    {
        unsigned trc:4;         /**< transmit retry count */
        unsigned res:12;        /**< reserved */
        unsigned tdr:10;        /**< ??? */
        unsigned rtry:1;        /**< retry error */
        unsigned lcar:1;        /**< loss of carrier */
        unsigned lcol:1;        /**< late collision */
        unsigned exdef:1;       /**< excessive deferral */
        unsigned uflo:1;        /**< underflow error */
        unsigned buff:1;        /**< out of buffers (ENP not found) */
    } tmd2;
    struct
    {
        unsigned res;           /**< reserved for user defined space */
    } tmd3;
} __PACKED__;

/** Receive Message Descriptor */
struct pcnet32_ringrx_t {
    struct
    {
        unsigned rbadr;         /**< receive buffer address */
    } rmd0;
    struct
    {
        unsigned bcnt:12;       /**< buffer byte count (two's complement) */
        unsigned ones:4;        /**< must be 1111b */
        unsigned res:4;         /**< reserved */
        unsigned bam:1;         /**< broadcast address match */
        unsigned lafm:1;        /**< logical filter address match */
        unsigned pam:1;         /**< physcial address match */
        unsigned bpe:1;         /**< bus parity error */
        unsigned enp:1;         /**< end of packet */
        unsigned stp:1;         /**< start of packet */
        unsigned buff:1;        /**< buffer error */
        unsigned crc:1;         /**< crc error on incoming frame */
        unsigned oflo:1;        /**< overflow error (lost all or part of incoming frame) */
        unsigned fram:1;        /**< frame error */
        unsigned err:1;         /**< error occured */
        unsigned own:1;         /**< 0=owned by guest driver, 1=owned by controller */
    } rmd1;
    struct
    {
        unsigned mcnt:12;       /**< message byte count */
        unsigned zeros:4;       /**< 0000b */
        unsigned rpc:8;         /**< receive frame tag */
        unsigned rcc:8;         /**< receive frame tag + reserved */
    } rmd2;
    struct
    {
        unsigned res;           /**< reserved for user defined space */
    } rmd3;
} __PACKED__;

/* pcnet32 device structure */
struct pcnet32_dev_t {
	unsigned char irq;
	pciDev_t *pcidev;
	unsigned short addr_io;
	mac_addr_t addr_mac;
	int lance_ver;
	int chip_ver;
};

struct pcnet32_dev_t *pcnet32_dev;
static netdev_t *ifdev;
QWORD init_block;

/* I/O operations */
unsigned char pcnet32_read8 (struct pcnet32_dev_t *dev, unsigned short port)
{
	return InPortByte (dev->addr_io + port);
}

unsigned short pcnet32_read16 (struct pcnet32_dev_t *dev, unsigned short port)
{
	return InPortWord (dev->addr_io + port);
}

void pcnet32_write16 (struct pcnet32_dev_t *dev, unsigned short port, unsigned short val)
{
	OutPortWord (dev->addr_io + port, val);
}

unsigned pcnet32_read32 (struct pcnet32_dev_t *dev, unsigned short port)
{
	return InPortDWord (dev->addr_io + port);
}

void pcnet32_write32 (struct pcnet32_dev_t *dev, unsigned short port, unsigned val)
{
	OutPortDWord (dev->addr_io + port, val);
}

/* Register access */
unsigned pcnet32_read_csr (struct pcnet32_dev_t *dev, unsigned short reg)
{
	pcnet32_write32 (dev, PCNET32_IO_RAP, reg);
	return pcnet32_read32 (dev, PCNET32_IO_RDP);
}

void pcnet32_write_csr (struct pcnet32_dev_t *dev, unsigned short reg, unsigned short val)
{
	pcnet32_write32 (dev, PCNET32_IO_RAP, reg);
	pcnet32_write32 (dev, PCNET32_IO_RDP, val);
}

void pcnet32_write_bcr (struct pcnet32_dev_t *dev, unsigned short reg, unsigned short val)
{
	pcnet32_write32 (dev, PCNET32_IO_RAP, reg);
	pcnet32_write32 (dev, PCNET32_IO_BDP, val);
}


/* Prototype */

unsigned pcnet32_ring_create (struct pcnet32_dev_t *dev, struct pcnet32_init_t *iblock);
unsigned pcnet32_start (struct pcnet32_dev_t *dev);
void pcnet32_int (pciDev_t* device);
unsigned pcnet32_int_rx (struct pcnet32_dev_t *dev);
unsigned pcnet32_tx (char *buf, unsigned len);

/* READ/WRITE func */
unsigned pcnet32_read (char *buf, unsigned len)
{
	return pcnet32_acthandler (DEV_ACT_READ, buf, len);
}

unsigned pcnet32_write (char *buf, unsigned len)
{
	return pcnet32_acthandler (DEV_ACT_WRITE, buf, len);
}

/* detect pcnet32 device in PC */
pciDev_t *pcnet32_detect ()
{
	/* First detect network card - is connected to PCI bus ?*/
	pciDev_t *pcidev = pcidev_find (AMD_VENDORID, PCNET_DEVICEID);

	if (!pcidev)
		return 0;

	return pcidev;
}

unsigned pcnet32_reset (struct pcnet32_dev_t *dev)
{
	if (!iblock)
	{
		Printf ("pcnet32_reset block is 0\n");
		return 0;
	}

	//int ret = pcnet32_ring_create (dev, iblock);

	//pcnet32_write_csr (dev, 58, ~0);

	pcnet32_read32 (dev, PCNET32_IO_RESET);
	pcnet32_write32 (dev, PCNET32_IO_RESET, 0);

	/* set to 32bit mode */
	pcnet32_write32 (dev, PCNET32_IO_RDP, 0);
	pcnet32_write_bcr (dev, PCNET32_BCR_SWMODE, 0x0002);

	/* Get the version of the chip */
	pcnet32_write16 (dev, PCNET32_IO_ADDR, 88);

	if (pcnet32_read16 (dev, PCNET32_IO_ADDR) != 88)
		dev->lance_ver = 0;
	else {
		dev->chip_ver = pcnet32_read16 (dev, PCNET32_IO_RDP);
		pcnet32_write16 (dev, PCNET32_IO_ADDR, 89);

		dev->chip_ver |= pcnet32_read16 (dev, PCNET32_IO_RDP) << 16;

		if ((dev->chip_ver & 0xfff) != 0x3) {
			Printf ("pcnet32 -> unknown revision !\n");
			while (1);
			return -1;
		}

		dev->chip_ver = (dev->chip_ver >> 12) & 0xffff;

		for (dev->lance_ver = 1; chip_table[dev->lance_ver].id_number != 0; ++ dev->lance_ver)
			if (chip_table[dev->lance_ver].id_number == dev->chip_ver)
				break;
	}

	//kPrintf ("> lance '%s' - ver: 0x%x & chip ver: 0x%x\n", chip_table[dev->lance_ver].name, dev->lance_ver, dev->chip_ver);

	iblock->mode = 0x0;	// 0x8000 promiskuitni rezim

	iblock->rlen = PCNET32_BUF_ENC_RX;

	iblock->tlen = PCNET32_BUF_ENC_TX;

	memcpy (iblock->padr, dev->addr_mac, 6);

	iblock->ladrf1 = (unsigned) ~0;
	iblock->ladrf2 = (unsigned) ~0;

	Printf("init block: %d\n", sizeof (struct pcnet32_init_t));

	//Printf("rxring: 0x%x / txring: 0x%x\n", iblock->rdra, iblock->tdra);

	init_block = (QWORD) iblock;

	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_STOP);

	pcnet32_write_csr (dev, 1, init_block);
	pcnet32_write_csr (dev, 2, ((unsigned) init_block >> 16));
	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_INIT);
	while (!(pcnet32_read_csr (dev, PCNET32_CSR_STATUS) & PCNET32_STATUS_IDON));
	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_START | PCNET32_STATUS_IENA);

	return 1;
}

/* INIT sequence */
unsigned init_pcnet32 ()
{
	unsigned i = 0;

	pciDev_t *pcidev = pcnet32_detect ();

	if (!pcidev)
		return 0;

	struct pcnet32_dev_t *dev = (struct pcnet32_dev_t *) NEW (sizeof (struct pcnet32_dev_t));

	if (!dev)
		return 0;


	PCNET32_TXRING =  (struct pcnet32_ringtx_t *)NEW(sizeof(struct pcnet32_ringtx_t));
	PCNET32_RXRING =  (struct pcnet32_ringrx_t *)NEW(sizeof(struct pcnet32_ringrx_t));

	uint16_t pciCommandRegister = pci_configRead(pcidev, PCI_COMMAND, 2);
	pci_configWrite_word(pcidev, PCI_COMMAND, pciCommandRegister | PCI_CMD_IO | PCI_CMD_BUSMASTER); // resets status register, sets command register


	/* get irq id */
	dev->irq = pcidev->irq;

	 uint8_t j;
	 for (j = 0; j < 6; ++j) // check network card BARs
	    {
	        if (pcidev->bar[j].memoryType == PCI_IO)
	        {
	        	dev->addr_io = pcidev->bar[j].baseAddress;
	            Printf("\nCheck PCNet check io bar %d  \n",j);
	            break; // We are interested in the first IO BAR
	        }
	    }

	// get mac address from ethernet
	for (i = 0; i < 6; i++)
		dev->addr_mac[i] = pcnet32_read8 (dev, i);

	irq_installPCIHandler(dev->irq, pcnet32_int, pcidev);

	pcnet32_dev = dev;



	return pcnet32_start (dev);
}



unsigned pcnet32_start (struct pcnet32_dev_t *dev)
{
	iblock = (struct pcnet32_init_t *) NEW (sizeof(struct pcnet32_init_t));	// TODO: page aligned address

	if (!iblock)
	{
		Printf ("pcnet32 iblock fail!\n");
		return 0;
	}

	int ret = pcnet32_ring_create (dev, iblock);

	/* pre-set to 32bit mode */
	pcnet32_write_csr (dev, 58, 0x102 | 0x0300);

	pcnet32_read32 (dev, PCNET32_IO_RESET);
	pcnet32_write32 (dev, PCNET32_IO_RESET, 0);

	/* Get the version of the chip */
	pcnet32_write16 (dev, PCNET32_IO_ADDR, 88);

	if (pcnet32_read16 (dev, PCNET32_IO_ADDR) != 88)
		dev->lance_ver = 0;
	else {
		dev->chip_ver = pcnet32_read16 (dev, PCNET32_IO_RDP);
		pcnet32_write16 (dev, PCNET32_IO_ADDR, 89);

		dev->chip_ver |= pcnet32_read16 (dev, PCNET32_IO_RDP) << 16;

		if ((dev->chip_ver & 0xfff) != 0x3) {
			Printf ("pcnet32 -> unknown revision !\n");
			while (1);
			return -1;
		}

		dev->chip_ver = (dev->chip_ver >> 12) & 0xffff;

		for (dev->lance_ver = 1; chip_table[dev->lance_ver].id_number != 0; ++ dev->lance_ver)
			if (chip_table[dev->lance_ver].id_number == dev->chip_ver)
				break;
	}

//	Printf ("> lance '%s' - ver: 0x%x & chip ver: 0x%x\n", chip_table[dev->lance_ver].name, dev->lance_ver, dev->chip_ver);

	/* set to 32bit mode */
	pcnet32_write32 (dev, PCNET32_IO_RDP, 0);
	pcnet32_write_bcr (dev, PCNET32_BCR_SWMODE, 0x102 | 0x300);

	//Printf ("58: 0x%x\n", pcnet32_read_csr (dev, 58));

	iblock->mode = 0x0;	// 0x8000 promiskuitni rezim

	iblock->rlen = PCNET32_BUF_ENC_RX;

	iblock->tlen = PCNET32_BUF_ENC_TX;

	memcpy (iblock->padr, dev->addr_mac, 6);

	iblock->ladrf1 = (unsigned) ~0;
	iblock->ladrf2 = (unsigned) ~0;

//	Printf("init block: %d\n", sizeof (struct pcnet32_init_t));
//	Printf("rxring: 0x%x / txring: 0x%x\n", iblock->rdra, iblock->tdra);

	init_block = (QWORD) iblock;

	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_STOP);

	pcnet32_write_csr (dev, 1, init_block);
	pcnet32_write_csr (dev, 2, ((unsigned) init_block >> 16));
	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_INIT);
	while (!(pcnet32_read_csr (dev, PCNET32_CSR_STATUS) & PCNET32_STATUS_IDON));
	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_START | PCNET32_STATUS_IENA);
	/*if (chip_table[dev->lance_ver].flags & LANCE_ENABLE_AUTOSELECT) {
		// This is 79C960 specific; Turn on auto-select of media
		   (AUI, BNC)
		pcnet32_write16 (dev, PCNET32_IO_ADDR, 0x2);
		// Don't touch 10base2 power bit
		pcnet32_write16 (dev, PCNET32_IO_BUSIF, pcnet32_read16 (dev, PCNET32_IO_BUSIF) | 0x2);
	}*/

	ifdev = netdev_create (dev->addr_mac, &pcnet32_read, &pcnet32_write, dev->addr_io);

	if (!ifdev)
		return 0;

	return 1;
}

unsigned pcnet32_ring_create (struct pcnet32_dev_t *dev, struct pcnet32_init_t *iblock)
{
	/* create tx ring */
		struct pcnet32_ringtx_t *ringtx = (struct pcnet32_ringtx_t *) PCNET32_TXRING;

		iblock->tdra = (unsigned) ringtx;

		memset (ringtx, 0, sizeof (struct pcnet32_ringtx_t));

		/* create rx ring */
		struct pcnet32_ringrx_t *ringrx = (struct pcnet32_ringrx_t *) PCNET32_RXRING;

		iblock->rdra = (unsigned) ringrx;

        unsigned short index = 0;
        for (index = 0; index < pcnet32_ring_xlen[PCNET32_BUF_ENC_RX]+1; index ++) {
                ringrx[index].rmd0.rbadr = (unsigned) NEW (sizeof (unsigned char) * PCNET32_RING_RX_BUF_LEN);
		ringrx[index].rmd1.bcnt = 2048;
                ringrx[index].rmd1.own = ~0;
		ringrx[index].rmd3.res = 0;
		ringrx[index].rmd1.ones = 0xf;
		ringrx[index].rmd2.zeros = 0x0;
        }

	return 1;
}


unsigned kix = 0;
void pcnet32_int (pciDev_t* device)
{
//	Printf("pcnet32_int called\n");

	struct pcnet32_dev_t *dev = pcnet32_dev;

	int status = pcnet32_read_csr (dev, PCNET32_CSR_STATUS);

	/* disable device interrupts */
	if (status & 0x8600)
		pcnet32_write_csr (dev, PCNET32_CSR_STATUS, status & ~PCNET32_STATUS_IENA);

        if (status & PCNET32_INT_RERR)
        	Printf( "pcnet32 -> read error\n");

        if (status & PCNET32_INT_WERR)
        	Printf("pcnet32 -> write error\n");

	if (status & PCNET32_INT_RINT) {
//		DPRINT (DBG_DRIVER | DBG_ETH, "pcnet32 -> pcnet32_int_rx ()");
		pcnet32_int_rx (dev);
	}

	/*if (status & PCNET32_STATUS_RXON)
		kPrintf ("pcnet32 -> RXON ()\n");
	if (status & PCNET32_STATUS_TXON)
		kPrintf ("pcnet32 -> TXON ()\n");
	if (status & PCNET32_STATUS_IDON)
		kPrintf ("pcnet32 -> IDON ()\n");*/

        if (status & PCNET32_INT_ERR) {
        	Printf("pcnet32 -> device error\n");
		pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_STOP);
		pcnet32_reset (dev);
		//pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_START);
		//pcnet32_reset (dev);
		//return;
		/*if (kix > 5) {
			pcnet32_write_csr (dev, PCNET32_CSR_STATUS, PCNET32_STATUS_STOP);
			kix = 0;
		}
		kix ++;*/
	}

	//kPrintf ("pcnet32 -> interrupt (0x%x)\n", status);

	/* enable device interrupts */
	pcnet32_write_csr (dev, PCNET32_CSR_STATUS, 0x7940);

}

unsigned pcnet32_int_rx (struct pcnet32_dev_t *dev)
{
	struct pcnet32_ringrx_t *ringrx = (struct pcnet32_ringrx_t *)PCNET32_RXRING;

    unsigned short index = 0;
    for (index = 0; index < pcnet32_ring_xlen[PCNET32_BUF_ENC_RX]; index ++)
    {
		if (ringrx[index].rmd1.own)
			continue;

		/* this is needed for again use, because pcnet32 change it to ~1 */
		ringrx[index].rmd1.own = ~0;

		char *buf = (char *) ringrx[index].rmd0.rbadr;

		netdev_rx_add_queue (ifdev, buf, ringrx[index].rmd2.mcnt);
    }

	return 1;
}

unsigned pcnet32_tx (char *buf, unsigned len)
{
	/* create tx ring */
	struct pcnet32_ringtx_t *ringtx =  (struct pcnet32_ringtx_t *) PCNET32_TXRING;

        unsigned short index = 0;
        for (index = 0; index < pcnet32_ring_xlen[PCNET32_BUF_ENC_TX]; index ++) {
		if (ringtx[index].tmd1.own == ~0)
			Printf( "pcnet32 -> bad txring.own bit!\n");

       ringtx[index].tmd0.tbadr = (unsigned) buf;
		ringtx[index].tmd1.bcnt = -len;
        ringtx[index].tmd1.own = ~0;
		ringtx[index].tmd3.res = 0;
		ringtx[index].tmd1.ones = ~0;
		ringtx[index].tmd1.stp = ~0;
		ringtx[index].tmd1.enp = ~0;
        }
}

char pcnet32_acthandler (unsigned act, char *block, unsigned block_len)
{
	switch (act) {
		case DEV_ACT_INIT:
		{
			return init_pcnet32 ();
		}
		break;
		case DEV_ACT_READ:
		{
			/* see pcnet32_int_rx interrupt */

			return 1;
		}
		break;
		case DEV_ACT_WRITE:
		{
			pcnet32_tx (block, block_len);

			return 1;
		}
		break;
	}

	return 0;
}
//#endif

