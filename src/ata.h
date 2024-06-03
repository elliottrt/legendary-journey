#ifndef _ATA
#define _ATA

#include "types.h"

struct ataidentify;

/* from https://wiki.osdev.org/ataread/write_sectors */
/* and  https://wiki.osdev.org/atapio_mode */

// TODO: ata code is duplicated, fix it

/* checks if ata is working properly */
void atainit(void);
/* flush hardware write cache, done after each write */
void atacacheflush(void);
/* perform software reset */
void atareset(void);
/* read sectors to some address */
bool ataread(uint32_t lba, uint8_t sectors, void *dst);
/* write sectors to disk */
bool atawrite(uint32_t lba, uint8_t sectors, const void *src);
/* returns 1 if ata encountered an error, 0 otherwise */
bool atacheckerror(void);
/* prints error encountered */
void ataerror(void);
/* returns writable/readable sectors */
uint32_t atasectors(void);
/* returns the address of the ATAIDENTIFY block */
struct ataidentify *atagetidentify(void);

/* https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data */
/* not sure how reliable this is */
struct ataidentify
{
	uint16_t configurationflags;
	uint16_t cylinders;
	uint16_t specificconfig;
	uint16_t heads;
	uint16_t retired0[2];
	uint16_t sectorspertrack;
	uint16_t vendorunique0[3];
	uint8_t  serial[20];
	uint16_t retired1[2];
	uint16_t obsolete0;
	uint8_t  firmwarerevision[8];
	uint8_t  modelnumber[40];
	uint8_t  maximumblocktransfer;
	uint8_t  vendorunique1;
	uint16_t trustedcomputing;
	uint16_t capabilityflags;
	uint16_t reserved0;
	uint16_t obsolete1[2];
	uint16_t translationfreefall;
	uint16_t currentcylinders;
	uint16_t currentheads;
	uint16_t currentsectorspertrack;
	uint32_t currentsectorcapacity;
	uint8_t  currentmultisectorsetting;
	uint8_t  commandflags;
	uint32_t useraddressablesectors;
	uint16_t obsolete2;
	uint8_t  multiworddmasupport;
	uint8_t  multiworddmaactive;
	uint8_t  advancedpio;
	uint8_t  reserved1;
	uint16_t minumxfercycletime;
	uint16_t recommendedumxfercycletime;
	uint16_t minimumpiocycletime;
	uint16_t minimumpiocycletimeiordy;
	uint16_t additionalsupported;
	uint16_t reserved2[5];
	uint8_t  queuedepth;
	uint8_t  reserved3;
	uint32_t serialatacapabilities;
	uint16_t serialatafeaturessupported;
	uint16_t serialatafeaturesenabled;
	uint16_t majorrevision;
	uint16_t minorrevision;
	uint16_t commandsetsupport[3];
	uint16_t commandsetenabled[3];
	uint8_t  ultradmasupport;
	uint8_t  ultradmaactive;
	uint16_t normalsecurityeraseunit;
	uint16_t extendedsecurityeraseunit;
	uint8_t  currentapmlevel;
	uint8_t  reserved4;
	uint16_t masterpasswordid;
	uint16_t hardwareresetresult;
	uint8_t  currentacousticvalue;
	uint8_t  recommendedacousticvalue;
	uint16_t streamminrequestsize;
	uint16_t streamingtransfertimedma;
	uint16_t streamingaccesslatencydmapio;
	uint32_t streamingperfgranularity;
	uint32_t max48bitlba[2];
	uint16_t streamingtransfertime;
	uint16_t dsmcap;
	uint16_t physicallogicalsectorsize;
	uint16_t interseekdelay;
	uint16_t worldwidename[4];
	uint16_t reservedworldwidename128[4];
	uint16_t reservedtlctechnicalreport;
	uint16_t wordsperlogicalsector[2];
	uint16_t commandsetsupportextensions;
	uint16_t commandsetactiveextensions;
	uint16_t reservedforextendedsupportandactive[6];
	uint16_t msnsupport;
	uint16_t securitystatus;
	uint16_t reserved5[31];
	uint16_t cfapowermode1;
	uint16_t reservedcfa[7];
	uint16_t nominalformfactor;
	uint16_t datasetmanagementfeature;
	uint16_t additionalproductid[4];
	uint16_t reservedcfa1[2];
	uint16_t currentmediaserial[30];
	uint16_t sctcommandtransport;
	uint16_t reserved6[2];
	uint16_t blockalignment;
	uint16_t wrverifysectorcountmode3only[2];
	uint16_t wrverifysectorcountmode2only[2];
	uint16_t nvcachecapabilities;
	uint16_t nvcachesizelsw;
	uint16_t nvcachesizemsw;
	uint16_t nominalmediarotationrate;
	uint16_t reserved7;
	uint8_t  nvcacheestspinuptimeseconds;
	uint8_t  reserved8;
	uint8_t  wrverifysectorcountmode;
	uint8_t  reserved9;
	uint16_t reserved10;
	uint16_t transportmajorversion;
	uint16_t transportminorversion;
	uint16_t reserved11[6];
	uint32_t extendeduseraddressablesectors[2];
	uint16_t minblocksperdownloadmicrocodemode03;
	uint16_t maxblocksperdownloadmicrocodemode03;
	uint16_t reserved12[19];
	uint8_t  signature;
	uint8_t  checksum;
} __attribute__((__packed__));

#endif
