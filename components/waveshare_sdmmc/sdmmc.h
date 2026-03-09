#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "sdmmc_cmd.h"

namespace esphome {
namespace waveshare_sdmmc {

class WaveshareSDMMC : public Component {
 public:
  void setup() override;
  void loop() override;

  // Funções principais
  bool write_file(const char *path, const char *data);
  std::string read_file(const char *path);

  // Sensores
  void set_total_space_sensor(sensor::Sensor *s) { this->total_space_sensor_ = s; }
  void set_free_space_sensor(sensor::Sensor *s) { this->free_space_sensor_ = s; }
  void set_mounted_sensor(binary_sensor::BinarySensor *s) { this->mounted_sensor_ = s; }
  void set_last_error_sensor(text_sensor::TextSensor *s) { this->last_error_sensor_ = s; }
  void set_last_read_sensor(text_sensor::TextSensor *s) { this->last_read_sensor_ = s; }

  // Atualização manual
  void update_sensors();

  // Funções auxiliares
  size_t get_total_space();
  size_t get_free_space();
  bool is_initialized() const { return this->initialized_; }

  void set_last_error(const std::string &err) { this->last_error_ = err; }
  void set_last_read(const std::string &txt) { this->last_read_ = txt; }

 protected:
  sdmmc_card_t *card_{nullptr};
  bool initialized_{false};

  // Sensores
  sensor::Sensor *total_space_sensor_{nullptr};
  sensor::Sensor *free_space_sensor_{nullptr};
  binary_sensor::BinarySensor *mounted_sensor_{nullptr};
  text_sensor::TextSensor *last_error_sensor_{nullptr};
  text_sensor::TextSensor *last_read_sensor_{nullptr};

  // Armazenamento interno
  std::string last_error_;
  std::string last_read_;
};
class UpdateSensorsAction : public Action<> {
 public:
  UpdateSensorsAction(WaveshareSDMMC *parent) : parent_(parent) {}
  void play() override {
    this->parent_->update_sensors();
  }
 protected:
  WaveshareSDMMC *parent_;
};

}  // namespace waveshare_sdmmc
}  // namespace esphome
