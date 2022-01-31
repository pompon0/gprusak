#ifndef GPRUSAK_TYPES_H_
#define GPRUSAK_TYPES_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <optional>
#include <functional>
#include <mutex>

#define INL [[gnu::always_inline]]
#define INLL __attribute__((always_inline))

namespace gprusak::types {

using u8 = uint8_t;
using u64 = uint64_t;
using s64 = int64_t;
template<typename T> using vec = std::vector<T>;
template<typename T> using func = std::function<T>;
template<typename T> using ptr = std::unique_ptr<T>;
template<typename T> INL inline static ptr<T> own(T *v){ return ptr<T>(v); }
using str = std::string;
using str32 = std::u32string;
using std::swap;


using Byte = uint8_t;
using Bytes = vec<Byte>;

INL inline str bytes_str(const Bytes &b) { return str(b.begin(),b.end()); }
INL inline Bytes str_bytes(const str &s) { return Bytes(s.begin(),s.end()); }

template<typename T> using opt = std::optional<T>;
template<typename T> INL inline static opt<T> just(const T &v){ return opt<T>(v); }

struct Nil {
  template<typename T> INL operator ptr<T>() const { return {}; }
  template<typename T> INL operator opt<T>() const { return {}; }
};

constexpr Nil nil;

template<typename F> struct Defer { F f; ~Defer(){ f(); } };
template<typename F> INL inline static Defer<F> defer(F f) { return {f}; }

// function signature static comparison 
template<typename F> struct _sig : _sig<decltype(&F::operator())> {};
template<typename F, typename Ret, typename ...Args> struct _sig<Ret(F::*)(Args...) const> { using value = Ret(Args...); };

template<typename F> using sig = typename _sig<F>::value;
template<typename F, typename Sig> constexpr bool has_sig = std::is_same<sig<F>,Sig>::value;

template<typename M> INL inline auto lock(M &m){ return std::lock_guard<M>(m); }

} // namespace gprusak::types

namespace gprusak {
  using namespace gprusak::types;
}

#endif // GPRUSAK_TYPES_H_
