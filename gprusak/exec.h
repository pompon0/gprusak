#ifndef GPRUSAK_EXEC_H_
#define GPRUSAK_EXEC_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "gprusak/log.h"
#include "gprusak/types.h"
#include "gprusak/errors.h"

namespace gprusak {

// https://stackoverflow.com/questions/56650579/why-should-i-close-all-file-descriptors-after-calling-fork-and-prior-to-callin  
// https://codereview.stackexchange.com/questions/235529/spawn-a-linux-subprocess-without-leaking-fds
// https://github.com/cobbal/swsh/blob/5c5e378421a8d754161ee6c3ead432c6d6fc214b/Sources/linuxSpawn/linux-spawn.c
static Error<>::Or<pid_t> spawn(const vec<str> &cmd) { FRAME("spawn(%)",gprusak::join(",",cmd));
 	pid_t child = ::fork();
	if(child<0) return Err::New("fork(): error");
  // TODO: consider a pipe to pass error from within child process
	if(child!=0) return child; 

  // Clear the file descriptors, except for stderr.
  // Set stdin and stdout to /dev/null, so that they are harder to misuse.
  int fd = open("/dev/null",O_RDWR);
  if(fd==-1) error("open(): error");
  if(dup2(fd,STDIN_FILENO)==-1) error("dup2(): error");
  if(dup2(fd,STDOUT_FILENO)==-1) error("dup2(): error");
  auto fdlimit = sysconf(_SC_OPEN_MAX);
  for(int i = 0; i < fdlimit; i++) switch(i) {
    case STDIN_FILENO: break;
    case STDOUT_FILENO: break;
    case STDERR_FILENO: break;
    default: close(i);
  }
	if(setsid()==-1) error("setsid(): error");

  // Execute the command.
  vec<vec<char>> args;
  vec<char*> argv;
  for(size_t i=0; i<cmd.size(); i++){
    args.emplace_back(cmd[i].begin(),cmd[i].end());
    args.back().push_back(0);
    argv.push_back(&args.back()[0]);
  }
  argv.push_back(0);
  ::execvp(&argv[0][0],&argv[0]);
	error("execvp(): %",strerror(errno));
}

}  // namespace gprusak

#endif  // GPRUSAK_EXEC_H_
