#include "modules/gpu.hpp"

#include <fstream>
#include <sstream>

waybar::modules::Gpu::Gpu(const std::string& id, const Json::Value& config)
    : ALabel(config, "gpu", id, "{}%", 30) {
  // Set default paths for GPU usage and VRAM usage
  gpu_usage_path_ = config_["gpu-usage-path"].isString()
                        ? config_["gpu-usage-path"].asString()
                        : "/sys/class/drm/card1/device/gpu_busy_percent";

  vram_used_path_ = config_["vram-used-path"].isString()
                        ? config_["vram-used-path"].asString()
                        : "/sys/class/drm/card1/device/mem_info_vram_used";

  thread_ = [this] {
    dp.emit();
    thread_.sleep_for(interval_);
  };
}

auto waybar::modules::Gpu::update() -> void {
  parseGpuInfo();

  std::string gpu_usage = getGpuUsage();
  std::string vram_used = getVramUsed();

  // Convert VRAM usage to GB if it's in bytes
  double vram_gb = 0.0;
  try {
    unsigned long vram_bytes = std::stoul(vram_used);
    vram_gb = vram_bytes / (1024.0 * 1024.0 * 1024.0);
  } catch (const std::exception& e) {
    // If conversion fails, just use the raw value
    vram_gb = 0.0;
  }

  int gpu_usage_int = 0;
  try {
    gpu_usage_int = std::stoi(gpu_usage);
  } catch (const std::exception& e) {
    // If conversion fails, just use 0
    gpu_usage_int = 0;
  }

  auto format = format_;
  auto state = getState(gpu_usage_int);
  if (!state.empty() && config_["format-" + state].isString()) {
    format = config_["format-" + state].asString();
  }

  // Check if we should use format-alt
  if (alt_ && config_["format-alt"].isString()) {
    format = config_["format-alt"].asString();
  }
  // Check for state-specific format-alt
  if (alt_ && !state.empty() && config_["format-alt-" + state].isString()) {
    format = config_["format-alt-" + state].asString();
  }

  if (format.empty()) {
    event_box_.hide();
  } else {
    event_box_.show();
    auto icons = std::vector<std::string>{state};
    label_.set_markup(fmt::format(
        fmt::runtime(format), gpu_usage_int, fmt::arg("icon", getIcon(gpu_usage_int, icons)),
        fmt::arg("gpuUsage", gpu_usage_int), fmt::arg("vramUsed", vram_gb),
        fmt::arg("vramUsedRaw", vram_used)));
  }

  if (tooltipEnabled()) {
    if (config_["tooltip-format"].isString()) {
      auto tooltip_format = config_["tooltip-format"].asString();
      label_.set_tooltip_text(fmt::format(
          fmt::runtime(tooltip_format), gpu_usage_int, fmt::arg("gpuUsage", gpu_usage_int),
          fmt::arg("vramUsed", vram_gb), fmt::arg("vramUsedRaw", vram_used)));
    } else {
      label_.set_tooltip_text(fmt::format("GPU: {}% VRAM: {:.2f}GB", gpu_usage_int, vram_gb));
    }
  }

  // Call parent update
  ALabel::update();
}

void waybar::modules::Gpu::parseGpuInfo() {
  // This method can be extended if more GPU information needs to be parsed
  gpuinfo_.clear();
}

std::string waybar::modules::Gpu::getGpuUsage() {
  std::ifstream file(gpu_usage_path_);
  if (file.is_open()) {
    std::string line;
    if (std::getline(file, line)) {
      // Trim whitespace
      line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
      line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
      return line;
    }
  }
  return "0";
}

std::string waybar::modules::Gpu::getVramUsed() {
  std::ifstream file(vram_used_path_);
  if (file.is_open()) {
    std::string line;
    if (std::getline(file, line)) {
      // Trim whitespace
      line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
      line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
      return line;
    }
  }
  return "0";
}
