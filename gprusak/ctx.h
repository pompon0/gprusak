#ifndef GPRUSAK_CTX_H_
#define GPRUSAK_CTX_H_

#include <mutex>
#include <memory>
#include <set>
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "gprusak/log.h"
#include "gprusak/types.h"
#include "gprusak/errors.h"
#include "gprusak/sys/raw.h"

namespace gprusak {

using ErrTimeout = sys::ErrTimeout;
struct ErrCancelled : ErrText<ErrCancelled> {};

namespace ctx_internal {

struct Ctx {
private:
  using Lock = std::lock_guard<std::mutex>; 
  std::mutex mtx;
  std::set<Ctx*> children; // read/write under lock.
  bool cancelled = 0; // read/write under lock. determines whether cancel_fds in the subtree have been cleaned up.
  
  std::shared_ptr<Ctx> parent; // immutable
  opt<sys::Event> cancel_fd; // immutable, only at the ctx which owns it. TODO: perhaps it should be propagated as well?
  opt<absl::Time> deadline; // immutable, propagated to the child ctxs.

  bool add_child(Ctx *child) { FRAME("add_child");
    Lock L(mtx);
    children.insert(child);
    return cancelled;
  }
  void del_child(Ctx *child) { FRAME("del_child");
    Lock L(mtx);
    children.erase(child);
  }
public:
  INL inline absl::Time now() const { return absl::Now(); }
  INL opt<absl::Time> get_deadline() const { return deadline; }
  INL Error<>::Or<sys::Descriptor> poll(vec<sys::Descriptor> fds) const {
    // find cancel_fd
    auto ctx = this;
    for(;ctx; ctx = ctx->parent.get()) if(ctx->cancel_fd) break;
    if(ctx) fds.push_back(ctx->cancel_fd->desc);
    auto [fd,err] = sys::Descriptor::poll(fds,deadline);
    if(err) return err;
    if(ctx && fd==ctx->cancel_fd->desc) return ErrCancelled::New("cancelled");
    return fd;
  }
  INL bool check_done() {
    // We do it this way only to avoid now() if unnecessary.
    { Lock L(mtx); if(cancelled) return true; }
    if(deadline && now()>*deadline) {
      cancel();
      return true;
    }
    return false;
  }

  // New() takes an upward lock, but it is detached, so it doesn't collide with cancel.
  static std::shared_ptr<Ctx> New(std::shared_ptr<Ctx> parent, bool cancellable, opt<absl::Time> deadline) {
    // This lock protects ctx from getting cancelled while still in the constructor.
    auto ctx = std::make_shared<Ctx>();
    Lock L(ctx->mtx); 
    ctx->parent = parent;
    ctx->deadline = deadline;
    if(parent) {
      if(auto d = parent->deadline) if(!deadline || *d<*deadline) ctx->deadline = d;
      ctx->cancelled = parent->add_child(ctx.get()); 
    }
    if(!ctx->cancelled && cancellable) {
      if(auto [e,err] = sys::Event::New(); err) error("Event::New(): %",err); else ctx->cancel_fd = e;
    }
    return ctx;
  }
  ~Ctx() {
    // not taking lock, to not cause deadlock with cancel().
    if(parent) parent->del_child(this);
    // cancel_fd is immutable and polling cancel_fd requires holding a reference to the Ctx,
    // so at this point noone is using it.
    if(cancel_fd) if(auto err = cancel_fd->close()) error("close(): %",err->show());
  } 
  
  // cancel takes downward locks, so it observes a consistent state of the ctx subtree.
  // (composed of snapshots from different times though).
  void cancel() { FRAME("cancel");
    Lock L(mtx);
    if(cancelled) return;
    if(cancel_fd) if(auto err = cancel_fd->inc(1)) error("cancel_fd->inc(): %",err->show());
    for(auto c : children) c->cancel();
    cancelled = true;
  }
};

}  // internal

struct CancellableCtx;

struct Ctx {
  INL Ctx(const Ctx&) = default;
  INL inline absl::Time now() const { return ptr->now(); }
  INL inline Error<> wait_until(absl::Time t) const {
    if(auto d = deadline(); d && *d<=t){ auto [_,err] = poll({}); return err; }
    if(auto [_,err] = with_deadline(t).poll({}); !err.is<ErrTimeout>()) return err;
    return {};
  }
  INL inline static Ctx background(){ return Ctx(ctx_internal::Ctx::New(0,false,nil)); }
  INL inline Ctx with_timeout(absl::Duration d) const { return with_deadline(now()+d); }
  INL inline Ctx with_deadline(absl::Time t) const { return Ctx(ctx_internal::Ctx::New(ptr,false,t)); }
  INL inline CancellableCtx with_cancel() const;
  INL inline bool done() const { return ptr->check_done(); }
  INL inline opt<absl::Time> deadline() const { return ptr->get_deadline(); }
  INL inline Error<>::Or<sys::Descriptor> poll(const vec<sys::Descriptor> &fds) const { return ptr->poll(fds); }

protected:
  INL Ctx(std::shared_ptr<ctx_internal::Ctx> &&_ptr) : ptr(_ptr) {}
  std::shared_ptr<ctx_internal::Ctx> ptr;
};

struct CancellableCtx : Ctx {
  void cancel(){ ptr->cancel(); }
private:
  using Ctx::Ctx;
};

INL CancellableCtx Ctx::with_cancel() const {
  return CancellableCtx(ctx_internal::Ctx::New(ptr,true,nil));
}

}  // gprusak

#endif  // GPRUSAK_CTX_H_
