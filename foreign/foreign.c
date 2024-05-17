#include <sys/wait.h>

__attribute__((always_inline))
int neut_core_v0_48_wifexited(int stat) {
  return WIFEXITED(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wtermsig(int stat) {
  return WTERMSIG(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wcoredump(int stat) {
  return WCOREDUMP(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_wstopsig(int stat) {
  return WSTOPSIG(stat);
}

__attribute__((always_inline))
int neut_core_v0_48_word_size() {
  return sizeof(void*);
}
