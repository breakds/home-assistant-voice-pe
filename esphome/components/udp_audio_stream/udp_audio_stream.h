#pragma once

#include "esphome/core/component.h"
#include "esphome/components/microphone/microphone.h"

#include <lwip/sockets.h>

namespace esphome {
namespace udp_audio_stream {

class UDPAudioStream : public Component {
 public:
  void setup() override;
  void loop() override;

  void set_microphone(microphone::Microphone *mic) { this->mic_ = mic; }
  void set_target_ip(const std::string &ip) { this->target_ip_ = ip; }
  void set_target_port(uint16_t port) { this->target_port_ = port; }

  void start();
  void stop();
  bool is_streaming() const { return this->streaming_; }

 protected:
  void on_audio_data_(const std::vector<uint8_t> &data);

  microphone::Microphone *mic_{nullptr};
  std::string target_ip_;
  uint16_t target_port_{5555};
  int socket_fd_{-1};
  struct sockaddr_in dest_addr_{};
  bool streaming_{false};
};

}  // namespace udp_audio_stream
}  // namespace esphome
