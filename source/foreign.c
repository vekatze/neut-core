#ifdef __linux__
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE
#endif

#include <dirent.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

__attribute__((always_inline)) int neut_core_v0_55_wifexited(int stat) {
  return WIFEXITED(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wifsignaled(int stat) {
  return WIFSIGNALED(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wifstopped(int stat) {
  return WIFSTOPPED(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wexitstatus(int stat) {
  return WEXITSTATUS(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wtermsig(int stat) {
  return WTERMSIG(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wcoredump(int stat) {
  return WCOREDUMP(stat);
}

__attribute__((always_inline)) int neut_core_v0_55_wstopsig(int stat) {
  return WSTOPSIG(stat);
}

__attribute__((always_inline)) size_t neut_core_v0_55_word_size() {
  return sizeof(void *);
}

__attribute__((always_inline)) int neut_core_v0_55_O_RDONLY() {
  return O_RDONLY;
}

__attribute__((always_inline)) int neut_core_v0_55_O_WRONLY() {
  return O_WRONLY;
}

__attribute__((always_inline)) int neut_core_v0_55_O_RDWR() { return O_RDWR; }

__attribute__((always_inline)) int neut_core_v0_55_O_CREAT() { return O_CREAT; }

__attribute__((always_inline)) int neut_core_v0_55_O_APPEND() {
  return O_APPEND;
}

__attribute__((always_inline)) int neut_core_v0_55_O_TRUNC() { return O_TRUNC; }

__attribute__((always_inline)) int neut_core_v0_55_SEEK_SET() {
  return SEEK_SET;
}

__attribute__((always_inline)) int neut_core_v0_55_SEEK_CUR() {
  return SEEK_CUR;
}

__attribute__((always_inline)) int neut_core_v0_55_SEEK_END() {
  return SEEK_END;
}

__attribute__((always_inline)) size_t neut_core_v0_55_thread_size() {
  return sizeof(pthread_t);
}

__attribute__((always_inline)) size_t neut_core_v0_55_thread_mutex_size() {
  return sizeof(pthread_mutex_t);
}

__attribute__((always_inline)) size_t neut_core_v0_55_thread_cond_size() {
  return sizeof(pthread_cond_t);
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_atomic_load_int(int64_t *ptr) {
  return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_atomic_fetch_add_int(int64_t *ptr, int64_t value) {
  return __atomic_fetch_add(ptr, value, __ATOMIC_ACQ_REL);
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_atomic_fetch_sub_int(int64_t *ptr, int64_t value) {
  return __atomic_fetch_sub(ptr, value, __ATOMIC_ACQ_REL);
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_atomic_increment_if_positive_int(int64_t *ptr) {
  int64_t current = __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
  while (current > 0) {
    if (__atomic_compare_exchange_n(ptr, &current, current + 1, 1,
                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
      return current;
    }
  }
  return 0;
}

__attribute__((always_inline)) int neut_core_v0_55_errno() { return errno; }

__attribute__((always_inline)) uint32_t neut_core_v0_55_UINT32_MAX() {
  return UINT32_MAX;
}

static int64_t neut_core_v0_55_skip_underscores(const char *str,
                                                int64_t length, int64_t i) {
  while (i < length && str[i] == '_') {
    i++;
  }
  return i;
}

static void neut_core_v0_55_read_int_sign(const char *str, int64_t length,
                                          int64_t *i, int64_t *out_sign) {
  *i = neut_core_v0_55_skip_underscores(str, length, *i);
  *out_sign = 1;
  if (*i < length) {
    if (str[*i] == '-') {
      *out_sign = -1;
      (*i)++;
    } else if (str[*i] == '+') {
      (*i)++;
    }
  }
}

static int neut_core_v0_55_read_prefix(const char *str, int64_t length,
                                       int64_t *i, const char *prefix,
                                       int64_t prefix_length) {
  int64_t j = *i;
  for (int64_t k = 0; k < prefix_length; k++) {
    j = neut_core_v0_55_skip_underscores(str, length, j);
    if (j >= length || str[j] != prefix[k]) {
      return 0;
    }
    j++;
  }
  *i = j;
  return 1;
}

static int neut_core_v0_55_decimal_digit(unsigned char ch, int *out_digit) {
  if ('0' <= ch && ch <= '9') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_55_binary_digit(unsigned char ch, int *out_digit) {
  if (ch == '0' || ch == '1') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_55_octal_digit(unsigned char ch, int *out_digit) {
  if ('0' <= ch && ch <= '7') {
    *out_digit = ch - '0';
    return 1;
  }
  return 0;
}

static int neut_core_v0_55_hex_digit(unsigned char ch, int *out_digit) {
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

static int neut_core_v0_55_read_required_digits(
    const char *str, int64_t length, int64_t *i,
    int (*read_digit)(unsigned char, int *), int base, uint64_t *out_value) {
  uint64_t result = 0;
  int has_digits = 0;
  while (1) {
    int digit = 0;
    *i = neut_core_v0_55_skip_underscores(str, length, *i);
    if (*i >= length || !read_digit((unsigned char)str[*i], &digit)) {
      break;
    }
    result = result * (uint64_t)base + (uint64_t)digit;
    (*i)++;
    has_digits = 1;
  }
  if (!has_digits) {
    return 0;
  }
  *out_value = result;
  return 1;
}

static int64_t neut_core_v0_55_parse_integer_literal(
    const char *str, int64_t length, const char *prefix, int64_t prefix_length,
    int (*read_digit)(unsigned char, int *), int base, int64_t *out_value) {
  int64_t i = 0;
  int64_t sign = 1;
  uint64_t result = 0;
  neut_core_v0_55_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_55_read_prefix(str, length, &i, prefix, prefix_length)) {
    return -1;
  }
  if (!neut_core_v0_55_read_required_digits(str, length, &i, read_digit, base,
                                            &result)) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  if (sign < 0) {
    *out_value = (int64_t)(0 - result);
  } else {
    *out_value = (int64_t)result;
  }
  return 1;
}

static int64_t neut_core_v0_55_parse_integer_digits(
    const char *str, int64_t length, int (*read_digit)(unsigned char, int *),
    int base, int64_t *out_value) {
  int64_t i = 0;
  uint64_t result = 0;
  if (!neut_core_v0_55_read_required_digits(str, length, &i, read_digit, base,
                                            &result)) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  *out_value = (int64_t)result;
  return 1;
}

static int neut_core_v0_55_parse_normalized_double(const char *str,
                                                   int64_t length,
                                                   double *out_value) {
  if (length < 0) {
    return 0;
  }
  char *normalized = malloc((size_t)length + 1);
  if (normalized == NULL) {
    return 0;
  }
  size_t normalized_length = 0;
  for (int64_t i = 0; i < length; i++) {
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

int neut_core_v0_55_sleep(double duration) {
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

int64_t neut_core_v0_55_parse_binary(const char *str, int64_t length,
                                     int64_t *out_value) {
  return neut_core_v0_55_parse_integer_literal(
      str, length, "0b", 2, neut_core_v0_55_binary_digit, 2, out_value);
}

int64_t neut_core_v0_55_parse_binary_digits(const char *str, int64_t length,
                                            int64_t *out_value) {
  return neut_core_v0_55_parse_integer_digits(
      str, length, neut_core_v0_55_binary_digit, 2, out_value);
}

int64_t neut_core_v0_55_parse_octal(const char *str, int64_t length,
                                    int64_t *out_value) {
  return neut_core_v0_55_parse_integer_literal(
      str, length, "0o", 2, neut_core_v0_55_octal_digit, 8, out_value);
}

int64_t neut_core_v0_55_parse_octal_digits(const char *str, int64_t length,
                                           int64_t *out_value) {
  return neut_core_v0_55_parse_integer_digits(
      str, length, neut_core_v0_55_octal_digit, 8, out_value);
}

int64_t neut_core_v0_55_parse_decimal(const char *str, int64_t length,
                                      int64_t *out_value) {
  return neut_core_v0_55_parse_integer_literal(
      str, length, "", 0, neut_core_v0_55_decimal_digit, 10, out_value);
}

int64_t neut_core_v0_55_parse_decimal_digits(const char *str, int64_t length,
                                             int64_t *out_value) {
  return neut_core_v0_55_parse_integer_digits(
      str, length, neut_core_v0_55_decimal_digit, 10, out_value);
}

int64_t neut_core_v0_55_parse_hex(const char *str, int64_t length,
                                  int64_t *out_value) {
  return neut_core_v0_55_parse_integer_literal(
      str, length, "0x", 2, neut_core_v0_55_hex_digit, 16, out_value);
}

int64_t neut_core_v0_55_parse_hex_digits(const char *str, int64_t length,
                                         int64_t *out_value) {
  return neut_core_v0_55_parse_integer_digits(
      str, length, neut_core_v0_55_hex_digit, 16, out_value);
}

static int neut_core_v0_55_read_float_exponent(const char *str, int64_t length,
                                               int64_t *i, char marker) {
  int has_digits = 0;
  *i = neut_core_v0_55_skip_underscores(str, length, *i);
  if (*i >= length || str[*i] != marker) {
    return 1;
  }
  (*i)++;
  *i = neut_core_v0_55_skip_underscores(str, length, *i);
  if (*i < length) {
    if (str[*i] == '-') {
      (*i)++;
    } else if (str[*i] == '+') {
      (*i)++;
    }
  }
  while (1) {
    int digit = 0;
    *i = neut_core_v0_55_skip_underscores(str, length, *i);
    if (*i >= length ||
        !neut_core_v0_55_decimal_digit((unsigned char)str[*i], &digit)) {
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

static int64_t neut_core_v0_55_parse_decimal_double(const char *str,
                                                    int64_t length,
                                                    double *out_value) {
  int64_t i = 0;
  int64_t sign = 1;
  uint64_t ignored = 0;
  neut_core_v0_55_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_55_read_required_digits(
          str, length, &i, neut_core_v0_55_decimal_digit, 10, &ignored)) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i >= length || str[i] != '.') {
    return -1;
  }
  i++;
  if (!neut_core_v0_55_read_required_digits(
          str, length, &i, neut_core_v0_55_decimal_digit, 10, &ignored)) {
    return -1;
  }
  if (!neut_core_v0_55_read_float_exponent(str, length, &i, 'e')) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  double value = 0.0;
  if (!neut_core_v0_55_parse_normalized_double(str, length, &value)) {
    return -1;
  }
  *out_value = value;
  return 1;
}

static int64_t neut_core_v0_55_parse_hex_double(const char *str, int64_t length,
                                                double *out_value) {
  int64_t i = 0;
  int64_t sign = 1;
  uint64_t ignored = 0;
  neut_core_v0_55_read_int_sign(str, length, &i, &sign);
  if (!neut_core_v0_55_read_prefix(str, length, &i, "0x", 2)) {
    return -1;
  }
  if (!neut_core_v0_55_read_required_digits(
          str, length, &i, neut_core_v0_55_hex_digit, 16, &ignored)) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i < length && str[i] == '.') {
    i++;
  } else {
    return -1;
  }
  if (!neut_core_v0_55_read_required_digits(
          str, length, &i, neut_core_v0_55_hex_digit, 16, &ignored)) {
    return -1;
  }
  if (!neut_core_v0_55_read_float_exponent(str, length, &i, 'p')) {
    return -1;
  }
  i = neut_core_v0_55_skip_underscores(str, length, i);
  if (i != length) {
    return -1;
  }
  double value = 0.0;
  if (!neut_core_v0_55_parse_normalized_double(str, length, &value)) {
    return -1;
  }
  *out_value = value;
  return 1;
}

int64_t neut_core_v0_55_parse_double(const char *str, int64_t length,
                                     double *out_value) {
  if (neut_core_v0_55_parse_hex_double(str, length, out_value) > 0) {
    return 1;
  }
  return neut_core_v0_55_parse_decimal_double(str, length, out_value);
}

int64_t neut_core_v0_55_write_loop(int fd, const char *buf, size_t len) {
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

ssize_t neut_core_v0_55_write_line(int fd, size_t len, const char *s) {
  struct iovec iov[2] = {{.iov_base = (void *)s, .iov_len = len},
                         {.iov_base = (void *)"\n", .iov_len = 1}};
  return writev(fd, iov, 2);
}

int neut_core_v0_55_write_int64_core(char *buf_end, int64_t v, int add_nl) {
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

int64_t neut_core_v0_55_write_int64(int fd, int64_t v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_55_write_int64_core(end, v, 0);
  return neut_core_v0_55_write_loop(fd, end - len, (size_t)len);
}

int64_t neut_core_v0_55_write_int64_nl(int fd, int64_t v) {
  char buf[22];
  char *end = buf + sizeof(buf);
  int len = neut_core_v0_55_write_int64_core(end, v, 1);
  return neut_core_v0_55_write_loop(fd, end - len, (size_t)len);
}

size_t neut_core_v0_55_build_fixed_buf(char *buf, size_t cap, double value,
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

int64_t neut_core_v0_55_write_double(int fd, double value, int decimals) {
  char buf[64];
  size_t len =
      neut_core_v0_55_build_fixed_buf(buf, sizeof buf, value, decimals, 0);
  if (!len) {
    return -1;
  }

  return neut_core_v0_55_write_loop(fd, buf, len);
}

int64_t neut_core_v0_55_write_double_line(int fd, double value, int decimals) {
  char buf[65];
  size_t len =
      neut_core_v0_55_build_fixed_buf(buf, sizeof buf, value, decimals, 1);
  if (!len) {
    return -1;
  }

  return neut_core_v0_55_write_loop(fd, buf, len);
}

ssize_t neut_core_v0_55_int64_strlen(int64_t v) {
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

size_t neut_core_v0_55_double_strlen(double value, int decimals) {
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

__attribute__((always_inline)) int64_t
neut_core_v0_55_get_online_cpu_count(void) {
  int64_t n = (int64_t)sysconf(_SC_NPROCESSORS_ONLN);
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
#define ARCH_TAG ARCH_NONE
#endif

__attribute__((always_inline)) arch_tag_t neut_core_v0_55_get_arch_tag() {
  return ARCH_TAG;
}

__attribute__((always_inline))
clockid_t neut_core_v0_55_CLOCK_REALTIME() {
  return CLOCK_REALTIME;
}

__attribute__((always_inline)) size_t neut_core_v0_55_sizeof_timespec() {
  return sizeof(struct timespec);
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_timespec_sec(struct timespec *ts) {
  return (int64_t)ts->tv_sec;
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_timespec_nsec(struct timespec *ts) {
  return (int64_t)ts->tv_nsec;
}

static int64_t neut_core_v0_55_days_from_civil(int64_t y, int64_t m,
                                               int64_t d) {
  y -= m <= 2;
  int64_t era = (y >= 0 ? y : y - 399) / 400;
  int64_t yoe = y - era * 400;
  int64_t doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
  int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}

static void neut_core_v0_55_civil_from_days(int64_t z, int64_t *y, int64_t *m,
                                            int64_t *d) {
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

int64_t neut_core_v0_55_civil_to_utc(int64_t y, int64_t mo, int64_t d,
                                     int64_t h, int64_t mi, int64_t s) {
  return neut_core_v0_55_days_from_civil(y, mo, d) * 86400 + h * 3600 +
         mi * 60 + s;
}

static int64_t neut_core_v0_55_tm_to_utc(const struct tm *tm) {
  return neut_core_v0_55_civil_to_utc(
      (int64_t)tm->tm_year + 1900, (int64_t)tm->tm_mon + 1, (int64_t)tm->tm_mday,
      (int64_t)tm->tm_hour, (int64_t)tm->tm_min, (int64_t)tm->tm_sec);
}

typedef struct {
  int64_t sec;
  int64_t min;
  int64_t hour;
  int64_t mday;
  int64_t mon;
  int64_t year;
  int64_t wday;
  int64_t yday;
  int64_t isdst;
  int64_t gmtoff;
  const char *zone;
} neut_time_fields;

__attribute__((always_inline))
size_t neut_core_v0_55_sizeof_fields() {
  return sizeof(neut_time_fields);
}

static void neut_core_v0_55_fill_utc_fields(int64_t epoch,
                                            neut_time_fields *f) {
  int64_t days = epoch >= 0 ? epoch / 86400 : (epoch - 86399) / 86400;
  int64_t secs = epoch - days * 86400;
  int64_t y, m, d;
  neut_core_v0_55_civil_from_days(days, &y, &m, &d);
  f->sec = secs % 60;
  f->min = (secs / 60) % 60;
  f->hour = secs / 3600;
  f->mday = d;
  f->mon = m;
  f->year = y;
  int64_t wd = (days % 7 + 4) % 7;
  f->wday = wd < 0 ? wd + 7 : wd;
  f->yday = days - neut_core_v0_55_days_from_civil(y, 1, 1);
  f->isdst = 0;
  f->gmtoff = 0;
  f->zone = "UTC";
}

neut_time_fields *neut_core_v0_55_decompose(int64_t epoch, int64_t is_local,
                                            neut_time_fields *f) {
  if (is_local) {
    time_t t = (time_t)epoch;
    struct tm tm;
    if (localtime_r(&t, &tm) == NULL) {
      return NULL;
    }
    int64_t off = (int64_t)tm.tm_gmtoff;
    neut_core_v0_55_fill_utc_fields(epoch + off, f);
    f->isdst = (int64_t)tm.tm_isdst;
    f->gmtoff = off;
    f->zone = tm.tm_zone;
    return f;
  }
  neut_core_v0_55_fill_utc_fields(epoch, f);
  return f;
}

__attribute__((always_inline)) int64_t
neut_core_v0_55_field_sec(neut_time_fields *f) { return f->sec; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_min(neut_time_fields *f) { return f->min; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_hour(neut_time_fields *f) { return f->hour; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_mday(neut_time_fields *f) { return f->mday; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_mon(neut_time_fields *f) { return f->mon; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_year(neut_time_fields *f) { return f->year; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_wday(neut_time_fields *f) { return f->wday; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_yday(neut_time_fields *f) { return f->yday; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_isdst(neut_time_fields *f) { return f->isdst; }
__attribute__((always_inline)) int64_t
neut_core_v0_55_field_gmtoff(neut_time_fields *f) { return f->gmtoff; }
__attribute__((always_inline)) const char *
neut_core_v0_55_field_zone(neut_time_fields *f) { return f->zone; }

static int neut_core_v0_55_days_in_month(int64_t year, int month) {
  static const int dim[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12) {
    return 0;
  }
  if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
    return 29;
  }
  return dim[month - 1];
}

int32_t neut_core_v0_55_parse_iso8601(const char *datetime_str,
                                      struct timespec *ts) {
  struct tm tm = {0};
  char *rest = strptime(datetime_str, "%Y-%m-%dT%H:%M:%S", &tm);
  if (rest == NULL) {
    return -1;
  }
  if (tm.tm_mday < 1 ||
      tm.tm_mday >
          neut_core_v0_55_days_in_month((int64_t)tm.tm_year + 1900,
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
    ts->tv_sec = (time_t)neut_core_v0_55_tm_to_utc(&tm);
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
    ts->tv_sec = (time_t)neut_core_v0_55_tm_to_utc(&tm) - gmtoff_sec;
    ts->tv_nsec = fraction;
    return 0;
  }
}

__attribute__((always_inline)) char *
neut_core_v0_55_GET_DNAME(struct dirent *d) {
  return d->d_name;
}

__attribute__((always_inline)) uint8_t
neut_core_v0_55_GET_DTYPE(struct dirent *d) {
  return d->d_type;
}

int64_t neut_core_v0_55_readdir(DIR *dir, struct dirent **entry) {
  errno = 0;
  *entry = readdir(dir);
  if (*entry == NULL) {
    return errno == 0 ? 0 : -1;
  }
  return 1;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_FIFO() {
  return DT_FIFO;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_CHR() {
  return DT_CHR;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_DIR() {
  return DT_DIR;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_BLK() {
  return DT_BLK;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_REG() {
  return DT_REG;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_LNK() {
  return DT_LNK;
}

__attribute__((always_inline)) uint8_t neut_core_v0_55_DT_SOCK() {
  return DT_SOCK;
}

int64_t neut_core_v0_55_is_regular_file(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }
  return S_ISREG(st.st_mode) ? 1 : 0;
}

int64_t neut_core_v0_55_is_directory(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }
  return S_ISDIR(st.st_mode) ? 1 : 0;
}

int64_t neut_core_v0_55_mkdir(const char *path, mode_t mode) {
  return (int64_t)mkdir(path, mode);
}

int64_t neut_core_v0_55_get_modification_time(const char *path, int64_t *sec,
                                              int64_t *nsec) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return -1;
  }

  *sec = (int64_t)st.st_mtime;
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

int64_t neut_core_v0_55_unlink(const char *path) {
  return (int64_t)unlink(path);
}

int64_t neut_core_v0_55_rmdir(const char *path) {
  return (int64_t)rmdir(path);
}

static int neut_core_v0_55_open_directory_nofollow(int parent_fd,
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

static int neut_core_v0_55_remove_directory_contents(int fd) {
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
          neut_core_v0_55_open_directory_nofollow(dir_fd, entry->d_name);
      if (child_fd < 0) {
        int saved_errno = errno;
        closedir(dir);
        errno = saved_errno;
        return -1;
      }
      if (neut_core_v0_55_remove_directory_contents(child_fd) != 0) {
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

int64_t neut_core_v0_55_remove_directory(const char *path) {
  int fd = neut_core_v0_55_open_directory_nofollow(-1, path);
  if (fd < 0) {
    return -1;
  }

  if (neut_core_v0_55_remove_directory_contents(fd) != 0) {
    return -1;
  }

  return (int64_t)rmdir(path);
}
