#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
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
static inline void printf_safe_color(FILE* stream, char* on_tty,
                                     char* on_not_tty) {
  if (isatty(fileno(stream)))
    fprintf(stream, "%s", on_tty);
  else
    fprintf(stream, "%s", on_not_tty);
}

int main(int argc, char** argv) {
#ifdef MAKE_FORK_FAIL_TEST
  seccomp_block_fork();
#endif
  if (argc < 2) {
    fprintf(stderr, "usage: %s <command> [args...]\n", argv[0]);
    return 1;
  }
  if (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0) {
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
            "  -h, --help\tPrint this help\n");
    return 1;
  }

  pid_t pid = fork();
  switch (pid) {
    case -1:
      printf_safe_color(stderr, "[\033[1;31m■\033[0m] ", "[■] ");
      perror("couldn't fork");
      return 1;
    case 0:
      freopen("/dev/null", "w", stdout);
      freopen("/dev/null", "w", stderr);
      execvp(argv[1], &argv[1]);
      break;
    default:
      printf_safe_color(stdout, "[\033[1;32m✓\033[0m] ", "[✓] ");
      printf("%jd\n", (intmax_t)pid);
      break;
  }

  return 0;
}
