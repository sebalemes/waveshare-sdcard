#include "sdmmc.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "ff.h"

namespace esphome {
namespace waveshare_sdmmc {

static const char *TAG = "waveshare_sdmmc";

// Pinos do TF Card na Waveshare ESP32-S3-ETH-8DI-8RO
static constexpr gpio_num_t PIN_NUM_MISO = GPIO_NUM_45;
static constexpr gpio_num_t PIN_NUM_MOSI = GPIO_NUM_47;
static constexpr gpio_num_t PIN_NUM_CLK  = GPIO_NUM_48;
static constexpr gpio_num_t PIN_NUM_CS   = GPIO_NUM_21;

void WaveshareSDMMC::setup() {
  ESP_LOGI(TAG, "Inicializando TF Card via SDSPI...");

  // Inicializa CS
  gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_NUM_CS, 1);
  delay(10);

  spi_bus_config_t bus_cfg = {};
  bus_cfg.mosi_io_num = PIN_NUM_MOSI;
  bus_cfg.miso_io_num = PIN_NUM_MISO;
  bus_cfg.sclk_io_num = PIN_NUM_CLK;
  bus_cfg.quadwp_io_num = GPIO_NUM_NC;
  bus_cfg.quadhd_io_num = GPIO_NUM_NC;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
  bus_cfg.data4_io_num = GPIO_NUM_NC;
  bus_cfg.data5_io_num = GPIO_NUM_NC;
  bus_cfg.data6_io_num = GPIO_NUM_NC;
  bus_cfg.data7_io_num = GPIO_NUM_NC;
#endif

  bus_cfg.max_transfer_sz = 4000;

  esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao inicializar SPI bus: %s", esp_err_to_name(ret));
    this->initialized_ = false;
    this->last_error_ = esp_err_to_name(ret);
    return;
  }

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = SPI2_HOST;
  host.max_freq_khz = 20000;

  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.host_id = SPI2_HOST;
  slot_config.gpio_cs = PIN_NUM_CS;

  esp_vfs_fat_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };

  ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &this->card_);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao montar SDSPI: %s", esp_err_to_name(ret));
    this->initialized_ = false;
    this->last_error_ = esp_err_to_name(ret);
    spi_bus_free(SPI2_HOST);
    return;
  }

  ESP_LOGI(TAG, "TF Card montado com sucesso em /sdcard");

  this->initialized_ = true;
  this->last_error_.clear();
}

void WaveshareSDMMC::loop() {
  // Nada automático
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
  DWORD fre_clust;

  if (f_getfree("0:", &fre_clust, &fs) != FR_OK) {
    this->last_error_ = "Erro espaço total";
    return 0;
  }

  DWORD tot_sect = (fs->n_fatent - 2) * fs->csize;

  return (size_t) tot_sect * 512ULL;
}

size_t WaveshareSDMMC::get_free_space() {

  if (!this->initialized_) return 0;

  FATFS *fs;
  DWORD fre_clust;

  if (f_getfree("0:", &fre_clust, &fs) != FR_OK) {
    this->last_error_ = "Erro espaço livre";
    return 0;
  }

  DWORD fre_sect = fre_clust * fs->csize;

  return (size_t) fre_sect * 512ULL;
}

void WaveshareSDMMC::update_sensors() {

  if (!this->initialized_) {

    if (this->last_error_sensor_ != nullptr)
      this->last_error_sensor_->publish_state("SD não montado");

    return;
  }

  if (this->total_space_sensor_ != nullptr)
    this->total_space_sensor_->publish_state(this->get_total_space());

  if (this->free_space_sensor_ != nullptr)
    this->free_space_sensor_->publish_state(this->get_free_space());

  if (this->last_error_sensor_ != nullptr)
    this->last_error_sensor_->publish_state(this->last_error_);

  if (this->last_read_sensor_ != nullptr)
    this->last_read_sensor_->publish_state(this->last_read_);
}

}  // namespace waveshare_sdmmc
}  // namespace esphome
