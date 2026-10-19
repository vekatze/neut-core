#ifdef __linux__
#define _GNU_SOURCE
#endif

#include <dirent.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#ifndef __wasi__
#include <pthread.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;
#endif

typedef int64_t neut_int;
typedef uint64_t neut_word;
typedef double neut_float;

#include "foreign/ryu/d2s.c"
#define to_chars f2s_to_chars
#include "foreign/ryu/f2s.c"
#undef to_chars

#ifndef __wasi__
__attribute__((always_inline)) int neut_core_v0_57_wifexited(int stat) {
  return WIFEXITED(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wtermsig(int stat) {
  return WTERMSIG(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wcoredump(int stat) {
  return WCOREDUMP(stat);
}

__attribute__((always_inline)) int neut_core_v0_57_wstopsig(int stat) {
  return WSTOPSIG(stat);
}

static int neut_core_v0_57_open_cloexec_pipe(int fds[2]) {
#ifdef __linux__
  return pipe2(fds, O_CLOEXEC);
#else
  if (pipe(fds) != 0) {
    return -1;
  }
  if (fcntl(fds[0], F_SETFD, FD_CLOEXEC) != 0 ||
      fcntl(fds[1], F_SETFD, FD_CLOEXEC) != 0) {
    int saved_errno = errno;
    close(fds[0]);
    close(fds[1]);
    errno = saved_errno;
    return -1;
  }
  return 0;
#endif
}

static int neut_core_v0_57_add_spawn_chdir(posix_spawn_file_actions_t *actions,
                                           const char *cwd) {
#if defined(__APPLE__) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 260000
  return posix_spawn_file_actions_addchdir(actions, cwd);
#else
  return posix_spawn_file_actions_addchdir_np(actions, cwd);
#endif
}

enum {
  NEUT_CORE_V0_57_STREAM_PIPED = 0,
  NEUT_CORE_V0_57_STREAM_INHERITED = 1,
  NEUT_CORE_V0_57_STREAM_DISCARDED = 2,
};

static int neut_core_v0_57_add_stream(posix_spawn_file_actions_t *actions,
                                      int mode, int child_end, int stream,
                                      int discarding_flags) {
  switch (mode) {
  case NEUT_CORE_V0_57_STREAM_PIPED:
    return posix_spawn_file_actions_adddup2(actions, child_end, stream);
  case NEUT_CORE_V0_57_STREAM_INHERITED:
#if defined(__APPLE__)
    return posix_spawn_file_actions_addinherit_np(actions, stream);
#else
    return 0;
#endif
  default:
    return posix_spawn_file_actions_addopen(actions, stream, "/dev/null",
                                            discarding_flags, 0);
  }
}

static int neut_core_v0_57_prepare_spawn(posix_spawn_file_actions_t *actions,
                                         posix_spawnattr_t *attributes,
                                         const char *cwd, const int modes[3],
                                         const int child_ends[3],
                                         int new_group) {
  int result = neut_core_v0_57_add_stream(actions, modes[0], child_ends[0],
                                          STDIN_FILENO, O_RDONLY);
  if (result == 0) {
    result = neut_core_v0_57_add_stream(actions, modes[1], child_ends[1],
                                        STDOUT_FILENO, O_WRONLY);
  }
  if (result == 0) {
    result = neut_core_v0_57_add_stream(actions, modes[2], child_ends[2],
                                        STDERR_FILENO, O_WRONLY);
  }
  short flags = 0;
#if defined(__APPLE__)
  flags |= POSIX_SPAWN_CLOEXEC_DEFAULT;
#elif defined(__GLIBC__) &&                                                    \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 34))
  if (result == 0) {
    result =
        posix_spawn_file_actions_addclosefrom_np(actions, STDERR_FILENO + 1);
  }
#endif
  if (result == 0 && new_group) {
    flags |= POSIX_SPAWN_SETPGROUP;
    result = posix_spawnattr_setpgroup(attributes, 0);
  }
  if (result == 0 && flags != 0) {
    result = posix_spawnattr_setflags(attributes, flags);
  }
  if (result == 0 && cwd != NULL) {
    result = neut_core_v0_57_add_spawn_chdir(actions, cwd);
  }
  return result;
}

static int neut_core_v0_57_spawn_with(const char *file, char *const argv[],
                                      const char *cwd, pid_t *pid,
                                      const int modes[3],
                                      const int child_ends[3],
                                      int new_group) {
  posix_spawn_file_actions_t actions;
  posix_spawnattr_t attributes;
  int result = posix_spawn_file_actions_init(&actions);
  if (result != 0) {
    return result;
  }
  result = posix_spawnattr_init(&attributes);
  if (result != 0) {
    posix_spawn_file_actions_destroy(&actions);
    return result;
  }
  result = neut_core_v0_57_prepare_spawn(&actions, &attributes, cwd, modes,
                                         child_ends, new_group);
  if (result == 0) {
    result = posix_spawnp(pid, file, &actions, &attributes, argv, environ);
  }
  posix_spawnattr_destroy(&attributes);
  posix_spawn_file_actions_destroy(&actions);
  return result;
}

static void neut_core_v0_57_close_if_open(int fd) {
  if (fd >= 0) {
    close(fd);
  }
}

int neut_core_v0_57_spawn(const char *file, char *const argv[],
                          const char *cwd, int input_mode, int output_mode,
                          int error_output_mode, int new_group, pid_t *pid,
                          int parent_ends[3]) {
  const int modes[3] = {input_mode, output_mode, error_output_mode};
  int pipes[3][2] = {{-1, -1}, {-1, -1}, {-1, -1}};
  int child_ends[3] = {-1, -1, -1};
  int result = 0;
  for (int i = 0; i < 3 && result == 0; i++) {
    if (modes[i] == NEUT_CORE_V0_57_STREAM_PIPED) {
      if (neut_core_v0_57_open_cloexec_pipe(pipes[i]) != 0) {
        result = errno;
      } else {
        int child_side = i == 0 ? 0 : 1;
        child_ends[i] = pipes[i][child_side];
        parent_ends[i] = pipes[i][1 - child_side];
      }
    }
  }
  if (result == 0) {
    result = neut_core_v0_57_spawn_with(file, argv, cwd, pid, modes,
                                        child_ends, new_group);
  }
  for (int i = 0; i < 3; i++) {
    neut_core_v0_57_close_if_open(child_ends[i]);
    if (result != 0 && child_ends[i] >= 0) {
      neut_core_v0_57_close_if_open(parent_ends[i]);
    }
    if (child_ends[i] < 0) {
      parent_ends[i] = -1;
    }
  }
  if (result != 0) {
    errno = result;
    return -1;
  }
  return 0;
}

int neut_core_v0_57_send_signal(pid_t target, int signal_kind, int to_group) {
  static const int signals[] = {SIGHUP, SIGINT, SIGKILL, SIGTERM};
  if (to_group) {
    return killpg(target, signals[signal_kind]);
  }
  return kill(target, signals[signal_kind]);
}

static int neut_core_v0_57_write_all(int fd, const char *buffer,
                                     size_t length) {
  size_t written = 0;
  while (written < length) {
    ssize_t bytes = write(fd, buffer + written, length - written);
    if (bytes >= 0) {
      written += (size_t)bytes;
    } else if (errno != EINTR) {
      return -1;
    }
  }
  return 0;
}

#ifdef F_SETNOSIGPIPE
int neut_core_v0_57_write_until_closed(int fd, const char *buffer,
                                       size_t length) {
  if (fcntl(fd, F_SETNOSIGPIPE, 1) != 0) {
    return -1;
  }
  if (neut_core_v0_57_write_all(fd, buffer, length) != 0 && errno != EPIPE) {
    return -1;
  }
  return 0;
}
#else
int neut_core_v0_57_write_until_closed(int fd, const char *buffer,
                                       size_t length) {
  sigset_t sigpipe;
  sigset_t pending;
  sigset_t previous;
  sigemptyset(&sigpipe);
  sigaddset(&sigpipe, SIGPIPE);
  sigpending(&pending);
  bool was_pending = sigismember(&pending, SIGPIPE);
  pthread_sigmask(SIG_BLOCK, &sigpipe, &previous);
  int result = neut_core_v0_57_write_all(fd, buffer, length);
  int saved_errno = errno;
  if (result != 0 && saved_errno == EPIPE) {
    result = 0;
    sigpending(&pending);
    if (!was_pending && sigismember(&pending, SIGPIPE)) {
      int received;
      sigwait(&sigpipe, &received);
    }
  }
  pthread_sigmask(SIG_SETMASK, &previous, NULL);
  errno = saved_errno;
  return result;
}
#endif
#endif

__attribute__((always_inline)) int neut_core_v0_57_O_RDONLY() {
  return O_RDONLY;
}

__attribute__((always_inline)) int neut_core_v0_57_O_WRONLY() {
  return O_WRONLY;
}

__attribute__((always_inline)) int neut_core_v0_57_O_RDWR() { return O_RDWR; }

__attribute__((always_inline)) int neut_core_v0_57_O_CREAT() { return O_CREAT; }

__attribute__((always_inline)) int neut_core_v0_57_O_APPEND() {
  return O_APPEND;
}

__attribute__((always_inline)) int neut_core_v0_57_O_TRUNC() { return O_TRUNC; }

__attribute__((always_inline)) int neut_core_v0_57_SEEK_SET() {
  return SEEK_SET;
}

__attribute__((always_inline)) int neut_core_v0_57_SEEK_CUR() {
  return SEEK_CUR;
}

__attribute__((always_inline)) int neut_core_v0_57_SEEK_END() {
  return SEEK_END;
}

#ifndef __wasi__
__attribute__((always_inline)) size_t neut_core_v0_57_thread_size() {
  return sizeof(pthread_t);
}

__attribute__((always_inline)) size_t neut_core_v0_57_thread_mutex_size() {
  return sizeof(pthread_mutex_t);
}

__attribute__((always_inline)) size_t neut_core_v0_57_thread_cond_size() {
  return sizeof(pthread_cond_t);
}
#endif

__attribute__((always_inline)) neut_int
neut_core_v0_57_atomic_load_int(neut_int *ptr) {
  return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_atomic_fetch_add_int(neut_int *ptr, neut_int value) {
  return __atomic_fetch_add(ptr, value, __ATOMIC_ACQ_REL);
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_atomic_fetch_sub_int(neut_int *ptr, neut_int value) {
  return __atomic_fetch_sub(ptr, value, __ATOMIC_ACQ_REL);
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_atomic_increment_if_positive_int(neut_int *ptr) {
  neut_int current = __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
  while (current > 0) {
    if (__atomic_compare_exchange_n(ptr, &current, current + 1, 1,
                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
      return current;
    }
  }
  return 0;
}

__attribute__((always_inline)) int neut_core_v0_57_errno() { return errno; }

__attribute__((always_inline)) neut_int neut_core_v0_57_ctlz_word(neut_word w) {
  return (neut_int)__builtin_clzll((unsigned long long)w);
}

int64_t neut_core_v0_57_lseek(int fd, int64_t offset, int origin) {
  return (int64_t)lseek(fd, (off_t)offset, origin);
}

__attribute__((always_inline)) uint32_t neut_core_v0_57_UINT32_MAX() {
  return UINT32_MAX;
}

static neut_int neut_core_v0_57_skip_underscores(const char *str,
                                                neut_int length, neut_int i) {
  while (i < length && str[i] == '_') {
    i++;
  }
  return i;
}

static void neut_core_v0_57_read_int_sign(const char *str, neut_int length,
                                          neut_int *i, neut_int *out_sign) {
  *i = neut_core_v0_57_skip_underscores(str, length, *i);
  *out_sign = 1;
  if (*i < length) {
    if (str[*i] == '-') {
      *out_sign = -1;
      (*i)++;
    }
  }
}

static int neut_core_v0_57_read_prefix(const char *str, neut_int length,
                                       neut_int *i, const char *prefix,
                                       neut_int prefix_length) {
  neut_int j = *i;
  for (neut_int k = 0; k < prefix_length; k++) {
    j = neut_core_v0_57_skip_underscores(str, length, j);
    if (j >= length || str[j] != prefix[k]) {
      return 0;
    }
    j++;
  }
  *i = j;
  return 1;
}

static int neut_core_v0_57_decimal_digit(unsigned char ch, int *out_digit) {
  if ('0' <= ch && ch <= '9') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_57_binary_digit(unsigned char ch, int *out_digit) {
  if (ch == '0' || ch == '1') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_57_octal_digit(unsigned char ch, int *out_digit) {
  if ('0' <= ch && ch <= '7') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_57_hex_digit(unsigned char ch, int *out_digit) {
  if ('0' <= ch && ch <= '9') {
    *out_digit = ch - '0';
    return 1;
  }
  if ('A' <= ch && ch <= 'F') {
    *out_digit = ch - 'A' + 10;
    return 1;
  }
  return 0;
}

static int neut_core_v0_57_read_required_digits(
    const char *str, neut_int length, neut_int *i,
    int (*read_digit)(unsigned char, int *), int base, neut_word *out_value) {
  neut_word result = 0;
  int has_digits = 0;
  while (1) {
    int digit = 0;
    *i = neut_core_v0_57_skip_underscores(str, length, *i);
    if (*i >= length || !read_digit((unsigned char)str[*i], &digit)) {
      break;
    }
    if (result > (UINT64_MAX - (neut_word)digit) / (neut_word)base) {
      return 0;
    }
    result = result * (neut_word)base + (neut_word)digit;
    (*i)++;
    has_digits = 1;
  }
  if (!has_digits) {
    return 0;
  }
  *out_value = result;
  return 1;
}

static neut_int neut_core_v0_57_parse_integer_literal(
    const char *str, neut_int length, const char *prefix, neut_int prefix_length,
    int (*read_digit)(unsigned char, int *), int base, neut_int *out_value) {
  neut_int i = 0;
  neut_int sign = 1;
  neut_word result = 0;
  neut_core_v0_57_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_57_read_prefix(str, length, &i, prefix, prefix_length)) {
    return -1;
  }
  if (!neut_core_v0_57_read_required_digits(str, length, &i, read_digit, base,
                                            &result)) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  if (sign < 0) {
    if (result > ((neut_word)1 << 63)) {
      return -1;
    }
    *out_value = (neut_int)(0 - result);
  } else {
    *out_value = (neut_int)result;
  }
  return 1;
}

static neut_int neut_core_v0_57_parse_integer_digits(
    const char *str, neut_int length, int (*read_digit)(unsigned char, int *),
    int base, neut_int *out_value) {
  neut_int i = 0;
  neut_word result = 0;
  if (!neut_core_v0_57_read_required_digits(str, length, &i, read_digit, base,
                                            &result)) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  *out_value = (neut_int)result;
  return 1;
}

static int neut_core_v0_57_parse_normalized_double(const char *str,
                                                   neut_int length,
                                                   double *out_value) {
  if (length < 0) {
    return 0;
  }
  char *normalized = malloc((size_t)length + 1);
  if (normalized == NULL) {
    return 0;
  }
  size_t normalized_length = 0;
  for (neut_int i = 0; i < length; i++) {
    if (str[i] != '_') {
      normalized[normalized_length] = str[i];
      normalized_length++;
    }
  }
  normalized[normalized_length] = '\0';
  char *end = NULL;
  double value = strtod(normalized, &end);
  int ok = end == normalized + normalized_length && isfinite(value);
  free(normalized);
  if (!ok) {
    return 0;
  }
  *out_value = value;
  return 1;
}

neut_int neut_core_v0_57_sleep(neut_float duration_arg) {
  double duration = (double)duration_arg;
  struct timespec request;
  struct timespec remaining;
  request.tv_sec = (time_t)duration;
  request.tv_nsec = (long)((duration - request.tv_sec) * 1e9);

  while (nanosleep(&request, &remaining) < 0) {
    if (errno != EINTR) {
      return -1;
    }
    request = remaining;
  }

  return 0;
}

neut_int neut_core_v0_57_parse_binary(const char *str, neut_int length,
                                     neut_int *out_value) {
  return neut_core_v0_57_parse_integer_literal(
      str, length, "0b", 2, neut_core_v0_57_binary_digit, 2, out_value);
}

neut_int neut_core_v0_57_parse_binary_digits(const char *str, neut_int length,
                                            neut_int *out_value) {
  return neut_core_v0_57_parse_integer_digits(
      str, length, neut_core_v0_57_binary_digit, 2, out_value);
}

neut_int neut_core_v0_57_parse_octal(const char *str, neut_int length,
                                    neut_int *out_value) {
  return neut_core_v0_57_parse_integer_literal(
      str, length, "0o", 2, neut_core_v0_57_octal_digit, 8, out_value);
}

neut_int neut_core_v0_57_parse_octal_digits(const char *str, neut_int length,
                                           neut_int *out_value) {
  return neut_core_v0_57_parse_integer_digits(
      str, length, neut_core_v0_57_octal_digit, 8, out_value);
}

neut_int neut_core_v0_57_parse_decimal(const char *str, neut_int length,
                                      neut_int *out_value) {
  return neut_core_v0_57_parse_integer_literal(
      str, length, "", 0, neut_core_v0_57_decimal_digit, 10, out_value);
}

neut_int neut_core_v0_57_parse_decimal_digits(const char *str, neut_int length,
                                             neut_int *out_value) {
  return neut_core_v0_57_parse_integer_digits(
      str, length, neut_core_v0_57_decimal_digit, 10, out_value);
}

neut_int neut_core_v0_57_parse_hex(const char *str, neut_int length,
                                  neut_int *out_value) {
  return neut_core_v0_57_parse_integer_literal(
      str, length, "0x", 2, neut_core_v0_57_hex_digit, 16, out_value);
}

neut_int neut_core_v0_57_parse_hex_digits(const char *str, neut_int length,
                                         neut_int *out_value) {
  return neut_core_v0_57_parse_integer_digits(
      str, length, neut_core_v0_57_hex_digit, 16, out_value);
}

static int neut_core_v0_57_read_float_exponent(const char *str, neut_int length,
                                               neut_int *i, char marker) {
  int has_digits = 0;
  *i = neut_core_v0_57_skip_underscores(str, length, *i);
  if (*i >= length || str[*i] != marker) {
    return 1;
  }
  (*i)++;
  *i = neut_core_v0_57_skip_underscores(str, length, *i);
  if (*i < length) {
    if (str[*i] == '-') {
      (*i)++;
    }
  }
  while (1) {
    int digit = 0;
    *i = neut_core_v0_57_skip_underscores(str, length, *i);
    if (*i >= length ||
        !neut_core_v0_57_decimal_digit((unsigned char)str[*i], &digit)) {
      break;
    }
    (*i)++;
    has_digits = 1;
  }
  if (!has_digits) {
    return 0;
  }
  return 1;
}

static neut_int neut_core_v0_57_parse_decimal_double(const char *str,
                                                    neut_int length,
                                                    double *out_value) {
  neut_int i = 0;
  neut_int sign = 1;
  int has_fraction = 0;
  int has_exponent = 0;
  neut_word ignored = 0;
  neut_core_v0_57_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_57_read_required_digits(
          str, length, &i, neut_core_v0_57_decimal_digit, 10, &ignored)) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i < length && str[i] == '.') {
    has_fraction = 1;
    i++;
    if (!neut_core_v0_57_read_required_digits(
            str, length, &i, neut_core_v0_57_decimal_digit, 10, &ignored)) {
      return -1;
    }
    i = neut_core_v0_57_skip_underscores(str, length, i);
  }
  has_exponent = i < length && str[i] == 'e';
  if (has_exponent) {
    if (!neut_core_v0_57_read_float_exponent(str, length, &i, 'e')) {
      return -1;
    }
  }
  if (!has_fraction && !has_exponent) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  double value = 0.0;
  if (!neut_core_v0_57_parse_normalized_double(str, length, &value)) {
    return -1;
  }
  *out_value = value;
  return 1;
}

static neut_int neut_core_v0_57_parse_hex_double(const char *str, neut_int length,
                                                double *out_value) {
  neut_int i = 0;
  neut_int sign = 1;
  int has_fraction = 0;
  int has_exponent = 0;
  neut_word ignored = 0;
  neut_core_v0_57_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_57_read_prefix(str, length, &i, "0x", 2)) {
    return -1;
  }
  if (!neut_core_v0_57_read_required_digits(
          str, length, &i, neut_core_v0_57_hex_digit, 16, &ignored)) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i < length && str[i] == '.') {
    has_fraction = 1;
    i++;
    if (!neut_core_v0_57_read_required_digits(
            str, length, &i, neut_core_v0_57_hex_digit, 16, &ignored)) {
      return -1;
    }
    i = neut_core_v0_57_skip_underscores(str, length, i);
  }
  has_exponent = i < length && str[i] == 'p';
  if (has_exponent) {
    if (!neut_core_v0_57_read_float_exponent(str, length, &i, 'p')) {
      return -1;
    }
  }
  if (!has_fraction && !has_exponent) {
    return -1;
  }
  i = neut_core_v0_57_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  double value = 0.0;
  if (!neut_core_v0_57_parse_normalized_double(str, length, &value)) {
    return -1;
  }
  *out_value = value;
  return 1;
}

static int neut_core_v0_57_match(const char *str, neut_int length, neut_int start,
                                 const char *word, neut_int word_len) {
  return start + word_len == length &&
         memcmp(str + start, word, (size_t)word_len) == 0;
}

static neut_int neut_core_v0_57_parse_special_double(const char *str,
                                                    neut_int length,
                                                    double *out_value) {
  neut_int i = 0;
  int negative = 0;
  if (i < length && str[i] == '-') {
    negative = 1;
    i++;
  }
  if (neut_core_v0_57_match(str, length, i, "inf", 3)) {
    *out_value = negative ? -INFINITY : INFINITY;
    return 1;
  }
  if (!negative && neut_core_v0_57_match(str, length, i, "nan", 3)) {
    *out_value = NAN;
    return 1;
  }
  return -1;
}

neut_int neut_core_v0_57_parse_double(const char *str, neut_int length,
                                     double *out_value) {
  if (neut_core_v0_57_parse_special_double(str, length, out_value) > 0) {
    return 1;
  }
  if (neut_core_v0_57_parse_hex_double(str, length, out_value) > 0) {
    return 1;
  }
  return neut_core_v0_57_parse_decimal_double(str, length, out_value);
}

neut_int neut_core_v0_57_write_loop(int fd, const char *buf, size_t len) {
  size_t off = 0;
  while (off < len) {
    ssize_t w = write(fd, buf + off, len - off);
    if (w < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }
    off += (size_t)w;
  }
  return (neut_int)len;
}

neut_int neut_core_v0_57_write_line(int fd, neut_int len, const char *s) {
  if (neut_core_v0_57_write_loop(fd, s, (size_t)len) < 0) {
    return -1;
  }
  if (neut_core_v0_57_write_loop(fd, "\n", 1) < 0) {
    return -1;
  }
  return (neut_int)(len + 1);
}

neut_int neut_core_v0_57_write_int64_core(char *buf_end, neut_int v,
                                          neut_int add_nl) {
  char *p = buf_end;
  if (add_nl) {
    *--p = '\n';
  }

  neut_word u = (v < 0) ? (neut_word)(-(__int128)v) : (neut_word)v;

  if (u == 0) {
    *--p = '0';
  }

  while (u) {
    neut_word q = (neut_word)(((__int128)u * 0xCCCCCCCCCCCCCCCDu) >> 67);
    uint32_t digit = (uint32_t)(u - q * 10);
    *--p = (char)('0' + digit);
    u = q;
  }
  if (v < 0) {
    *--p = '-';
  }

  return (int)(buf_end - p);
}

neut_int neut_core_v0_57_write_int64(int fd, neut_int v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_57_write_int64_core(end, v, 0);
  return neut_core_v0_57_write_loop(fd, end - len, (size_t)len);
}

neut_int neut_core_v0_57_write_int64_nl(int fd, neut_int v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_57_write_int64_core(end, v, 1);
  return neut_core_v0_57_write_loop(fd, end - len, (size_t)len);
}

neut_int neut_core_v0_57_build_float32_buf(char *buf, neut_int cap, float value,
                                           neut_int add_nl);

neut_int neut_core_v0_57_build_float64_buf(char *buf, neut_int cap, double value,
                                           neut_int add_nl) {
  if (cap < 32) {
    errno = EOVERFLOW;
    return 0;
  }

  int n = d2s_buffered_n(value, buf);
  if (n < 0 || (neut_int)n >= cap) {
    errno = EOVERFLOW;
    return 0;
  }

  if (add_nl) {
    if ((neut_int)n + 1 >= cap) {
      errno = EOVERFLOW;
      return 0;
    }
    buf[n++] = '\n';
    buf[n] = '\0';
  }
  return (neut_int)n;
}

neut_int neut_core_v0_57_build_float_buf(char *buf, neut_int cap,
                                         neut_float value, neut_int add_nl) {
  return (neut_int)neut_core_v0_57_build_float64_buf(buf, (size_t)cap, value,
                                                     add_nl);
}

neut_int neut_core_v0_57_build_float32_buf(char *buf, neut_int cap, float value,
                                           neut_int add_nl) {
  if (cap < 32) {
    errno = EOVERFLOW;
    return 0;
  }

  int n = f2s_buffered_n(value, buf);
  if (n < 0 || (neut_int)n >= cap) {
    errno = EOVERFLOW;
    return 0;
  }

  if (add_nl) {
    if ((neut_int)n + 1 >= cap) {
      errno = EOVERFLOW;
      return 0;
    }
    buf[n++] = '\n';
    buf[n] = '\0';
  }
  return (neut_int)n;
}

neut_int neut_core_v0_57_decompose_float32(float value, neut_int *out_sign,
                                          neut_int *out_mantissa,
                                          neut_int *out_exponent);

neut_int neut_core_v0_57_decompose_double(neut_float value, neut_int *out_sign,
                                         neut_int *out_mantissa,
                                         neut_int *out_exponent) {
  const uint64_t bits = double_to_bits(value);
  const bool ieeeSign =
      ((bits >> (DOUBLE_MANTISSA_BITS + DOUBLE_EXPONENT_BITS)) & 1) != 0;
  const uint64_t ieeeMantissa =
      bits & ((1ull << DOUBLE_MANTISSA_BITS) - 1);
  const uint32_t ieeeExponent =
      (uint32_t)((bits >> DOUBLE_MANTISSA_BITS) &
                 ((1u << DOUBLE_EXPONENT_BITS) - 1));

  *out_sign = ieeeSign ? 1 : 0;
  if (ieeeExponent == ((1u << DOUBLE_EXPONENT_BITS) - 1u)) {
    return ieeeMantissa == 0 ? 1 : 2;
  }
  if (ieeeExponent == 0 && ieeeMantissa == 0) {
    *out_mantissa = 0;
    *out_exponent = 0;
    return 0;
  }

  floating_decimal_64 v;
  if (d2d_small_int(ieeeMantissa, ieeeExponent, &v)) {
    for (;;) {
      const uint64_t q = div10(v.mantissa);
      const uint32_t r = ((uint32_t)v.mantissa) - 10 * ((uint32_t)q);
      if (r != 0) {
        break;
      }
      v.mantissa = q;
      ++v.exponent;
    }
  } else {
    v = d2d(ieeeMantissa, ieeeExponent);
  }
  *out_mantissa = (neut_int)v.mantissa;
  *out_exponent = (neut_int)v.exponent;
  return 0;
}

neut_int neut_core_v0_57_decompose_float32(float value, neut_int *out_sign,
                                          neut_int *out_mantissa,
                                          neut_int *out_exponent) {
  const uint32_t bits = float_to_bits(value);
  const bool ieeeSign =
      ((bits >> (FLOAT_MANTISSA_BITS + FLOAT_EXPONENT_BITS)) & 1) != 0;
  const uint32_t ieeeMantissa = bits & ((1u << FLOAT_MANTISSA_BITS) - 1);
  const uint32_t ieeeExponent =
      (bits >> FLOAT_MANTISSA_BITS) & ((1u << FLOAT_EXPONENT_BITS) - 1);

  *out_sign = ieeeSign ? 1 : 0;
  if (ieeeExponent == ((1u << FLOAT_EXPONENT_BITS) - 1u)) {
    return ieeeMantissa == 0 ? 1 : 2;
  }
  if (ieeeExponent == 0 && ieeeMantissa == 0) {
    *out_mantissa = 0;
    *out_exponent = 0;
    return 0;
  }

  const floating_decimal_32 v = f2d(ieeeMantissa, ieeeExponent);
  *out_mantissa = (neut_int)v.mantissa;
  *out_exponent = (neut_int)v.exponent;
  return 0;
}

neut_int neut_core_v0_57_write_double(int fd, neut_float value) {
  char buf[32];
  neut_int len = neut_core_v0_57_build_float_buf(buf, (neut_int)sizeof buf, value, 0);
  if (!len) {
    return -1;
  }

  return neut_core_v0_57_write_loop(fd, buf, len);
}

neut_int neut_core_v0_57_write_double_line(int fd, neut_float value) {
  char buf[32];
  neut_int len = neut_core_v0_57_build_float_buf(buf, (neut_int)sizeof buf, value, 1);
  if (!len) {
    return -1;
  }

  return neut_core_v0_57_write_loop(fd, buf, len);
}

neut_int neut_core_v0_57_int64_strlen(neut_int v) {
  neut_word u = (v < 0) ? (neut_word)(-v) : (neut_word)v;
  int len = 0;
  if (u == 0) {
    len = 1;
  } else {
    while (u) {
      ++len;
      u /= 10;
    }
  }
  if (v < 0) {
    ++len;
  }
  return len;
}

neut_int neut_core_v0_57_double_strlen(neut_float value) {
  char buf[32];
  int n = d2s_buffered_n(value, buf);
  if (n < 0) {
    errno = EOVERFLOW;
    return 0;
  }
  return (neut_int)n;
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_get_online_cpu_count(void) {
  neut_int n = (neut_int)sysconf(_SC_NPROCESSORS_ONLN);
  return (n > 0) ? n : 1;
}

// cf. e_machine in elf.h
typedef enum {
  ARCH_X86_64 = 62, // EM_X86_64
  ARCH_AARCH64 = 183, // EM_AARCH64
  ARCH_UNSUPPORTED = 0xFFFF,
} arch_tag_t;

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_TAG ARCH_X86_64
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_TAG ARCH_AARCH64
#else
#define ARCH_TAG ARCH_UNSUPPORTED
#endif

__attribute__((always_inline)) arch_tag_t neut_core_v0_57_get_arch_tag() {
  return ARCH_TAG;
}

__attribute__((always_inline))
clockid_t neut_core_v0_57_CLOCK_REALTIME() {
  return CLOCK_REALTIME;
}

__attribute__((always_inline)) size_t neut_core_v0_57_sizeof_timespec() {
  return sizeof(struct timespec);
}

__attribute__((always_inline)) int64_t
neut_core_v0_57_timespec_sec(struct timespec *ts) {
  return (int64_t)ts->tv_sec;
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_timespec_nsec(struct timespec *ts) {
  return (neut_int)ts->tv_nsec;
}

static int64_t neut_core_v0_57_days_from_civil(int64_t y, int64_t m,
                                               int64_t d) {
  y -= m <= 2;
  int64_t era = (y >= 0 ? y : y - 399) / 400;
  int64_t yoe = y - era * 400;
  int64_t doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
  int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}

static void neut_core_v0_57_civil_from_days(int64_t z, int64_t *y,
                                                int64_t *m, int64_t *d) {
  z += 719468;
  int64_t era = (z >= 0 ? z : z - 146096) / 146097;
  int64_t doe = z - era * 146097;
  int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int64_t y0 = yoe + era * 400;
  int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  int64_t mp = (5 * doy + 2) / 153;
  *d = doy - (153 * mp + 2) / 5 + 1;
  *m = mp < 10 ? mp + 3 : mp - 9;
  *y = y0 + (*m <= 2);
}

int64_t neut_core_v0_57_civil_to_utc(neut_int y, neut_int mo, neut_int d,
                                     neut_int h, neut_int mi, neut_int s) {
  int64_t days = neut_core_v0_57_days_from_civil((int64_t)y, (int64_t)mo,
                                                 (int64_t)d);
  return days * 86400 + (int64_t)h * 3600 + (int64_t)mi * 60 + (int64_t)s;
}

static int64_t neut_core_v0_57_tm_to_utc(const struct tm *tm) {
  return neut_core_v0_57_civil_to_utc(
      (neut_int)tm->tm_year + 1900, (neut_int)tm->tm_mon + 1, (neut_int)tm->tm_mday,
      (neut_int)tm->tm_hour, (neut_int)tm->tm_min, (neut_int)tm->tm_sec);
}

typedef struct {
  neut_int sec;
  neut_int min;
  neut_int hour;
  neut_int mday;
  neut_int mon;
  neut_int year;
  neut_int wday;
  neut_int yday;
  neut_int isdst;
  neut_int gmtoff;
  const char *zone;
} neut_time_fields;

__attribute__((always_inline))
size_t neut_core_v0_57_sizeof_fields() {
  return sizeof(neut_time_fields);
}

static void neut_core_v0_57_fill_utc_fields(int64_t epoch,
                                            neut_time_fields *f) {
  int64_t days = epoch >= 0 ? epoch / 86400 : (epoch - 86399) / 86400;
  int64_t secs = epoch - days * 86400;
  int64_t y, m, d;
  neut_core_v0_57_civil_from_days(days, &y, &m, &d);
  f->sec = (neut_int)(secs % 60);
  f->min = (neut_int)((secs / 60) % 60);
  f->hour = (neut_int)(secs / 3600);
  f->mday = (neut_int)d;
  f->mon = (neut_int)m;
  f->year = (neut_int)y;
  int64_t wd = (days % 7 + 4) % 7;
  f->wday = (neut_int)(wd < 0 ? wd + 7 : wd);
  f->yday = (neut_int)(days - neut_core_v0_57_days_from_civil(y, 1, 1));
  f->isdst = 0;
  f->gmtoff = 0;
  f->zone = "UTC";
}

neut_time_fields *neut_core_v0_57_decompose(int64_t epoch, neut_int is_local,
                                            neut_time_fields *f) {
  if (is_local) {
    time_t t = (time_t)epoch;
    struct tm tm;
    if (localtime_r(&t, &tm) == NULL) {
      return NULL;
    }
    int64_t off = (int64_t)tm.tm_gmtoff;
    neut_core_v0_57_fill_utc_fields(epoch + off, f);
    f->isdst = (neut_int)tm.tm_isdst;
    f->gmtoff = (neut_int)off;
    f->zone = tm.tm_zone;
    return f;
  }
  neut_core_v0_57_fill_utc_fields(epoch, f);
  return f;
}

__attribute__((always_inline)) neut_int
neut_core_v0_57_field_sec(neut_time_fields *f) { return f->sec; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_min(neut_time_fields *f) { return f->min; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_hour(neut_time_fields *f) { return f->hour; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_mday(neut_time_fields *f) { return f->mday; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_mon(neut_time_fields *f) { return f->mon; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_year(neut_time_fields *f) { return f->year; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_wday(neut_time_fields *f) { return f->wday; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_yday(neut_time_fields *f) { return f->yday; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_isdst(neut_time_fields *f) { return f->isdst; }
__attribute__((always_inline)) neut_int
neut_core_v0_57_field_gmtoff(neut_time_fields *f) { return f->gmtoff; }
__attribute__((always_inline)) const char *
neut_core_v0_57_field_zone(neut_time_fields *f) { return f->zone; }

static int neut_core_v0_57_days_in_month(neut_int year, int month) {
  static const int dim[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12) {
    return 0;
  }
  if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
    return 29;
  }
  return dim[month - 1];
}

int32_t neut_core_v0_57_parse_iso8601(const char *datetime_str,
                                      struct timespec *ts) {
  struct tm tm = {0};
  char *rest = strptime(datetime_str, "%Y-%m-%dT%H:%M:%S", &tm);
  if (rest == NULL) {
    return -1;
  }
  if (tm.tm_mday < 1 ||
      tm.tm_mday >
          neut_core_v0_57_days_in_month((neut_int)tm.tm_year + 1900,
                                        tm.tm_mon + 1) ||
      tm.tm_hour < 0 || tm.tm_hour > 23 || tm.tm_min < 0 || tm.tm_min > 59 ||
      tm.tm_sec < 0 || tm.tm_sec > 59) {
    return -1;
  }
  int fraction = 0;
  if (*rest == '.') {
    rest++;
    if (*rest < '0' || *rest > '9') {
      return -1;
    }
    int digits = 0;
    while (*rest >= '0' && *rest <= '9') {
      if (digits < 9) {
        fraction = fraction * 10 + (*rest - '0');
      }
      digits++;
      rest++;
    }
    while (digits < 9) {
      fraction *= 10;
      digits++;
    }
  }
  if (strcmp(rest, "Z") == 0) {
    ts->tv_sec = (time_t)neut_core_v0_57_tm_to_utc(&tm);
    ts->tv_nsec = fraction;
    return 0;
  } else {
    int gmtoff_sign = 1;
    char sign;
    if (sscanf(rest, "%c", &sign) == 1) {
      rest += 1;
      if (sign == '+') {
        gmtoff_sign = 1;
      } else if (sign == '-') {
        gmtoff_sign = -1;
      } else {
        return -1;
      }
    } else {
      return -1;
    }
    struct tm mini_tm = {0};
    char *trail = NULL;
    trail = trail ?: strptime(rest, "%H:%M:%S", &mini_tm);
    trail = trail ?: strptime(rest, "%H:%M", &mini_tm);
    trail = trail ?: strptime(rest, "%H%M", &mini_tm);
    trail = trail ?: strptime(rest, "%H", &mini_tm);
    int gmtoff_abssec = 0;
    if (trail == NULL || strcmp(trail, "") != 0) {
      return -1;
    } else if (mini_tm.tm_hour < 0 || mini_tm.tm_hour > 23 ||
               mini_tm.tm_min < 0 || mini_tm.tm_min > 59 ||
               mini_tm.tm_sec < 0 || mini_tm.tm_sec > 59) {
      return -1;
    } else {
      gmtoff_abssec =
          mini_tm.tm_hour * 3600 + mini_tm.tm_min * 60 + mini_tm.tm_sec;
    }
    int gmtoff_sec = gmtoff_sign * gmtoff_abssec;
    ts->tv_sec = (time_t)neut_core_v0_57_tm_to_utc(&tm) - gmtoff_sec;
    ts->tv_nsec = fraction;
    return 0;
  }
}

__attribute__((always_inline)) char *
neut_core_v0_57_GET_DNAME(struct dirent *d) {
  return d->d_name;
}

__attribute__((always_inline)) uint8_t
neut_core_v0_57_GET_DTYPE(struct dirent *d) {
  return d->d_type;
}

neut_int neut_core_v0_57_readdir(DIR *dir, struct dirent **entry) {
  errno = 0;
  *entry = readdir(dir);
  if (*entry == NULL) {
    return errno == 0 ? 0 : -1;
  }
  return 1;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_FIFO() {
  return DT_FIFO;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_CHR() {
  return DT_CHR;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_DIR() {
  return DT_DIR;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_BLK() {
  return DT_BLK;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_REG() {
  return DT_REG;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_LNK() {
  return DT_LNK;
}

__attribute__((always_inline)) uint8_t neut_core_v0_57_DT_SOCK() {
  return DT_SOCK;
}

neut_int neut_core_v0_57_is_regular_file(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }
  return S_ISREG(st.st_mode) ? 1 : 0;
}

neut_int neut_core_v0_57_is_directory(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }
  return S_ISDIR(st.st_mode) ? 1 : 0;
}

neut_int neut_core_v0_57_mkdir(const char *path, neut_int mode) {
  return (neut_int)mkdir(path, (mode_t)mode);
}

neut_int neut_core_v0_57_get_modification_time(const char *path, neut_int *sec,
                                              neut_int *nsec) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return -1;
  }

  *sec = (neut_int)st.st_mtime;
#if defined(__APPLE__)
  *nsec = st.st_mtimespec.tv_nsec;
#elif defined(st_mtim) ||                                                      \
    (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L)
  *nsec = st.st_mtim.tv_nsec;
#else
  *nsec = 0;
#endif
  return 0;
}

neut_int neut_core_v0_57_unlink(const char *path) {
  return (neut_int)unlink(path);
}

neut_int neut_core_v0_57_rmdir(const char *path) {
  return (neut_int)rmdir(path);
}

static int neut_core_v0_57_open_directory_nofollow(int parent_fd,
                                                   const char *path) {
  int flags = O_RDONLY;
#ifdef O_CLOEXEC
  flags |= O_CLOEXEC;
#endif
#ifdef O_DIRECTORY
  flags |= O_DIRECTORY;
#endif
#ifdef O_NOFOLLOW
  flags |= O_NOFOLLOW;
#endif

  int fd = parent_fd >= 0 ? openat(parent_fd, path, flags) : open(path, flags);
  if (fd < 0) {
    return -1;
  }

  struct stat st;
  if (fstat(fd, &st) != 0) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }
  if (!S_ISDIR(st.st_mode)) {
    close(fd);
    errno = ENOTDIR;
    return -1;
  }

  return fd;
}

static int neut_core_v0_57_remove_directory_contents(int fd) {
  DIR *dir = fdopendir(fd);
  if (dir == NULL) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }

  for (;;) {
    errno = 0;
    struct dirent *entry = readdir(dir);
    if (entry == NULL) {
      if (errno != 0) {
        int saved_errno = errno;
        closedir(dir);
        errno = saved_errno;
        return -1;
      }
      break;
    }

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    int dir_fd = dirfd(dir);
    struct stat st;
    if (fstatat(dir_fd, entry->d_name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
      int saved_errno = errno;
      closedir(dir);
      errno = saved_errno;
      return -1;
    }

    if (S_ISDIR(st.st_mode)) {
      int child_fd =
          neut_core_v0_57_open_directory_nofollow(dir_fd, entry->d_name);
      if (child_fd < 0) {
        int saved_errno = errno;
        closedir(dir);
        errno = saved_errno;
        return -1;
      }
      if (neut_core_v0_57_remove_directory_contents(child_fd) != 0) {
        int saved_errno = errno;
        closedir(dir);
        errno = saved_errno;
        return -1;
      }
      if (unlinkat(dir_fd, entry->d_name, AT_REMOVEDIR) != 0) {
        int saved_errno = errno;
        closedir(dir);
        errno = saved_errno;
        return -1;
      }
    } else if (unlinkat(dir_fd, entry->d_name, 0) != 0) {
      int saved_errno = errno;
      closedir(dir);
      errno = saved_errno;
      return -1;
    }
  }

  return closedir(dir);
}

neut_int neut_core_v0_57_remove_directory(const char *path) {
  int fd = neut_core_v0_57_open_directory_nofollow(-1, path);
  if (fd < 0) {
    return -1;
  }

  if (neut_core_v0_57_remove_directory_contents(fd) != 0) {
    return -1;
  }

  return (neut_int)rmdir(path);
}
