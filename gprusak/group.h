#ifndef GPRUSAK_GROUP_H_
#define GPRUSAK_GROUP_H_

#include <future>
#include <mutex>

#include "gprusak/ctx.h"

namespace gprusak {

struct Group {
  INL void collect(Error<> x) { FRAME("Group::collect()");
    auto l = lock(mtx);
    // WARNING: A task is not allowed to remove its future!
    if(!x || err) return;
    ctx.cancel();
    err = x;
  }

public:
  template<typename F> INL void spawn(F f) { FRAME("Group::spawn");
    static_assert(has_sig<F,Error<>(Ctx)>);
    auto l = lock(mtx);
    tasks.push_back(std::async(std::launch::async,[this,f]()INLL{ collect(f(ctx)); }));
  }

  template<typename F> static Error<> Run(Ctx ctx, F f) { FRAME("Group::Run");
    static_assert(has_sig<F,Error<>(Ctx,Group&)>);
    Group g(ctx);
    g.collect(f(g.ctx,g));
    while(auto t = g.pop_task()) t->wait();
    return g.err;
  };

private:
  opt<std::shared_future<void>> pop_task() { FRAME("Group::pop_task()");
    auto l = lock(mtx);
    if(!tasks.size()) return nil;
    auto f = std::move(tasks.back());
    tasks.pop_back();
    return f;
  }

  Group(const Ctx &_ctx) : ctx(_ctx.with_cancel()) {}
  Group(const Group&) = delete;

  // TODO: add task id to the context to improve logging.
  CancellableCtx ctx;
  std::mutex mtx;
  vec<std::future<void>> tasks;
  Error<> err;
};

} // namespace gprusak

#endif  // GPRUSAK_GROUP_H_
