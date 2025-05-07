#include "process.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef MAKE_FORK_FAIL_TEST
#include <errno.h>
#include <seccomp.h>

void seccomp_block_fork() {
  scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

  seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fork), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(vfork), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(clone), 0);

  seccomp_load(ctx);
  seccomp_release(ctx);
}
#endif

// if stdout/stderr is redirected to file
// then we do not want escape characters
static inline void fprintf_safe_color(FILE *stream, const char *on_tty,
                                      const char *on_not_tty) {
  if (isatty(fileno(stream)))
    fprintf(stream, "%s", on_tty);
  else
    fprintf(stream, "%s", on_not_tty);
}

static inline void run_in_shell(int argc, char **argv, char *shell,
                                char *shellscript) {
  char **shell_args = malloc((argc + 3) * sizeof(char *));

  if (shell_args == NULL) {
    perror("couldn't malloc");
    exit(1);
  }

  shell_args[0] = shell; // detected shell, abspath not needed
  shell_args[1] = "-i";  // interactive
  /*
   *  .sh/.fish wrapper to simply pass args as argv[] to shellscript without
   *  complicated string sanitization. our alternative is to run something like
   *     sh -c 'something'
   *  where « ' » needs to be escaped using « '\' »
   */
  shell_args[2] = shellscript;

  for (int i = 2; i < argc; i++)
    shell_args[i + 1] = argv[i];

  // must be terminated by null, see execvp manpage
  shell_args[argc + 1] = (char *)NULL;

  execvp(shell_args[0], &shell_args[0]);
}

static inline void usage() {
  fprintf(stderr, "%s",
          "stfu\n"
          "  Call commands from the terminal without\n"
          "  clogging stdout/stderr. Equivalent to BASH:\n\n"
          "     $cmd\n"
          "     ^Z           (ctrl+z)\n"
          "     bg\n"
          "     disown\n\n"
          "Usage:\n"
          "  stfu <command> [args...]\n\n"
          "Options:\n"
          "  -h, --help\tPrint this help\n"
          "  -s, --shell\tRun the cmd using the user's shell, not directly\n");
}

int main(int argc, char **argv) {
  char shell[256] = {0}, stfu_realpath[256] = {0};
  u_int8_t want_run_in_shell = 0;
  char *shellscript = NULL;

#ifdef MAKE_FORK_FAIL_TEST
  seccomp_block_fork();
#endif

  if (argc < 2) {
    fprintf(stderr, "usage: %s <command> [args...]\n", argv[0]);
    return 1;
  }

  if (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0) {
    usage();
    return 0;
  }

  // --shell option copies the user's shell (useful for e.g. aliases)
  if (strcmp("--shell", argv[1]) == 0 || strcmp("-s", argv[1]) == 0) {
    want_run_in_shell = 1;
    pid_t pid = -1, ppid = -1;

    if (get_pids(&pid, &ppid) != 0) {
      fprintf(stderr, "couldn't get parent pid of stfu\n");
      return 1;
    }

    if (get_process_comm_from_pid(ppid, shell, sizeof(shell)) != 0) {
      fprintf(stderr, "couldn't get parent process name\n");
      return 1;
    }

    if (get_process_realpath(pid, stfu_realpath) != 0) {
      fprintf(stderr, "couldn't get parent pid of stfu\n");
      return 1;
    }

    if (strcmp(shell, "fish") == 0) {
      // fish uses $argv instead of $@, see stfu.*sh
      shellscript = "stfu.fish";
    } else {
      // assume typical posix shell e.g. sh zsh bash
      shellscript = "stfu.sh";
    }
  }

  pid_t pid = fork();
  switch (pid) {
  case -1:
    fprintf_safe_color(stderr, "[\033[1;31m■\033[0m] ", "[■] ");
    perror("couldn't fork");
    return 1;
  case 0:
#ifndef DEBUG
    // equivalent to `&> /dev/null`
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
#endif
    if (want_run_in_shell) {
      // mutes shellname
      run_in_shell(argc, argv, shell, shellscript);
    } else {
      execvp(argv[1], &argv[1]);
    }
    perror("execvp failed");
  default:
    fprintf_safe_color(stdout, "[\033[1;32m✓\033[0m] ", "[✓] ");
    if (want_run_in_shell)
      printf("%s, ", shell);
    printf("%jd\n", (intmax_t)pid);
    break;
  }

  return 0;
}
