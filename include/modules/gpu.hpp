#pragma once

#include <fmt/format.h>

#include <fstream>
#include <string>
#include <unordered_map>

#include "ALabel.hpp"
#include "util/sleeper_thread.hpp"

namespace waybar::modules {

class Gpu : public ALabel {
 public:
  Gpu(const std::string&, const Json::Value&);
  virtual ~Gpu() = default;
  auto update() -> void override;

 private:
  void parseGpuInfo();
  std::string getGpuUsage();
  std::string getVramUsed();

  std::string gpu_usage_path_;
  std::string vram_used_path_;
  std::string gpu_usage_;
  std::string vram_used_;
  std::unordered_map<std::string, std::string> gpuinfo_;

  util::SleeperThread thread_;
};

}  // namespace waybar::modules
