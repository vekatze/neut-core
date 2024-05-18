#include <stdio.h>
#include <pthread.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
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

__attribute__((always_inline))
int neut_core_v0_48_O_RDONLY() {
  return O_RDONLY;
}

__attribute__((always_inline))
int neut_core_v0_48_O_WRONLY() {
  return O_WRONLY;
}

__attribute__((always_inline))
int neut_core_v0_48_O_RDWR() {
  return O_RDWR;
}

__attribute__((always_inline))
int neut_core_v0_48_O_CREAT() {
  return O_CREAT;
}

__attribute__((always_inline))
int neut_core_v0_48_O_APPEND() {
  return O_APPEND;
}

__attribute__((always_inline))
int neut_core_v0_48_SEEK_SET() {
  return SEEK_SET;
}

__attribute__((always_inline))
int neut_core_v0_48_SEEK_CUR() {
  return SEEK_CUR;
}

__attribute__((always_inline))
int neut_core_v0_48_SEEK_END() {
  return SEEK_END;
}

__attribute__((always_inline))
int neut_core_v0_48_thread_mutex_size() {
  return sizeof(pthread_mutex_t);
}

__attribute__((always_inline))
int neut_core_v0_48_thread_cond_size() {
  return sizeof(pthread_cond_t);
}

__attribute__((always_inline))
int neut_core_v0_48_errno() {
  return errno;
}
