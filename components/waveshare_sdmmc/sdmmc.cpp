#include "sdmmc.h"
#include "esphome/core/log.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_vfs_fat.h"

namespace esphome {
namespace waveshare_sdmmc {

static const char *TAG = "waveshare_sdmmc";

void WaveshareSDMMC::setup() {
  ESP_LOGI(TAG, "Inicializando SDMMC...");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };

  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &this->card_);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao montar SDMMC: %s", esp_err_to_name(ret));
    this->initialized_ = false;
    return;
  }

  ESP_LOGI(TAG, "SDMMC montado com sucesso!");
  this->initialized_ = true;
}

void WaveshareSDMMC::loop() {
  // opcional
}

bool WaveshareSDMMC::write_file(const char *path, const char *data) {
  if (!this->initialized_) return false;

  FILE *f = fopen(path, "w");
  if (!f) return false;

  fprintf(f, "%s", data);
  fclose(f);
  return true;
}

std::string WaveshareSDMMC::read_file(const char *path) {
  if (!this->initialized_) return "";

  FILE *f = fopen(path, "r");
  if (!f) return "";

  char buffer[512];
  std::string result;

  while (fgets(buffer, sizeof(buffer), f)) {
    result += buffer;
  }

  fclose(f);
  return result;
}

}  // namespace waveshare_sdmmc
}  // namespace esphome
