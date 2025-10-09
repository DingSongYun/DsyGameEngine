#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

// 基础整数类型（已经由cstdint提供，但可以添加别名）
// uint8_t, uint16_t, uint32_t, uint64_t
// int8_t, int16_t, int32_t, int64_t

// 浮点类型
using float32 = float;
using float64 = double;

// 字符类型
using char8 = char;
using char16 = char16_t;
using char32 = char32_t;