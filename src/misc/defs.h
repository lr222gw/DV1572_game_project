#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <cassert>

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <cstddef>

#include <cstring>
#include <string>
#include <string_view>

#include <sstream>
#include <fstream>
#include <iostream>

#include <utility>
#include <functional>
#include <algorithm>

#include <utility>
#include <memory>

#include <array>
#include <vector>
#include <unordered_map>
#include <map>

//#include <optional>

#include "enum.h"

#include <GL/glew.h> // <- always include before gl.h & glfw3.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "stb_image.h"


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
// byte type:
                            using Byte         = Uint8;
// glm vectors:
                            using Uvec2        = glm::uvec2;
                            using Uvec3        = glm::uvec3;
                            using Uvec4        = glm::uvec4;
                            using Vec2         = glm::vec2;
                            using Vec3         = glm::vec3;
                            using Vec4         = glm::vec4;
// glm matrices:
                            using Mat2         = glm::mat2;
                            using Mat3         = glm::mat3;
                            using Mat4         = glm::mat4;
// gtc quaternion:
                            using Quat         = glm::quat;
// floating point types:
                            using Float32      = float;
                            using Float64      = double;
// string types:
                            using String       = std::basic_string<char>;
                            using StringView   = std::basic_string_view<char>;
// stream types:
                            using IfStream     = std::ifstream;
                            using StringStream = std::stringstream;
// index type:
                            using Index        = int;
// size type:
                            using Size         = size_t;
// boolean type:
                            using Bool         = bool;
// function type:
template< class R, class... Args >
                            using Function     = std::function<R(Args...)>;
// array types:
template <class T>          using Vector       = std::vector<T>;
template <class T, Size N>  using Array        = std::array<T,N>;
// smart pointer types:
template <class T>          using SharedPtr    = std::shared_ptr<T>;
template <class T>          using UniquePtr    = std::unique_ptr<T>;
template <class T>          using WeakPtr      = std::weak_ptr<T>;
// container types:
template <class K, class V> using Map          = std::map<K,V>;
template <class K, class V> using HashMap      = std::unordered_map<K,V>;
// optional type:
// template <class T>          using Optional     = std::optional<T>;

//constexpr Idx  const   invalid_idx = -1;
constexpr char const * invalid_str = "ERROR";
