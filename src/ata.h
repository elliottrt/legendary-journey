#ifndef _ATA
#define _ATA

#include "types.h"

#define BIT(n) (1 << (n))

/* from https://wiki.osdev.org/ataread/write_sectors */
/* and  https://wiki.osdev.org/atapio_mode */

/* checks if ata is working properly */
void atainit(void);
/* flush hardware write cache, done after each write */
void atacacheflush(void);
/* perform software reset */
void atareset(void);
/* read sectors to some address */
void ataread(uint lba, uchar sectors, void *dst);
/* write sectors to disk */
void atawrite(uint lba, uchar sectors, const void *src);
/* returns 1 if ata encountered an error, 0 otherwise */
int atacheckerror(void);
/* prints error encountered */
void ataerror(void);
/* returns writable/readable sectors */
uint atasectors(void);

/* https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data */
/* not sure how reliable this is */
struct __attribute__((__packed__)) ataidentify
{
	ushort configurationflags;
	ushort cylinders;
	ushort specificconfig;
	ushort heads;
	ushort retired0[2];
	ushort sectorspertrack;
	ushort vendorunique0[3];
	uchar  serial[20];
	ushort retired1[2];
	ushort obsolete0;
	uchar  firmwarerevision[8];
	uchar  modelnumber[40];
	uchar  maximumblocktransfer;
	uchar  vendorunique1;
	ushort trustedcomputing;
	ushort capabilityflags;
	ushort reserved0;
	ushort obsolete1[2];
	ushort translationfreefall;
	ushort currentcylinders;
	ushort currentheads;
	ushort currentsectorspertrack;
	uint currentsectorcapacity;
	uchar  currentmultisectorsetting;
	uchar  commandflags;
	uint useraddressablesectors;
	ushort obsolete2;
	uchar  multiworddmasupport;
	uchar  multiworddmaactive;
	uchar  advancedpio;
	uchar  reserved1;
	ushort minumxfercycletime;
	ushort recommendedumxfercycletime;
	ushort minimumpiocycletime;
	ushort minimumpiocycletimeiordy;
	ushort additionalsupported;
	ushort reserved2[5];
	uchar  queuedepth;
	uchar  reserved3;
	uint serialatacapabilities;
	ushort serialatafeaturessupported;
	ushort serialatafeaturesenabled;
	ushort majorrevision;
	ushort minorrevision;
	ushort commandsetsupport[3];
	ushort commandsetenabled[3];
	uchar  ultradmasupport;
	uchar  ultradmaactive;
	ushort normalsecurityeraseunit;
	ushort extendedsecurityeraseunit;
	uchar  currentapmlevel;
	uchar  reserved4;
	ushort masterpasswordid;
	ushort hardwareresetresult;
	uchar  currentacousticvalue;
	uchar  recommendedacousticvalue;
	ushort streamminrequestsize;
	ushort streamingtransfertimedma;
	ushort streamingaccesslatencydmapio;
	uint streamingperfgranularity;
	uint max48bitlba[2];
	ushort streamingtransfertime;
	ushort dsmcap;
	ushort physicallogicalsectorsize;
	ushort interseekdelay;
	ushort worldwidename[4];
	ushort reservedworldwidename128[4];
	ushort reservedtlctechnicalreport;
	ushort wordsperlogicalsector[2];
	ushort commandsetsupportextensions;
	ushort commandsetactiveextensions;
	ushort reservedforextendedsupportandactive[6];
	ushort msnsupport;
	ushort securitystatus;
	ushort reserved5[31];
	ushort cfapowermode1;
	ushort reservedcfa[7];
	ushort nominalformfactor;
	ushort datasetmanagementfeature;
	ushort additionalproductid[4];
	ushort reservedcfa1[2];
	ushort currentmediaserial[30];
	ushort sctcommandtransport;
	ushort reserved6[2];
	ushort blockalignment;
	ushort wrverifysectorcountmode3only[2];
	ushort wrverifysectorcountmode2only[2];
	ushort nvcachecapabilities;
	ushort nvcachesizelsw;
	ushort nvcachesizemsw;
	ushort nominalmediarotationrate;
	ushort reserved7;
	uchar  nvcacheestspinuptimeseconds;
	uchar  reserved8;
	uchar  wrverifysectorcountmode;
	uchar  reserved9;
	ushort reserved10;
	ushort transportmajorversion;
	ushort transportminorversion;
	ushort reserved11[6];
	uint extendeduseraddressablesectors[2];
	ushort minblocksperdownloadmicrocodemode03;
	ushort maxblocksperdownloadmicrocodemode03;
	ushort reserved12[19];
	uchar  signature;
	uchar  checksum;
};

#endif
