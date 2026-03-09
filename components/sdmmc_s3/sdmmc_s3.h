#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sdmmc {

class SDMMC : public Component {
 public:
  void setup() override;
  void dump_config() override;

  bool mounted_{false};
  size_t total_bytes_{0};
  size_t used_bytes_{0};
};

}  // namespace sdmmc
}  // namespace esphome
