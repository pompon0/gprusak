#define DEBUG_MODE
#define VERBOSE

#include "gprusak/types.h"
#include "gprusak/log.h"
#include <iostream>
#include <experimental/coroutine>

using namespace gprusak;

template<typename M> struct IsMonad {
  struct Ptr {
    M *m;
    ~Ptr(){ FRAME("~Ptr"); delete m; }
    operator M(){ FRAME("operator M(%)",m); return *m; }
  };
  struct promise_type {
    M *m;
    promise_type() : m(new M{}) { FRAME("promise_type"); }
    Ptr get_return_object(){ FRAME("get_return_object"); return {m}; }
    std::experimental::suspend_never initial_suspend() { FRAME("initial_suspend"); return {}; }
    std::experimental::suspend_never final_suspend(){ FRAME("final_suspend"); return {}; }
    void unhandled_exception() {}
    void return_value(M x){ FRAME("return_value(%)",v); *m = x; }
  };

  struct awaiter {
    Error e;
    bool await_ready(){ return false; }
    template<typename V> void await_suspend(std::experimental::coroutine_handle<V> h) {
      *h.promise().m = {.ok=false,.err=e.err};
      h();
      h.destroy();
    }
    T await_resume(){ return e.v; }
  };

  awaiter operator co_await() { return {*this}; }
};

template<typename A> struct Maybe : IsMonad<Maybe<A>> {
  bool ok;
  A a;
 
  INL static Maybe<A> ret(A a){ return {.ok=true,.a=a}; }
  
  template<typename B, typename F> INL static Maybe<B> apply(Maybe<A> a, F f) {
    return a.ok ? f(a.a) : Maybe<B>{.ok=false};
  }
};


template<typename T> struct Error {
  bool ok;
  T v;
  str err;

  struct Ptr {
    Error *e;
    ~Ptr(){ FRAME("~Ptr"); delete e; }
    operator Error(){ FRAME("operator Error(%)",e); return *e; }
  };

  struct promise_type {
    Error *e;
    promise_type() : e(new Error{}) { FRAME("promise_type"); }
    Ptr get_return_object(){ FRAME("get_return_object"); return {e}; }
    std::experimental::suspend_never initial_suspend() { FRAME("initial_suspend"); return {}; }
    std::experimental::suspend_never final_suspend(){ FRAME("final_suspend"); return {}; }
    void unhandled_exception() {}
    void return_value(T v){ FRAME("return_value(%)",v); *e = {.ok=true,.v=v}; }
  };

  struct awaiter {
    Error e;
    bool await_ready(){ return e.ok; }
    template<typename V> void await_suspend(std::experimental::coroutine_handle<V> h) {
      DEBUG if(e.ok) error("await_suspend called on OK result");
      *h.promise().e = {.ok=false,.err=e.err};
      h.destroy();
    }
    T await_resume(){ return e.v; }
  };

  awaiter operator co_await() { return {*this}; }
};

Error<long> assert_pos(long i) {
  if(i<=0) return Error<long>{.ok=false, .err="non-positive"};
  return Error<long>{.ok=true, .v=i};
}

Error<str> plus1(int i) {
  for(size_t j=0; j<5; j++) i = (co_await assert_pos(i))+1;
  co_return fmt("result is %",i);
}

int main() {
  StreamLogger _(std::cerr);
  auto x = plus1(3);
  info("x = {%,%,%}",x.ok,x.err,x.v);
  return 0;
}
