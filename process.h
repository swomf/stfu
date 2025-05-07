#include <stdlib.h>
int get_pids(pid_t *pidbuf, pid_t *ppidbuf);
int get_process_comm_from_pid(pid_t pid, char *buffer, size_t size);
int get_process_realpath(pid_t pid, char *buffer);
