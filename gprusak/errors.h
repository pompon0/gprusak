#ifndef GPRUSAK_ERRORS_H_
#define GPRUSAK_ERRORS_H_

#include <memory>

#include "gprusak/types.h"
#include "gprusak/string.h"

namespace gprusak {

/*template<typename ...ETs>  struct [[nodiscard]] Error {
  Error(){}
  template<typename ET> INL Error(ptr<ET> &&p) {
    static_assert(TypeSet<ETs...>::template contains<ET>());
    eg = own(static_cast<ErrGeneric*>(p.release()));
  }
  template<typename ...Xs> Error(Error<Xs...> &&e) : eg(std::move(e.eg)) {
    static_assert(TypeSet<ETs...>::template contains<Xs...>());
  }
  template<typename ET> [[nodiscard]] INL ptr<ET> is() {
    static_assert(TypeSet<ETs...>::template contains<ET>());
    if(dynamic_cast<ET*>(eg.get())) return own((ET*)eg.release());
    return {};
  }
  template<typename ...Xs> friend struct Error;
  INL operator bool() const { return bool(eg); }
  INL str show() { return eg->show(); }

  template<typename T> struct [[nodiscard]] Or {
    template<typename ET> INL Or(ptr<ET> &&p) : Or(Error(std::move(p))) {}
    INL Or(Error &&err) : err(std::move(err)) {}
    INL Or(T &&_val) : val(std::move(_val)) {}
    INL Or(const T &_val) : val(_val) {}
    T val;
    Error err;
  };
private:
  ptr<ErrGeneric> eg;
};*/

template<typename ET> struct Error;

struct ErrGeneric {
  virtual ~ErrGeneric(){}
  virtual str show() const = 0;
  virtual std::shared_ptr<const ErrGeneric> cause() const { return 0; }
protected:
  template<typename> friend struct Error;
  template<typename ET> INL static Error<ET> New(ET *p) { return std::shared_ptr<const ET>(p); }
  template<typename ET> INL static std::shared_ptr<const ET> raw(Error<ET> err) { return err.err; }
};

template<typename ET = ErrGeneric> struct [[nodiscard]] Error {
  static_assert(std::is_base_of_v<ErrGeneric,ET>);
  INL Error(){}
  template<typename X> INL Error(Error<X> e) : err(e.err) {}
  template<typename X> INL Error<X> is() const {
    static_assert(std::is_base_of_v<ET,X>);
    for(auto y = err; y; y = y->cause()) if(auto x = std::dynamic_pointer_cast<const X>(y)) return x;
    return Error<X>();
  }
  INL const ET * operator->(){ return err.get(); }
  INL operator bool() const { return bool(err); }
  template<typename> friend struct Error;
  friend struct ErrGeneric;

  template<typename T> struct [[nodiscard]] Or {
    template<typename X> INL Or(Error<X> e) : err(Error(e)) {
      static_assert(std::is_base_of_v<ET,X>);
    }
    INL Or(T &&_val) : val(std::move(_val)) {}
    INL Or(const T &_val) : val(_val) {}
    // TODO: this thing requires T to provide a default constructor (so that destructor can work correctly).
    // Instead we could use opt<T>, but that requires dereferencing in at the call site.
    T val;
    Error err;
  };

  template<typename ...Args> INL Error<> wrap(const str &fmt, Args ...args);

private:
  INL Error(std::shared_ptr<const ET> _err) : err(_err) {}
  std::shared_ptr<const ET> err;
};

template<typename ET> struct ErrText : ErrGeneric {
  template<typename ...Args> INL static Error<ET> New(const str &fmt, Args ...args) {
    auto p = new ET();
    p->msg = gprusak::fmt(fmt,args...);
    return ErrGeneric::New(p);
  } 
  str show() const { return cause_ ? gprusak::fmt("% : %",msg,cause_->show()) : msg; }
  std::shared_ptr<const ErrGeneric> cause() const { return cause_; }
private:
  template<typename> friend struct Error;
  str msg;
  std::shared_ptr<const ErrGeneric> cause_;
};

struct Err : ErrText<Err> {};

template<typename ET>
template<typename ...Args> INL Error<> Error<ET>::wrap(const str &fmt, Args ...args) {
  if(!err) return {};
  auto p = new Err;
  p->msg = gprusak::fmt(fmt,args...);
  p->cause_ = err;
  return ErrGeneric::New(p);
}

}  // namespace gprusak

#endif  // GPRUSAK_ERRORS_H_

