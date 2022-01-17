#ifndef GPRUSAK_LEX_H_
#define GPRUSAK_LEX_H_

#include "gprusak/types.h"

namespace gprusak {

template<typename T> struct _lex {
  INL static auto cmp(const T &a, const T &b){ return a<=>b; }
};

template<typename T> INL static auto lex(const T &a, const T &b){ return _lex<T>::cmp(a,b); }

template<> struct _lex<str> {
  INL static auto cmp(const str &a, const str &b){ return a.compare(b) <=> 0; }
};

template<typename T> struct _lex<opt<T>> {
  INL static auto cmp(const opt<T> &a, const opt<T> &b) {
    if(bool(a) && bool(b)) return lex(*a,*b);
    return bool(a) <=> bool(b);
  }
};

template<typename T> struct _lex<vec<T>> {
  INL static auto cmp(const vec<T> &a, const vec<T> &b) {
    size_t n = std::min(a.size(),b.size());
    for(size_t i=0; i<n; i++) if(auto x = lex(a[i],b[i]); x!=0) return x;
    return a.size()<=>b.size();
  }
};

}

#endif  // GPRUSAK_LEX_H_
