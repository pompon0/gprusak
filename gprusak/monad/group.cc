#include <experimental/coroutine>

/*
pool (with capacity nx parallelism

physical parallelism
  bounded by the number of cores
logical parallelism
  bounded by an arbitrary n
  requires threads to wait to be usable
  the underlying physical thread has to stop a waiting logical thread to be usable

  n threads,

  t := thread
    p1 := t.pool(2)
    p2 := t.pool(3)

core parallelism (realtime concurrent execution)
state/stack parallelism (functions partially executed at the same time)
*/

// https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html#fn2
// https://timsong-cpp.github.io/cppwp/n4861/dcl.fct.def.coroutine#5
// aaand the most useless description:
//   https://en.cppreference.com/w/cpp/language/coroutines
// stacklessness:
//   https://stackoverflow.com/questions/57163510/are-stackless-c20-coroutines-a-problem
// golang-style C++ coroutine library:
//   https://github.com/idealvin/co
//
// to get golang-like coroutines:
// - each suspendable function needs to return a ReturnType, which is also Awaitable
// - each suspendable function needs to be called via co_await
// - top level function executed by a thread would have to be able to poll on
//   all events the coroutines might be awaiting for (can be as easy as an epoll)
// - research what io_uring provides before jumping to epoll
//
// struct ReturnType {
//   struct promise_type {
//     ReturnType get_return_object();
//   }
// }
//
// struct State {
//   promise_type p;
//   Args args;
//   LocalVars local_vars;
// }
// coroutine {
//   s = new State;
//   r = s.p.get_return_object();
//   co_await s.p.initial_suspend();
//
//   // suspension point
//   return r;
//
//   // co_await : Awaitable -> Awaiter
//   a = co_await awaitable;
//   if(!a.await_ready()) {
//     if(a.await_suspend(handle{s})) {
//       return r;
//     }
//   }
//   resume_point:
//     x = a.await_resume();
//
//   // co_return expr
//   s.p.return_value(expr)
//   co_await s.p.final_suspend();

struct X {
  struct promise_type {
    X get_return_object(){
      return {std::coroutine_handler<promise_type>::from_promise(*this)};
    }
    std::suspend_never initial_suspend() { return {}; }
    //std::suspend_never final_suspend() { return {}; }
  };
  std::coroutine_handle<promise_type> cont;
};

struct Awaiter {
  // true if should execute synchronously
  bool await_ready() { return false; }
  
  // computes the result of the co_await expression at the start of the continuation
  void await_resume() {}

  // yield continuation h
  void await_suspend(std::coroutine_handle<> h){}
}

X hello() {
  co_await std::suspend_always{};
}

int main() {
  auto x = hello();
  x.cont();
  return 0;
}
