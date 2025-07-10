#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

__attribute__((always_inline)) int neut_core_v0_51_wifexited(int stat) {
  return WIFEXITED(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wtermsig(int stat) {
  return WTERMSIG(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wcoredump(int stat) {
  return WCOREDUMP(stat);
}

__attribute__((always_inline)) int neut_core_v0_51_wstopsig(int stat) {
  return WSTOPSIG(stat);
}

__attribute__((always_inline)) size_t neut_core_v0_51_word_size() {
  return sizeof(void *);
}

__attribute__((always_inline)) int neut_core_v0_51_O_RDONLY() {
  return O_RDONLY;
}

__attribute__((always_inline)) int neut_core_v0_51_O_WRONLY() {
  return O_WRONLY;
}

__attribute__((always_inline)) int neut_core_v0_51_O_RDWR() { return O_RDWR; }

__attribute__((always_inline)) int neut_core_v0_51_O_CREAT() { return O_CREAT; }

__attribute__((always_inline)) int neut_core_v0_51_O_APPEND() {
  return O_APPEND;
}

__attribute__((always_inline)) int neut_core_v0_51_SEEK_SET() {
  return SEEK_SET;
}

__attribute__((always_inline)) int neut_core_v0_51_SEEK_CUR() {
  return SEEK_CUR;
}

__attribute__((always_inline)) int neut_core_v0_51_SEEK_END() {
  return SEEK_END;
}

__attribute__((always_inline)) size_t neut_core_v0_51_thread_size() {
  return sizeof(pthread_t);
}

__attribute__((always_inline)) size_t neut_core_v0_51_thread_mutex_size() {
  return sizeof(pthread_mutex_t);
}

__attribute__((always_inline)) size_t neut_core_v0_51_thread_cond_size() {
  return sizeof(pthread_cond_t);
}

__attribute__((always_inline)) int neut_core_v0_51_errno() { return errno; }

__attribute__((always_inline)) uint32_t neut_core_v0_51_UINT32_MAX() {
  return UINT32_MAX;
}

int neut_core_v0_51_sleep(double duration) {
  struct timespec ts;
  ts.tv_sec = (time_t)duration;
  ts.tv_nsec = (long)((duration - ts.tv_sec) * 1e9);
  return nanosleep(&ts, NULL);
}

int64_t neut_core_v0_51_parse_binary(const char *str, int64_t length,
                                     int64_t *out_value) {
  int64_t result = 0;
  int64_t i = 0;
  int64_t sign = 1;
  if (i < length) {
    if (str[i] == '-') {
      sign = -1;
      i++;
    } else if (str[i] == '+') {
      sign = 1;
      i++;
    }
  }
  if (length - i >= 2 && str[i] == '0' && str[i + 1] == 'b') {
    i += 2;
  }
  int has_digits = 0;
  while (i < length) {
    unsigned char ch = str[i];
    if (ch == '0' || ch == '1') {
      result = result * 2 + (ch - '0');
      i++;
      has_digits = 1;
    } else if (ch == '_') {
      i++;
    } else {
      break;
    }
  }
  if (!has_digits) {
    return -1;
  }
  if (i == length) {
    *out_value = sign * result;
    return 1;
  } else {
    return -1;
  }
}

int64_t neut_core_v0_51_parse_decimal(const char *str, int64_t length,
                                      int64_t *out_value) {
  int64_t result = 0;
  int64_t i = 0;
  int64_t sign = 1;
  if (i < length) {
    if (str[i] == '-') {
      sign = -1;
      i++;
    } else if (str[i] == '+') {
      sign = 1;
      i++;
    }
  }
  int has_digits = 0;
  while (i < length) {
    unsigned char ch = str[i];
    if (isdigit(ch)) {
      result = result * 10 + (ch - '0');
      i++;
      has_digits = 1;
    } else if (ch == '_') {
      i++;
    } else {
      break;
    }
  }
  if (!has_digits) {
    return -1;
  }
  if (i == length) {
    *out_value = sign * result;
    return 1;
  } else {
    return -1;
  }
}

int64_t neut_core_v0_51_parse_hex(const char *str, int64_t length,
                                  int64_t *out_value) {
  int64_t result = 0;
  int64_t i = 0;
  int64_t sign = 1;
  if (i < length) {
    if (str[i] == '-') {
      sign = -1;
      i++;
    } else if (str[i] == '+') {
      sign = 1;
      i++;
    }
  }
  if (length - i >= 2 && str[i] == '0' && str[i + 1] == 'x') {
    i += 2;
  }
  int has_digits = 0;
  while (i < length) {
    unsigned char ch = str[i];
    if (isdigit(ch)) {
      result = result * 16 + (ch - '0');
      i++;
      has_digits = 1;
    } else if ('a' <= ch && ch <= 'f') {
      result = result * 16 + (ch - 'a' + 10);
      i++;
      has_digits = 1;
    } else if ('A' <= ch && ch <= 'F') {
      result = result * 16 + (ch - 'A' + 10);
      i++;
      has_digits = 1;
    } else if (ch == '_') {
      i++;
    } else {
      break;
    }
  }
  if (!has_digits) {
    return -1;
  }
  if (i == length) {
    *out_value = sign * result;
    return 1;
  } else {
    return -1;
  }
}

int64_t neut_core_v0_51_parse_double(const char *str, int64_t length,
                                     double *out_value) {
  double result = 0.0;
  int64_t i = 0;
  double sign = 1.0;
  if (i < length) {
    unsigned char ch = str[i];
    if (ch == '-') {
      sign = -1.0;
      i++;
    } else if (ch == '+') {
      sign = 1.0;
      i++;
    } else if (ch == '_') {
      i++;
    }
  }
  int has_digits = 0;
  while (i < length) {
    unsigned char ch = str[i];
    if (isdigit(ch)) {
      result = result * 10.0 + (ch - '0');
      i++;
      has_digits = 1;
    } else if (ch == '_') {
      i++;
    } else {
      break;
    }
  }
  if (!has_digits) {
    return -1;
  }
  double fraction = 1.0;
  if (i < length && str[i] == '.') {
    i++;
    int has_fractions = 0;
    while (i < length) {
      unsigned char ch = str[i];
      if (isdigit(ch)) {
        fraction /= 10.0;
        result += (ch - '0') * fraction;
        i++;
        has_fractions = 1;
      } else if (ch == '_') {
        i++;
      } else {
        break;
      }
    }
    if (!has_fractions) {
      return -1;
    }
  }
  int exp_sign = 1;
  int exponent = 0;
  if (i < length && (str[i] == 'e' || str[i] == 'E')) {
    i++;
    if (i < length) {
      if (str[i] == '-') {
        exp_sign = -1;
        i++;
      } else if (str[i] == '+') {
        exp_sign = 1;
        i++;
      }
      while (i < length) {
        unsigned char ch = str[i];
        if (isdigit(ch)) {
          exponent = exponent * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
    } else {
      return -1;
    }
  }
  if (i == length) {
    *out_value = sign * result * pow(10, exp_sign * exponent);
    return 1;
  } else {
    return -1;
  }
}

int64_t neut_core_v0_51_write_loop(int fd, const char *buf, size_t len) {
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
  return (int64_t)len;
}

ssize_t neut_core_v0_51_write_line(int fd, size_t len, const char *s) {
  struct iovec iov[2] = {{.iov_base = (void *)s, .iov_len = len},
                         {.iov_base = (void *)"\n", .iov_len = 1}};
  return writev(fd, iov, 2);
}

int neut_core_v0_51_write_int64_core(char *buf_end, int64_t v, int add_nl) {
  char *p = buf_end;
  if (add_nl) {
    *--p = '\n';
  }

  uint64_t u = (v < 0) ? (uint64_t)(-(__int128)v) : (uint64_t)v;

  if (u == 0) {
    *--p = '0';
  }

  while (u) {
    uint64_t q = (uint64_t)(((__int128)u * 0xCCCCCCCCCCCCCCCDu) >> 67);
    uint32_t digit = (uint32_t)(u - q * 10);
    *--p = (char)('0' + digit);
    u = q;
  }
  if (v < 0) {
    *--p = '-';
  }

  return (int)(buf_end - p);
}

int64_t neut_core_v0_51_write_int64(int fd, int64_t v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_51_write_int64_core(end, v, 0);
  return neut_core_v0_51_write_loop(fd, end - len, (size_t)len);
}

int64_t neut_core_v0_51_write_int64_nl(int fd, int64_t v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_51_write_int64_core(end, v, 1);
  return neut_core_v0_51_write_loop(fd, end - len, (size_t)len);
}

size_t neut_core_v0_51_build_fixed_buf(char *buf, size_t cap, double value,
                                       int decimals, int add_nl) {
  if (decimals < 0) {
    errno = EINVAL;
    return 0;
  }

  if (decimals > 17) {
    decimals = 17;
  }

  if (cap < 64) {
    errno = EOVERFLOW;
    return 0;
  }

  int n = snprintf(buf, cap, "%.*f", decimals, value);
  if (n < 0 || (size_t)n >= cap) {
    errno = EOVERFLOW;
    return 0;
  }

  if (add_nl) {
    if ((size_t)n + 1 >= cap) {
      errno = EOVERFLOW;
      return 0;
    }
    buf[n++] = '\n';
    buf[n] = '\0';
  }
  return (size_t)n;
}

int64_t neut_core_v0_51_write_double(int fd, double value, int decimals) {
  char buf[64];
  size_t len =
      neut_core_v0_51_build_fixed_buf(buf, sizeof buf, value, decimals, 0);
  if (!len) {
    return -1;
  }

  return neut_core_v0_51_write_loop(fd, buf, len);
}

int64_t neut_core_v0_51_write_double_line(int fd, double value, int decimals) {
  char buf[65];
  size_t len =
      neut_core_v0_51_build_fixed_buf(buf, sizeof buf, value, decimals, 1);
  if (!len) {
    return -1;
  }

  return neut_core_v0_51_write_loop(fd, buf, len);
}

ssize_t neut_core_v0_51_int64_strlen(int64_t v) {
  uint64_t u = (v < 0) ? (uint64_t)(-v) : (uint64_t)v;
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

size_t neut_core_v0_51_double_strlen(double value, int decimals) {
  if (decimals < 0) {
    errno = EINVAL;
    return 0;
  }
  if (decimals > 17) {
    decimals = 17;
  }
  int n = snprintf(NULL, 0, "%.*f", decimals, value);
  if (n < 0) {
    errno = EOVERFLOW;
    return 0;
  }
  return (size_t)n;
}
