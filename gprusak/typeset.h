#ifndef GPRUSAK_TYPESET_H_
#define GPRUSAK_TYPESET_H_

namespace gprusak {

template<typename ET> struct Has { protected: enum { OK = 1 }; };

template<typename ...T> struct TypeSet : private Has<T>... {
  // TODO: this is actually a hack, in a sense that it won't evaluate if X is not in T, i.e. it will never return "false".
  template<typename ...X> static constexpr bool contains(){ return (Has<X>::OK && ...); }
  template<typename A> INL static void foreach(A &&a){ (a.template run<T>(),...); }
};

}  // namespace gprusak

#endif  // GPRUSAK_TYPESET_H_
