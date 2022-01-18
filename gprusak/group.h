#ifndef GPRUSAK_GROUP_H_
#define GPRUSAK_GROUP_H_

#include <future>
#include <mutex>

#include "gprusak/ctx.h"

namespace gprusak {

struct Group {
private:
  using Lock = std::lock_guard<std::mutex>; 

public:
  template<typename F> INL void spawn(F f) {
    static_assert(has_sig<F,Error<>(Ctx)>);
    Lock l(mtx);
    auto id = task_count++;
    tasks[id] = std::async(std::launch::async,[this,id,f]()INLL{
      if(auto f_err = f(Ctx(ctx))) {
        Lock l(mtx);
        tasks.erase(id);
        if(!err) {
          ctx.cancel();
          err = f_err;
        }
      }
    }).share();
  }

  template<typename F> static Error<> Run(Ctx ctx, F f) {
    static_assert(has_sig<F,Error<>(Ctx,Group&)>);
    Group g(ctx);
    g.spawn([&g,f](Ctx ctx)INLL{ return f(ctx,g); });
    while(auto t = g.pop_task()) t->wait();
    return g.err;
  };

private:
  opt<std::shared_future<void>> pop_task() {
    Lock l(mtx);
    if(!tasks.size()) return nil;
    auto it = tasks.begin();
    auto res = it->second;
    tasks.erase(it);
    return res;
  }

  Group(const Ctx &_ctx) : ctx(_ctx.with_cancel()) {}
  Group(const Group&) = delete;

  CancellableCtx ctx;
  std::mutex mtx;
  std::map<size_t,std::shared_future<void>> tasks;
  size_t task_count = 0;
  Error<> err;
};

} // namespace gprusak

#endif  // GPRUSAK_GROUP_H_
