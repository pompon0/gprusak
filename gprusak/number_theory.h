#ifndef GPRUSAK_NUMBER_THEORY_H_
#define GPRUSAK_NUMBER_THEORY_H_

#include "gprusak/log.h"

namespace gprusak {

// uint64_t is not affected by the integer promotion rules (as long as size(int)<8),
// therefore it is safe to multiply these.
//
// WG14/N1124 6.3.1.1 
// http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf
// If an int can represent all values of the original type, the value is converted to an int;
// otherwise, it is converted to an unsigned int. These are called the integer
// promotions. All other types are unchanged by the integer promotions (...)
// The integer promotions are applied only: as part of the usual arithmetic conversions, to certain
// argument expressions, to the operands of the unary +, -, and ~ operators, and to both operands of the
// shift operators, as specified by their respective subclauses.
INL inline static uint64_t pow(uint64_t a, uint64_t b) {
  uint64_t res = 1;
  while(b) {
    if(b&1) res *= a;
    a *= a;
    b >>= 1;
  }
  return res;
}

// inv(a)*a % (1<<bits(T)) = 1
// T is an unsigned integer type.
template<typename T> T inv(T a) { FRAME("inv(%)",a);
  static_assert(T(-1)>T(0));
  if(!a%2) return 0; // TODO: throw error
  // By Fermat's little theorem:
  // a^totient(n) = 1 (%n)
  // a^{-1} = a^{totient(n)-1}
  //
  // For n = 1<<bits(T) we have totient(n) = n/2
  T tot = T(1)<<(sizeof(T)*8-1);
  return pow(a,tot-T(1));
}

}  // namespace gprusak

#endif  // GPRUSAK_NUMBER_THEORY_H_
