#ifndef GPRUSAK_DIGITS_H_
#define GPRUSAK_DIGITS_H_

#include <cstring>
#include "gprusak/string.h"

namespace gprusak {

struct Digits {
  const char *small = "0123456789abcdefghijklmnopqrstuvwxyz";
  const char *big =   "0123456789ABCDEFGHIJKLMNOPQRSTUVQXYZ";
  uint8_t dig[256];
  Digits()
  {
    memset(dig,255,sizeof(dig));
    for(uint8_t i=0; small[i]; ++i) dig[(uint8_t)small[i]] = i;
    for(uint8_t i=0; big[i]; ++i) dig[(uint8_t)big[i]] = i;
  }
  template<typename T> T parse(str s, uint8_t base) const
  { T v = 0; for(char c : s) v = v*base + dig[(uint8_t)c]; return v; }
  template<typename T> str print(T v, uint8_t base, size_t len) const
  { str s(len,0); for(size_t i=len; i--;){ s[i] = small[v%base]; v /= base; } return s; }
  template<typename T> str print(T v, uint8_t base) const
  { str s; while(v){ s = small[v%base]+s; v /= base; } return s.size() ? s : "0"; }
  template<typename T> str print_big(T v, uint8_t base, size_t len) const
  { str s(len,0); for(size_t i=len; i--;){ s[i] = big[v%base]; v /= base; } return s; }
  template<typename T> str print_big(T v, uint8_t base) const
  { str s; while(v){ s = big[v%base]+s; v /= base; } return s.size() ? s : "0"; }

  bool is_base(str s, uint8_t base) const
  { for(char c : s) if(dig[(uint8_t)c]>=base) return 0; return 1; }
};
inline const Digits & digits(){ static Digits D; return D; }

template<typename T> inline str dec(T v){ return digits().print(v,10); }
inline str hex32(uint32_t v){ return digits().print(v,16,8); }
inline str hex64(uint64_t v){ return digits().print(v,16,16); }

inline bool is_dec(const str &s){ return digits().is_base(s,10); }
inline bool is_hex(const str &s){ return digits().is_base(s,16); }

template<typename T = int> T parse_dec(const str &s){ return digits().parse<T>(s,10); }
template<typename T = int> T parse_hex(const str &s){ return digits().parse<T>(s,16); }

}  // namespace gprusak

#endif  // GPRUSAK_DIGITS_H_
