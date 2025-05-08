#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_PATH_SIZE 64

// when some shell calls stfu, stfu is opened as a child
// process. then we can use ppid to detect the shell caller
// without relying on unreliable env vars e.g. $SHELL
//
// mutes pidbuf and ppidbuf, pass as &int
//
// returns -1 if fail
int get_pids(pid_t *pidbuf, pid_t *ppidbuf) {
  FILE *f = fopen("/proc/self/stat", "r");
  if (!f)
    return -1;

  char comm[256], state;
  // from proc_pid_stat manpage, the format of /proc/self/stat is:
  // pid (comm) state ppid ...
  // We only need pid, comm, state, ppid
  fscanf(f, "%d %s %c %d", pidbuf, comm, &state, ppidbuf);
  fclose(f);
  return 0;
}

// comm is the filename of the executable
int get_process_comm_from_pid(pid_t pid, char *buffer, size_t size) {
  char path[PROC_PATH_SIZE];
  snprintf(path, sizeof(path), "/proc/%d/comm", pid);

  FILE *f = fopen(path, "r");
  if (!f)
    return -1;

  if (!fgets(buffer, size, f)) {
    fclose(f);
    return -1;
  }
  fclose(f);

  // remove trailing newline
  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  }
  return 0;
}

int get_process_realpath(pid_t pid, char *buffer) {
  char path[PROC_PATH_SIZE];
  snprintf(path, sizeof(path), "/proc/%d/exe", pid);
  if (realpath(path, buffer) == NULL)
    return 1;
  return 0;
}
