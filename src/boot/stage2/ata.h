#ifndef _STAGE2_ATA
#define _STAGE2_ATA

#include <stdint.h>

/* From https://wiki.osdev.org/ATA_read/write_sectors */
/* and  https://wiki.osdev.org/ATA_PIO_Mode */

/* checks if ata is working properly and returns max sectors */
uint32_t ata_init(void);
/* flush hardware write cache, done after each write */
void ata_cache_flush(void);
/* perform software reset */
void ata_reset(void);
/* read sectors to some address */
void ata_read (uint32_t lba, uint8_t sectors, void *dst);
/* write sectors to disk */
void ata_write(uint32_t lba, uint8_t sectors, void *src);
/* returns 1 if ata encountered an error, 0 otherwise */
int ata_check_error(void);
/* prints error encountered */
void ata_error(void);

/* https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data */
/* not sure how reliable this is */
struct __attribute__((__packed__)) ata_identify
{
	uint16_t configuration_flags;
	uint16_t cylinders;
	uint16_t specific_config;
	uint16_t heads;
	uint16_t retired0[2];
	uint16_t sectors_per_track;
	uint16_t vendor_unique0[3];
	uint8_t  serial[20];
	uint16_t retired1[2];
	uint16_t obsolete0;
	uint8_t  firmware_revision[8];
	uint8_t  model_number[40];
	uint8_t  maximum_block_transfer;
	uint8_t  vendor_unique1;
	uint16_t trusted_computing;
	uint16_t capability_flags;
	uint16_t reserved0;
	uint16_t obsolete1[2];
	uint16_t translation_freefall;
	uint16_t current_cylinders;
	uint16_t current_heads;
	uint16_t current_sectors_per_track;
	uint32_t current_sector_capacity;
	uint8_t  current_multisector_setting;
	uint8_t  command_flags;
	uint32_t user_addressable_sectors;
	uint16_t obsolete2;
	uint8_t  multiword_dma_support;
	uint8_t  multiword_dma_active;
	uint8_t  advanced_pio;
	uint8_t  reserved1;
	uint16_t min_umxfer_cycle_time;
	uint16_t recommended_umxfer_cycle_time;
	uint16_t minimum_pio_cycle_time;
	uint16_t minimum_pio_cycle_time_iordy;
	uint16_t additional_supported;
	uint16_t reserved2[5];
	uint8_t  queue_depth;
	uint8_t  reserved3;
	uint32_t serial_ata_capabilities;
	uint16_t serial_ata_features_supported;
	uint16_t serial_ata_features_enabled;
	uint16_t major_revision;
	uint16_t minor_revision;
	uint16_t command_set_support[3];
	uint16_t command_set_enabled[3];
	uint8_t  ultra_dma_support;
	uint8_t  ultra_dma_active;
	uint16_t normal_security_erase_unit;
	uint16_t extended_security_erase_unit;
	uint8_t  current_apm_level;
	uint8_t  reserved4;
	uint16_t master_password_id;
	uint16_t hardware_reset_result;
	uint8_t  current_acoustic_value;
	uint8_t  recommended_acoustic_value;
	uint16_t stream_min_request_size;
	uint16_t streaming_transfer_time_dma;
	uint16_t streaming_access_latency_dmapio;
	uint32_t streaming_perf_granularity;
	uint32_t max_48bit_lba[2];
	uint16_t streaming_transfer_time;
	uint16_t dsm_cap;
	uint16_t physical_logical_sector_size;
	uint16_t inter_seek_delay;
	uint8_t  world_wide_name[8];
	uint16_t reserved_world_wide_name128[4];
	uint16_t reserved_tlc_technical_report;
	uint16_t words_per_logical_sector[2];
	uint16_t command_set_support_extensions;
	uint16_t command_set_active_extensions;
	uint16_t reserved_for_extended_support_and_active[6];
	uint16_t msn_support;
	uint16_t security_status;
	uint16_t reserved5[31];
	uint16_t cfa_power_mode1;
	uint16_t reserved_cfa[7];
	uint16_t nominal_form_factor;
	uint16_t data_set_management_feature;
	uint16_t additional_product_id[4];
	uint16_t reserved_cfa1[2];
	uint16_t current_media_serial[30];
	uint16_t sct_command_transport;
	uint16_t reserved6[2];
	uint16_t block_alignment;
	uint16_t wr_verify_sector_count_mode3_only[2];
	uint16_t wr_verify_sector_count_mode2_only[2];
	uint16_t nv_cache_capabilities;
	uint16_t nv_cache_size_lsw;
	uint16_t nv_cache_size_msw;
	uint16_t nominal_media_rotation_rate;
	uint16_t reserved7;
	uint8_t  nv_cache_est_spinup_time_seconds;
	uint8_t  reserved8;
	uint8_t  wr_verify_sector_count_mode;
	uint8_t  reserved9;
	uint16_t reserved10;
	uint16_t transport_major_version;
	uint16_t transport_minor_version;
	uint16_t reserved11[6];
	uint32_t extended_user_addressable_sectors[2];
	uint16_t min_blocks_per_download_microcode_mode03;
	uint16_t max_blocks_per_download_microcode_mode03;
	uint16_t reserved12[19];
	uint8_t  signature;
	uint8_t  checksum;
};

#endif
