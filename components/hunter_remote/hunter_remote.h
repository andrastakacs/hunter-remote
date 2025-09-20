#pragma once
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#ifdef USE_ESP32
#include "esphome/core/hal.h"
#endif

namespace esphome {
namespace hunter_remote {

class HunterRemote : public Component {
 public:
  void set_pin(GPIOPin *pin) { pin_ = pin; }
  void setup() override {
    pin_->setup();
  }
  void loop() override;
  void dump_config() override;
  void startProgram(int num);
  void stopZone(int zone);
  void startZone(int zone, int time);
 protected:
  GPIOPin *pin_;
  void sendLow();
  void sendHigh();
  void writeBus(std::vector<std::uint8_t> buffer, bool extrabit);
  void hunterBitfield(std::vector<std::uint8_t> &bits, std::uint8_t pos, std::uint8_t val, std::uint8_t len);
};


}  // namespace empty_component
}  // namespace esphome