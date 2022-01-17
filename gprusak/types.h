#ifndef GPRUSAK_TYPES_H_
#define GPRUSAK_TYPES_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <optional>

#define INL [[gnu::always_inline]]
#define INLL __attribute__((always_inline))

namespace gprusak::types {

using u8 = uint8_t;
using u64 = uint64_t;
using s64 = int64_t;
template<typename T> using vec = std::vector<T>;
template<typename T> using ptr = std::unique_ptr<T>;
template<typename T> INL static ptr<T> own(T *v){ return ptr<T>(v); }
using str = std::string;
using str32 = std::u32string;
using std::swap;

using Byte = uint8_t;
using Bytes = vec<Byte>;

INL inline str bytes_str(const Bytes &b) { return str(b.begin(),b.end()); }
INL inline Bytes str_bytes(const str &s) { return Bytes(s.begin(),s.end()); }

template<typename T> using opt = std::optional<T>;
template<typename T> INL static opt<T> just(const T &v){ return opt<T>(v); }

struct Nil {
  template<typename T> INL operator ptr<T>(){ return {}; }
  template<typename T> INL operator opt<T>(){ return {}; }
};

constexpr Nil nil;

template<typename F> struct Defer { F f; ~Defer(){ f(); } };
template<typename F> INL static Defer<F> defer(F f) { return {f}; }

} // namespace gprusak::types

namespace gprusak {
  using namespace gprusak::types;
}

#endif // GPRUSAK_TYPES_H_
