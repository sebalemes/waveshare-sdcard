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
    this->last_error_ = esp_err_to_name(ret);
    return;
  }

  ESP_LOGI(TAG, "SDMMC montado com sucesso!");
  this->initialized_ = true;
}

void WaveshareSDMMC::loop() {
  // Nada automático — tudo manual
}

bool WaveshareSDMMC::write_file(const char *path, const char *data) {
  if (!this->initialized_) {
    this->last_error_ = "SD não montado";
    return false;
  }

  FILE *f = fopen(path, "w");
  if (!f) {
    this->last_error_ = "Erro ao abrir arquivo";
    return false;
  }

  fprintf(f, "%s", data);
  fclose(f);

  this->last_error_.clear();
  return true;
}

std::string WaveshareSDMMC::read_file(const char *path) {
  if (!this->initialized_) {
    this->last_error_ = "SD não montado";
    return "";
  }

  FILE *f = fopen(path, "r");
  if (!f) {
    this->last_error_ = "Erro ao abrir arquivo";
    return "";
  }

  char buffer[512];
  std::string result;

  while (fgets(buffer, sizeof(buffer), f)) {
    result += buffer;
  }

  fclose(f);

  this->last_read_ = result;
  this->last_error_.clear();
  return result;
}

size_t WaveshareSDMMC::get_total_space() {
  if (!this->initialized_) return 0;

  FATFS *fs;
  DWORD fre_clust, fre_sect, tot_sect;

  f_getfree("0:", &fre_clust, &fs);
  tot_sect = (fs->n_fatent - 2) * fs->csize;
  return tot_sect * 512;
}

size_t WaveshareSDMMC::get_free_space() {
  if (!this->initialized_) return 0;

  FATFS *fs;
  DWORD fre_clust, fre_sect, tot_sect;

  f_getfree("0:", &fre_clust, &fs);
  fre_sect = fre_clust * fs->csize;
  return fre_sect * 512;
}

void WaveshareSDMMC::update_sensors() {
  if (this->mounted_sensor_)
    this->mounted_sensor_->publish_state(this->initialized_);

  if (!this->initialized_) {
    if (this->last_error_sensor_)
      this->last_error_sensor_->publish_state("SD não montado");
    return;
  }

  if (this->total_space_sensor_)
    this->total_space_sensor_->publish_state(this->get_total_space());

  if (this->free_space_sensor_)
    this->free_space_sensor_->publish_state(this->get_free_space());

  if (this->last_error_sensor_)
    this->last_error_sensor_->publish_state(this->last_error_);

  if (this->last_read_sensor_)
    this->last_read_sensor_->publish_state(this->last_read_);
}

}  // namespace waveshare_sdmmc
}  // namespace esphome
