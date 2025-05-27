#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <stdint.h>
#include <float.h>


__attribute__((always_inline))
int neut_core_v0_51_wifexited(int stat) {
  return WIFEXITED(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wtermsig(int stat) {
  return WTERMSIG(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wcoredump(int stat) {
  return WCOREDUMP(stat);
}

__attribute__((always_inline))
int neut_core_v0_51_wstopsig(int stat) {
  return WSTOPSIG(stat);
}

__attribute__((always_inline))
size_t neut_core_v0_51_word_size() {
  return sizeof(void*);
}

__attribute__((always_inline))
int neut_core_v0_51_O_RDONLY() {
  return O_RDONLY;
}

__attribute__((always_inline))
int neut_core_v0_51_O_WRONLY() {
  return O_WRONLY;
}

__attribute__((always_inline))
int neut_core_v0_51_O_RDWR() {
  return O_RDWR;
}

__attribute__((always_inline))
int neut_core_v0_51_O_CREAT() {
  return O_CREAT;
}

__attribute__((always_inline))
int neut_core_v0_51_O_APPEND() {
  return O_APPEND;
}

__attribute__((always_inline))
int neut_core_v0_51_SEEK_SET() {
  return SEEK_SET;
}

__attribute__((always_inline))
int neut_core_v0_51_SEEK_CUR() {
  return SEEK_CUR;
}

__attribute__((always_inline))
int neut_core_v0_51_SEEK_END() {
  return SEEK_END;
}

__attribute__((always_inline))
size_t neut_core_v0_51_thread_size() {
  return sizeof(pthread_t);
}

__attribute__((always_inline))
size_t neut_core_v0_51_thread_mutex_size() {
  return sizeof(pthread_mutex_t);
}

__attribute__((always_inline))
size_t neut_core_v0_51_thread_cond_size() {
  return sizeof(pthread_cond_t);
}

__attribute__((always_inline))
int neut_core_v0_51_errno() {
  return errno;
}

__attribute__((always_inline))
uint32_t neut_core_v0_51_UINT32_MAX() {
  return UINT32_MAX;
}

int neut_core_v0_51_sleep(double duration) {
  struct timespec ts;
  ts.tv_sec = (time_t)duration;
  ts.tv_nsec = (long)((duration - ts.tv_sec) * 1e9);
  return nanosleep(&ts, NULL);
}

int64_t neut_core_v0_51_parse_binary(const char *str, int64_t length, int64_t *out_value) {
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

int64_t neut_core_v0_51_parse_decimal(const char *str, int64_t length, int64_t *out_value) {
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

int64_t neut_core_v0_51_parse_hex(const char *str, int64_t length, int64_t *out_value) {
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

int64_t neut_core_v0_51_parse_double(const char *str, int64_t length, double *out_value) {
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
