#include "modules/rapl.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "util/format.hpp"

#if (FMT_VERSION >= 80000)
#include <fmt/args.h>
#else
#include <fmt/core.h>
#endif

waybar::modules::Rapl::Rapl(const std::string& id, const Json::Value& config)
    : ALabel(config, "rapl", id, "{power}W", 10),
      sysfs_dir_(config["sysfs-dir"].isString() ? config["sysfs-dir"].asString() : "/sys/class/powercap/intel-rapl:0"),
      prev_package_energy_(0),
      prev_core_energy_(0),
      first_update_(true) {
  // 检查RAPL文件是否存在
  if (!std::filesystem::exists(getPackagePath()) || !std::filesystem::exists(getCorePath()) ||
      !std::filesystem::exists(getMaxEnergyRangePath())) {
    throw std::runtime_error("RAPL sysfs files not found");
  }

  thread_ = [this] {
    dp.emit();
    thread_.sleep_for(interval_);
  };
}

std::string waybar::modules::Rapl::getPackagePath() const {
  return sysfs_dir_ + "/energy_uj";
}

std::string waybar::modules::Rapl::getCorePath() const {
  return sysfs_dir_ + ":0/energy_uj";
}

std::string waybar::modules::Rapl::getMaxEnergyRangePath() const {
  return sysfs_dir_ + ":0/max_energy_range_uj";
}

auto waybar::modules::Rapl::update() -> void {
  // 读取当前能量值
  std::ifstream package_file(getPackagePath());
  std::ifstream core_file(getCorePath());

  if (!package_file.is_open() || !core_file.is_open()) {
    event_box_.hide();
    return;
  }

  uint64_t package_energy, core_energy;
  package_file >> package_energy;
  core_file >> core_energy;

  // 获取当前时间
  auto current_time = std::chrono::steady_clock::now();

  // 计算功耗
  double package_power = 0.0;
  double core_power = 0.0;

  if (!first_update_) {
    // 计算时间差（秒）
    std::chrono::duration<double> time_diff = current_time - prev_time_;
    double seconds = time_diff.count();

    if (seconds > 0) {
      // 计算能量差（微焦耳）
      uint64_t package_energy_diff = package_energy - prev_package_energy_;
      uint64_t core_energy_diff = core_energy - prev_core_energy_;

      // 处理计数器回绕
      uint64_t max_energy_range;
      std::ifstream max_energy_file(getMaxEnergyRangePath());
      if (max_energy_file.is_open()) {
        max_energy_file >> max_energy_range;
        if (package_energy_diff > max_energy_range / 2) {
          package_energy_diff += max_energy_range;
        }
        if (core_energy_diff > max_energy_range / 2) {
          core_energy_diff += max_energy_range;
        }
      }

      // 计算功耗（瓦特 = 焦耳/秒）
      package_power = (package_energy_diff / 1000000.0) / seconds;  // 转换为焦耳
      core_power = (core_energy_diff / 1000000.0) / seconds;        // 转换为焦耳
    }
  }

  // 更新上一次的值
  prev_package_energy_ = package_energy;
  prev_core_energy_ = core_energy;
  prev_time_ = current_time;
  first_update_ = false;

  // 获取状态基于功耗（转换为uint8_t用于getState）
  uint8_t power_state = static_cast<uint8_t>(std::min(package_power, 255.0));
  auto state = getState(power_state);

  // 格式化输出
  auto format = format_;
  if (format.empty()) {
    event_box_.hide();
  } else {
    event_box_.show();

    // 使用float_format4w格式化功耗值
    fmt::dynamic_format_arg_store<fmt::format_context> store;
    store.push_back(fmt::arg("power", float_format4w(package_power)));
    store.push_back(fmt::arg("core_power", float_format4w(core_power)));

    // 计算其他功耗（非核心部分）
    double other_power = package_power - core_power;
    store.push_back(fmt::arg("other_power", float_format4w(other_power)));

    label_.set_markup(fmt::vformat(format, store));

    // 设置工具提示
    if (tooltipEnabled()) {
      std::string tooltip = fmt::format("Package: {:.2f}W\nCore: {:.2f}W\nOther: {:.2f}W",
                                        package_power, core_power, other_power);
      label_.set_tooltip_text(tooltip);
    }
  }

  // 调用父类的update方法
  ALabel::update();
}