#include <sys/wait.h>

int __neut__unsafe__wifexited(int stat) {
  return WIFEXITED(stat);
}

int __neut__unsafe__wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

int __neut__unsafe__wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

int __neut__unsafe__wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

int __neut__unsafe__wtermsig(int stat) {
  return WTERMSIG(stat);
}

int __neut__unsafe__wcoredump(int stat) {
  return WCOREDUMP(stat);
}

int __neut__unsafe__wstopsig(int stat) {
  return WSTOPSIG(stat);
}
