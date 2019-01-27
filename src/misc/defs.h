#pragma once

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <cstddef>

#include <cstring>
#include <string> // TODO: replace?
#include <string_view>

#include <sstream>
#include <fstream>

#include <algorithm>

#include <memory>

#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <optional>

#include "enum.h"

// #include <string_view>

// unsigned integer types:
                            using Uint8        = uint8_t;
                            using Uint16       = uint16_t;
                            using Uint32       = uint32_t;
                            using Uint64       = uint64_t;
// signed integer types:
                            using Int8         = int8_t;
                            using Int16        = int16_t;
                            using Int32        = int32_t;
                            using Int64        = int64_t;
// floating point types:
                            using Float32      = float;
                            using Float64      = double;
// string types:
                            using String       = std::basic_string<char>;
                            using Str          = String;
                            using StringView   = std::basic_string_view<char>;
                            using StrView      = StringView;
// stream types:
                            using Ifstream     = std::ifstream;
                            using StringStream = std::stringstream;
// index type:
                            using Index        = int;
                            using Idx          = Index;
// size type:
                            using Size         = size_t;
// boolean type:
                            using Bool         = bool;
// array types:
template <class T>          using Vector       = std::vector<T>;
template <class T>          using Vec          = Vector<T>;
template <class T, Size N>  using Array        = std::array<T,N>;
template <class T, Size N>  using Arr          = Array<T,N>;
// smart pointer types:
template <class T>          using SharedPtr    = std::shared_ptr<T>;
template <class T>          using UniquePtr    = std::unique_ptr<T>;
template <class T>          using WeakPtr      = std::weak_ptr<T>;
// container types:
template <class K, class V> using Map          = std::map<K,V>;
template <class K, class V> using HashMap      = std::unordered_map<K,V>;
// optional type:
template <class T>          using Optional     = std::optional<T>;
template <class T>          using Opt          = Optional<T>;

constexpr Idx  const   invalid_idx = -1;
constexpr char const * invalid_str = "ERROR";
