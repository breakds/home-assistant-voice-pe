#include "udp_audio_stream.h"
#include "esphome/core/log.h"

#include <arpa/inet.h>
#include <lwip/sockets.h>
#include <cerrno>

namespace esphome {
namespace udp_audio_stream {

static const char *TAG = "udp_audio_stream";

void UDPAudioStream::setup() {
  // Socket creation deferred to start() - network not ready during setup
  ESP_LOGI(TAG, "UDP Audio Stream configured, target: %s:%d",
           this->target_ip_.c_str(), this->target_port_);
}

void UDPAudioStream::loop() {
  // Nothing needed in loop - data sent via callback
}

void UDPAudioStream::start() {
  if (this->streaming_) {
    ESP_LOGW(TAG, "Already streaming");
    return;
  }

  // Create UDP socket (deferred from setup to ensure network is ready)
  if (this->socket_fd_ < 0) {
    this->socket_fd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->socket_fd_ < 0) {
      ESP_LOGE(TAG, "Failed to create socket: %d", errno);
      return;
    }

    // Configure destination address
    memset(&this->dest_addr_, 0, sizeof(this->dest_addr_));
    this->dest_addr_.sin_family = AF_INET;
    this->dest_addr_.sin_port = htons(this->target_port_);
    inet_aton(this->target_ip_.c_str(), &this->dest_addr_.sin_addr);
  }

  this->mic_->add_data_callback([this](const std::vector<uint8_t> &data) {
    this->on_audio_data_(data);
  });
  this->streaming_ = true;
  ESP_LOGI(TAG, "Started streaming audio to %s:%d",
           this->target_ip_.c_str(), this->target_port_);
}

void UDPAudioStream::stop() {
  if (!this->streaming_) {
    return;
  }
  this->streaming_ = false;
  ESP_LOGI(TAG, "Stopped streaming");
}

void UDPAudioStream::on_audio_data_(const std::vector<uint8_t> &data) {
  if (!this->streaming_ || this->socket_fd_ < 0) {
    return;
  }

  // Send raw audio bytes directly (no header)
  ssize_t sent = sendto(this->socket_fd_, data.data(), data.size(), 0,
                        (struct sockaddr *)&this->dest_addr_, sizeof(this->dest_addr_));
  if (sent < 0) {
    ESP_LOGW(TAG, "Failed to send audio data: %d", errno);
  }
}

}  // namespace udp_audio_stream
}  // namespace esphome
