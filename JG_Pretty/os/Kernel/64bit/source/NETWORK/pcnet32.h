/*
**
**  PCNet-PCI (Lance) network card implementation, derived from
**  AMD whitepaper's pseudo-code at:
**  http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/19669.pdf
**
**  (c)2001 Graham Batty
**  License: NewOS License
**
*/

#ifndef _PCNET32_DEV_H
#define _PCNET32_DEV_H

/* AMD Vendor ID */
#define AMD_VENDORID                          0x1022

/* PCNet/Home Device IDs */
#define PCNET_DEVICEID                        0x2000
#define PCHOME_DEVICEID                       0x2001


#define	DEV_ACT_INIT		0x1
#define	DEV_ACT_READ		0x2
#define	DEV_ACT_WRITE		0x4
#define	DEV_ACT_RESET		0x8
#define	DEV_ACT_STOP		0x10
#define	DEV_ACT_MOUNT		0x20
#define	DEV_ACT_PLAY		0x20
#define	DEV_ACT_UPDATE		0x40
#define	DEV_ACT_UMOUNT		0x80

/* externs */
char pcnet32_acthandler (unsigned act, char *block, unsigned block_len);
unsigned init_pcnet32 ();
#endif
