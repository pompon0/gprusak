#ifndef GPRUSAK_SYS_RAW_H_
#define GPRUSAK_SYS_RAW_H_

#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <cstring>
#include <sys/eventfd.h>
#include <compare>

#include "absl/time/time.h"

#include "gprusak/log.h"
#include "gprusak/errors.h"

namespace gprusak::sys {

struct ErrTimeout : ErrText<ErrTimeout> {};

// TODO: consider making these shared/unique pointers, so that system resources
// are not leaked. It has to cooperate well with fork() and pipe lifecycle.
struct Descriptor {
  int fd = invalid;
  enum { invalid = -1 };
 
  INL auto operator<=>(const Descriptor&) const = default;

  Error<> close() {
    if(fd==invalid) return {};
    if(::close(fd)==-1) return Err::New("%",strerror(errno));
    fd = invalid;
    return {};
  }

  INL static Error<>::Or<Descriptor> poll(const vec<Descriptor> &fds, opt<absl::Time> deadline/*, sigmask*/) {
    vec<::pollfd> x(fds.size());
    ::timespec t;
    if(deadline) t = absl::ToTimespec(*deadline-absl::UnixEpoch());
    for(size_t i=0; i<fds.size(); i++) x[i] = {
      .fd = fds[i].fd,
      .events = POLLIN,
    };
    auto res = ::ppoll(x.data(),x.size(),deadline ? &t : 0,0);
    if(res==0) return ErrTimeout::New("timeout");
    if(res==-1) return Err::New("ppoll(): %",strerror(errno));
    for(size_t i=0; i<fds.size(); i++) {
      if(x[i].revents&POLLIN) return fds[i];
      if(x[i].revents!=0) return Err::New("an error on fd %",x[i].fd);
    }
    return Err::New("this shouldn't happen");
  }
};

struct Pipe {
  Descriptor in,out;
  
  static Error<>::Or<Pipe> New() {
  int f[2];
  if(::pipe(f)==-1) return Err::New("pipe(): %",strerror(errno));
  return Pipe{.in = {f[1]}, .out = {f[0]} };
}

  // blocking until all is written
  Error<> write(const Bytes &data) {
    ssize_t s = ::write(in.fd,data.data(),data.size());
    if(s==-1) return Err::New("write(): %",strerror(errno));
    if(s!=data.size()) return Err::New("wrote %/% bytes",s,data.size());
    return {};
  }

  // blocking until some data available
  Error<>::Or<Bytes> read() {
    Bytes data(PIPE_BUF);
    ssize_t s = ::read(out.fd,data.data(),data.size());
    if(s==-1) return Err::New("read(): %",strerror(errno));
    data.resize(s);
    return data; 
  }

  Error<> close() {
    if(auto err = in.close()) return err;
    if(auto err = out.close()) return err;
    return {};
  }
};

struct Event {
  Descriptor desc;
  // TODO: expose semaphore functionality
  INL static Error<>::Or<Event> New() {
    int e = ::eventfd(0,0);
    if(e==-1) return Err::New("eventfd(): %",strerror(errno));
    return Event{.desc={.fd=e}};
  }

  INL Error<> inc(uint64_t v) {
    ssize_t s = ::write(desc.fd,&v,sizeof v);
    if(s==-1) return Err::New("write(): %",strerror(errno));
    if(s!=sizeof v) return Err::New("wrote %/% bytes",s,sizeof v);
    return {};
  }

  INL Error<> close() { return desc.close(); }
};

}  // gprusak::sys 

#endif  // GPRUSAK_SYS_RAW_H_
