#pragma once

#include "esphome/core/component.h"
#include "sdmmc_cmd.h"

namespace esphome {
namespace waveshare_sdmmc {

class WaveshareSDMMC : public Component {
 public:
  void setup() override;
  void loop() override;

  bool write_file(const char *path, const char *data);
  std::string read_file(const char *path);

 protected:
  sdmmc_card_t *card_{nullptr};
  bool initialized_{false};
};

}  // namespace waveshare_sdmmc
}  // namespace esphome
