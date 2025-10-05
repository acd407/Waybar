#pragma once

#include <fmt/format.h>

#include <cstdint>
#include <fstream>
#include <string>
#include <utility>

#include "ALabel.hpp"
#include "util/sleeper_thread.hpp"

namespace waybar::modules {

class Rapl : public ALabel {
 public:
  Rapl(const std::string &, const Json::Value &);
  virtual ~Rapl() = default;
  auto update() -> void override;

 private:
  std::string getPackagePath() const;
  std::string getCorePath() const;
  std::string getMaxEnergyRangePath() const;

  std::string sysfs_dir_;
  util::SleeperThread thread_;
  uint64_t prev_package_energy_;
  uint64_t prev_core_energy_;
  std::chrono::time_point<std::chrono::steady_clock> prev_time_;
  bool first_update_;
};

}  // namespace waybar::modules