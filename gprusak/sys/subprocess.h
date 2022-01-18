#ifndef GPRUSAK_SYS_H_
#define GPRUSAK_SYS_H_

#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>

#include "gprusak/log.h"
#include "gprusak/ctx.h"
#include "gprusak/sys/raw.h"

namespace gprusak::sys {

static Error<>::Or<str> subprocess(Ctx::Ptr ctx, const vec<str> &cmd, const str &input) {
  auto [parent_child,pc_err] = Pipe::New();
  if(pc_err) return Err::Wrap(err,"Pipe::New()");
  auto [child_parent,cp_err] = Pipe::New();
  if(cp_err) return Err::Wrap(err,"Pipe::New()");
  pid_t pid = fork();
  if(pid==-1) return Err::New("fork(): %",strerror(errno));
  if(pid==0) { // child
    // redirect stdin and stdout (stderr is propagated)
    if(dup2(parent_child.out.fd,0)==-1) error("dup2(): %",strerror(errno));
    if(dup2(child_parent.in.fd,1)==-1) error("dup2(): %",strerror(errno));
    parent_child.close();
    child_parent.close();
    // execute
    vec<vec<char>> args;
    vec<char*> argv;
    for(size_t i=0; i<cmd.size(); i++){
      args.emplace_back(cmd[i].begin(),cmd[i].end());
      args.back().push_back(0);
      argv.push_back(&args.back()[0]);
    }
    argv.push_back(0);
    if(::execv(&argv[0][0],&argv[0])==-1) error("execv(): %",strerror(errno));
    error("execv() returned");
  }
  // close child ends of pipes, so that pipes will be closed,
  // once subprocess is terminated.
  parent_child.out.close();
  child_parent.in.close();
  // write input asynchronously, until pipe is closed.
  auto twrite = std::async(std::launch::async,[parent_child,input]() mutable { 
    parent_child.write(input);
    parent_child.close();
    info("write complete");
  }); 
  // read output asynchronously, until pipe is closed.
  auto tread = std::async(std::launch::async,[child_parent]() mutable {
    str output;
    while(1) {
      auto buffer = child_parent.read();
      if(buffer.empty()) break;
      output += buffer;
    }
    child_parent.close();
    info("read complete, size = %",output.size());
    return output;
  });
  // wait for subprocess asynchronously.
  ctx = ctx.with_cancel();
  auto tdone = std::async(std::launch::async,[ctx,pid]{
    int status;
    if(waitpid(pid,&status,0)==-1) error("waitpid(%): %",pid,strerror(errno));
    info("subprocess terminated");
    if(WIFEXITED(status)) {
      if(auto s = WEXITSTATUS(status); s==EXIT_SUCCESS) {  
        ctx.cancel();
        return;
      } else {
        error("WEXITSTATUS() = %",s);
      }
    } else if(WIFSIGNALED(status)) {
      error("WTERMSIG() = %",WTERMSIG(status));
    } else {
      error("unknown termination reason");
    }
  });
  // wait for the context to finish.
  ctx.poll();
  info("ctx terminated");
  // kill the subprocess if still running.
  if(waitpid(pid,0,WNOHANG)==0) {
    info("killing subprocess");
    if(kill(pid,SIGKILL)==-1) error("kill(%): %",pid,strerror(errno));
  }
  // wait for the tasks
  tdone.get(); // ends since subprocess terminated
  twrite.get(); // ends since the pipe is closed (subprocess terminanted)
  return tread.get(); // ends since the pipe is closed (subprocess terminated)
}

} // namespace gprusak::sys

#endif  // GPRUSAK_SYS_H_
