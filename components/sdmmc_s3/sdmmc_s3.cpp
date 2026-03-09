#include "sdmmc_s3.h"
#include "esphome/core/log.h"

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_vfs_fat.h"

namespace esphome {
namespace sdmmc_s3 {

static const char *TAG = "sdmmc_s3";

void SDMMCS3::setup() {
  ESP_LOGI(TAG, "Inicializando SDMMC...");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.flags = SDMMC_HOST_FLAG_1BIT;

  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 1;

  slot_config.clk = GPIO_NUM_48;
  slot_config.cmd = GPIO_NUM_47;
  slot_config.d0  = GPIO_NUM_45;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };

  sdmmc_card_t *card;

  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao montar SDMMC: %s", esp_err_to_name(ret));
    mounted_ = false;
    return;
  }

  mounted_ = true;

  FATFS *fs;
  DWORD fre_clust, fre_sect, tot_sect;

  f_getfree("0:", &fre_clust, &fs);

  tot_sect = (fs->n_fatent - 2) * fs->csize;
  fre_sect = fre_clust * fs->csize;

  total_bytes_ = tot_sect * 512;
  used_bytes_  = total_bytes_ - (fre_sect * 512);

  ESP_LOGI(TAG, "SDMMC montado com sucesso");
}

void SDMMCS3::dump_config() {
  ESP_LOGCONFIG(TAG, "SDMMC S3:");
  ESP_LOGCONFIG(TAG, "  Montado: %s", mounted_ ? "Sim" : "Não");
  if (mounted_) {
    ESP_LOGCONFIG(TAG, "  Total: %u bytes", (unsigned) total_bytes_);
    ESP_LOGCONFIG(TAG, "  Usado: %u bytes", (unsigned) used_bytes_);
  }
}

}  // namespace sdmmc_s3
}  // namespace esphome
