#include "sdmmc.h"
#include "esphome/core/log.h"

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_vfs_fat.h"
#include "ff.h"

namespace esphome {
namespace waveshare_sdmmc {

static const char *TAG = "waveshare_sdmmc";

void WaveshareSDMMC::setup() {
  ESP_LOGI(TAG, "Inicializando SDMMC...");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  // Ajuste aqui se a placa exigir pinos específicos
  // Exemplo:
  // slot_config.width = 4;
  // slot_config.clk = GPIO_NUM_xx;
  // slot_config.cmd = GPIO_NUM_xx;
  // slot_config.d0  = GPIO_NUM_xx;
  // slot_config.d1  = GPIO_NUM_xx;
  // slot_config.d2  = GPIO_NUM_xx;
  // slot_config.d3  = GPIO_NUM_xx;

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
  this->last_error_.clear();
}

void WaveshareSDMMC::loop() {
  // Nada automático — tudo manual
}

bool WaveshareSDMMC::write_file(const char *path, const char *data) {
  if (!this->initialized_) {
    this->last_error_ = "SD não montado";
    return false;
  }

  if (path == nullptr || data == nullptr) {
    this->last_error_ = "Parâmetro inválido";
    return false;
  }

  FILE *f = fopen(path, "w");
  if (!f) {
    this->last_error_ = "Erro ao abrir arquivo para escrita";
    return false;
  }

  int written = fprintf(f, "%s", data);
  fclose(f);

  if (written < 0) {
    this->last_error_ = "Erro ao gravar arquivo";
    return false;
  }

  this->last_error_.clear();
  return true;
}

std::string WaveshareSDMMC::read_file(const char *path) {
  if (!this->initialized_) {
    this->last_error_ = "SD não montado";
    return "";
  }

  if (path == nullptr) {
    this->last_error_ = "Parâmetro inválido";
    return "";
  }

  FILE *f = fopen(path, "r");
  if (!f) {
    this->last_error_ = "Erro ao abrir arquivo para leitura";
    return "";
  }

  char buffer[512];
  std::string result;

  while (fgets(buffer, sizeof(buffer), f) != nullptr) {
    result += buffer;
  }

  fclose(f);

  this->last_read_ = result;
  this->last_error_.clear();
  return result;
}

size_t WaveshareSDMMC::get_total_space() {
  if (!this->initialized_) {
    return 0;
  }

  FATFS *fs = nullptr;
  DWORD fre_clust = 0;

  FRESULT res = f_getfree("0:", &fre_clust, &fs);
  if (res != FR_OK || fs == nullptr) {
    this->last_error_ = "Erro ao obter espaço total";
    return 0;
  }

  DWORD tot_sect = (fs->n_fatent - 2) * fs->csize;
  return static_cast<size_t>(tot_sect) * 512ULL;
}

size_t WaveshareSDMMC::get_free_space() {
  if (!this->initialized_) {
    return 0;
  }

  FATFS *fs = nullptr;
  DWORD fre_clust = 0;

  FRESULT res = f_getfree("0:", &fre_clust, &fs);
  if (res != FR_OK || fs == nullptr) {
    this->last_error_ = "Erro ao obter espaço livre";
    return 0;
  }

  DWORD fre_sect = fre_clust * fs->csize;
  return static_cast<size_t>(fre_sect) * 512ULL;
}

void WaveshareSDMMC::update_sensors() {
  if (this->mounted_sensor_ != nullptr) {
    this->mounted_sensor_->publish_state(this->initialized_);
  }

  if (!this->initialized_) {
    if (this->last_error_sensor_ != nullptr) {
      this->last_error_sensor_->publish_state("SD não montado");
    }
    return;
  }

  if (this->total_space_sensor_ != nullptr) {
    this->total_space_sensor_->publish_state(this->get_total_space());
  }

  if (this->free_space_sensor_ != nullptr) {
    this->free_space_sensor_->publish_state(this->get_free_space());
  }

  if (this->last_error_sensor_ != nullptr) {
    this->last_error_sensor_->publish_state(this->last_error_);
  }

  if (this->last_read_sensor_ != nullptr) {
    this->last_read_sensor_->publish_state(this->last_read_);
  }
}

}  // namespace waveshare_sdmmc
}  // namespace esphome
