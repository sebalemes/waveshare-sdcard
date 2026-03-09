#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sdmmc_s3 {

class SDMMCS3 : public Component {
 public:
  void setup() override;
  void dump_config() override;

  bool mounted_{false};
  size_t total_bytes_{0};
  size_t used_bytes_{0};
};

}  // namespace sdmmc_s3
}  // namespace esphome
