#pragma once

#include <fmt/format.h>
#include <glibmm/ustring.h>

#include <cmath>
#include <string>

class pow_format {
 public:
  pow_format(long long val, std::string&& unit, bool binary = false)
      : val_(val), unit_(unit), binary_(binary) {};

  long long val_;
  std::string unit_;
  bool binary_;
};

namespace fmt {
template <>
struct formatter<pow_format> {
  char spec = 0;
  int width = 0;

  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && *it == ':') ++it;
    if (it && (*it == '>' || *it == '<' || *it == '=')) {
      spec = *it;
      ++it;
    }
    if (it == end || *it == '}') return it;
    if ('0' <= *it && *it <= '9') {
      // We ignore it for now, but keep it for compatibility with
      // existing configs where the format for pow_format'ed numbers was
      // 'string' and specifications such as {:>9} were valid.
      // The rationale for ignoring it is that the only reason to specify
      // an alignment and a with is to get a fixed width bar, and ">" is
      // sufficient in this implementation.
#if FMT_VERSION < 80000
      width = parse_nonnegative_int(it, end, ctx);
#else
      width = detail::parse_nonnegative_int(it, end, -1);
#endif
    }
    return it;
  }

  template <class FormatContext>
  auto format(const pow_format& s, FormatContext& ctx) const -> decltype(ctx.out()) {
    const char* units[] = {"", "k", "M", "G", "T", "P", nullptr};

    auto base = s.binary_ ? 1024ull : 1000ll;
    auto fraction = (double)s.val_;

    int pow;
    for (pow = 0; units[pow + 1] != nullptr && fraction / base >= 1; ++pow) {
      fraction /= base;
    }

    auto number_width = 5              // coeff in {:.1f} format
                        + s.binary_;   // potential 4th digit before the decimal point
    auto max_width = number_width + 1  // prefix from units array
                     + s.binary_       // for the 'i' in GiB.
                     + s.unit_.length();

    const char* format;
    std::string string;
    switch (spec) {
      case '>':
        return fmt::format_to(ctx.out(), "{:>{}}", fmt::format("{}", s), max_width);
      case '<':
        return fmt::format_to(ctx.out(), "{:<{}}", fmt::format("{}", s), max_width);
      case '=':
        format = "{coefficient:<{number_width}.1f}{padding}{prefix}{unit}";
        break;
      case 0:
      default:
        format = "{coefficient:.1f}{prefix}{unit}";
        break;
    }
    return fmt::format_to(
        ctx.out(), fmt::runtime(format), fmt::arg("coefficient", fraction),
        fmt::arg("number_width", number_width),
        fmt::arg("prefix", std::string() + units[pow] + ((s.binary_ && pow) ? "i" : "")),
        fmt::arg("unit", s.unit_),
        fmt::arg("padding", pow         ? ""
                            : s.binary_ ? "  "
                                        : " "));
  }
};

// Glib ustirng support
template <>
struct formatter<Glib::ustring> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const Glib::ustring& value, FormatContext& ctx) const {
    return formatter<std::string>::format(static_cast<std::string>(value), ctx);
  }
};
}  // namespace fmt

// 辅助函数：格式化数值部分，使用与float_format4w相同的逻辑
inline std::string format_number_part(double value) {
  // 动态选择格式确保总长度=4字符（如 "0.2"、"33"、"100"）
  std::string formatted;
  if (value >= 100.0) {
    formatted = fmt::format("{:.0f}", std::round(value));
  } else if (value >= 10.0) {
    formatted = fmt::format("{:.1f}", value);
  } else {
    formatted = fmt::format("{:.2f}", value);
  }

  // 强制截断或填充到4字符（在前面填充空格，保持右对齐）
  if (formatted.length() > 4) {
    formatted.resize(4);
  } else if (formatted.length() < 4) {
    formatted.insert(0, 4 - formatted.length(), ' ');
  }

  return formatted;
}

class float_format4w {
 public:
  float_format4w(float value) : value_(value) {}

  float value_;
};

namespace fmt {
template <>
struct formatter<float_format4w> {
  // 解析格式字符串（保留接口但不处理参数）
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();  // 忽略所有格式选项
  }

  template <class FormatContext>
  auto format(const float_format4w& s, FormatContext& ctx) const -> decltype(ctx.out()) {
    // 使用辅助函数格式化数值，确保长度为4字符
    std::string formatted = format_number_part(s.value_);
    return fmt::format_to(ctx.out(), "{}", formatted);
  }
};
}  // namespace fmt

class pow_format5w {
 public:
  pow_format5w(uint64_t bytes, bool binary = false) : bytes_(bytes), binary_(binary) {}

  uint64_t bytes_;
  bool binary_;
};

namespace fmt {
template <>
struct formatter<pow_format5w> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <class FormatContext>
  auto format(const pow_format5w& s, FormatContext& ctx) const -> decltype(ctx.out()) {
    const char* units = "KMGTPE";
    int unit_idx = -1;
    auto size = static_cast<double>(s.bytes_);
    auto base = s.binary_ ? 1024.0 : 1000.0;

    if (size < 10.0) {
      return fmt::format_to(ctx.out(), "0.00K");
    }

    while (size >= base && unit_idx < 5) {
      size /= base;
      unit_idx++;
    }

    if (unit_idx < 0) {
      unit_idx = 0;
      size /= base;
    }

    // 使用与float_format4w相同的逻辑格式化数值部分
    std::string number_part = format_number_part(size);
    std::string unit_part = std::string(1, units[unit_idx]);
    std::string formatted = number_part + unit_part;

    return fmt::format_to(ctx.out(), "{}", formatted);
  }
};
}  // namespace fmt
