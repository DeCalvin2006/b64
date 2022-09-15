#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Logging Funcions
 * Debug only
 * */
#define logger_help(...)                                                       \
  fprintf(stderr, "\x1b[32m"), fprintf(stderr, __VA_ARGS__),                   \
      fprintf(stderr, "\x1b[0m")

#define logger_error(...)                                                      \
  fprintf(stderr, "\x1b[31m"), fprintf(stderr, __VA_ARGS__),                   \
      fprintf(stderr, "\x1b[0m")

#ifdef _DEBUG
#define logger_log(...)                                                        \
  fprintf(stderr, "\x1b[33m"), fprintf(stderr, __VA_ARGS__),                   \
      fprintf(stderr, "\x1b[0m")

#else
#define logger_log(...)

#endif

char base[64] = {};

int table[256] = {};

void init_table() {
  for (int i = 0; i < 26; i++) {
    base[i] = 'A' + i;
  }
  for (int i = 0; i < 26; i++) {
    base[i + 26] = 'a' + i;
  }
  for (int i = 0; i < 10; i++) {
    base[i + 52] = '0' + i;
  }
  base[62] = '+';
  base[63] = '/';
  for (int i = 0; i < 64; i++) {
    table[(int)base[i]] = i;
  }
  table[(int)'='] = 0;
}

void print_bit(int k, int num) {
  if (num) {
    print_bit(k >> 1, num - 1);
    logger_log("%c", (k & 1) + '0');
  }
}

char *decode(const char *source) {
  size_t src_length = strlen(source);
  if (src_length % 4 != 0) {
    logger_error("Error: Wrongly encoded input string.\n Did you really think "
                 "you type or paste the string correctly?\n");
    return 0;
  }
  size_t res_length = (src_length / 4 * 3);
  res_length++; // last one ending char
  char *result = (char *)malloc(sizeof(char) * res_length);
  memset(result, 0, sizeof(char) * res_length);
  if (result == 0) {
    logger_error("Error: Failed to allocate memory.\n");
    return 0;
  }
  const char *i = source;
  char *dest = result;
  while (i + 4 < source + src_length) {
    int a[4] = {
        table[(int)*i],
        table[(int)*(i + 1)],
        table[(int)*(i + 2)],
        table[(int)*(i + 3)],
    };
    int c[3] = {
        (a[0] << 2) | (a[1] >> 4),
        (a[1] & 15) << 4 | a[2] >> 2,
        (a[2] & 3) << 6 | a[3],
    };
    dest[0] = c[0];
    dest[1] = c[1];
    dest[2] = c[2];
    dest += 3;
    i += 4;
  }
  if (i < source + src_length) {
    int a[4] = {};
    int cnt = 0;
    while (i < source + src_length) {
      a[cnt++] = table[(int)*i];
      i++;
    }
    int c[3] = {
        (a[0] << 2) | (a[1] >> 4),
        (a[1] & 15) << 4 | a[2] >> 2,
        (a[2] & 3),
    };
    dest[0] = c[0];
    dest[1] = c[1];
    dest[2] = c[2];
    dest += 3;
  }
  dest[0] = '\0';
  return result;
}

char *encode(const char *source) {
  size_t src_length = strlen(source);
  size_t res_length = ((src_length / 3 + (src_length % 3 > 0)) * 4);
  res_length++; // last one ending char

  char *result = (char *)malloc(sizeof(char) * res_length);
  if (result == 0) {
    logger_error("Error: Failed to allocate memory.\n");
    return 0;
  }
  memset(result, 0, sizeof(char) * res_length);
  const char *i = source;
  char *dest = result;
  while (i + 2 < source + src_length) {
    int a[3] = {
        *i,
        *(i + 1),
        *(i + 2),
    };
    int c[4] = {
        (a[0] >> 2),
        (a[0] & 3) << 4 | (a[1] >> 4),
        (a[1] & 15) << 2 | (a[2] >> 6),
        (a[2] & 63),
    };
    dest[0] = base[c[0]];
    dest[1] = base[c[1]];
    dest[2] = base[c[2]];
    dest[3] = base[c[3]];
    dest += 4;
    i += 3;
  }
  if (i != source + src_length) {
    int a[3] = {0, 0, 0};
    int cnt = 0;
    while (i < source + src_length) {
      a[cnt++] = *i;
      i++;
    }
    int c[4] = {
        (a[0] >> 2),
        (a[0] & 3) << 4 | (a[1] >> 4),
        (a[1] & 15) << 2 | (a[2] >> 6),
        (a[2] & 63),
    };
    dest[0] = base[c[0]];
    dest[1] = base[c[1]];
    if (cnt == 2) {
      dest[2] = base[c[2]];
    } else {
      dest[2] = '=';
    }
    dest[3] = '=';
    dest += 4;
  }
  *dest = '\0';
  return result;
}

char buf[BUFSIZ];
int main() {
  init_table();
  while (1) {
    memset(buf, 0, sizeof buf);
    logger_help("Input command:");
    char type = 0;
    type = getchar();
    if (type != 'd' && type != 'e') {
      logger_error("Error: Unknown command %c (expected: d for decode, "
                   "e for encode).\n",
                   type);
      goto end;
    }
    logger_help("Input string to decode:");
    scanf("%s", buf);
    char *result;
    switch (type) {
    case 'd': {
      result = decode(buf);
      if (result) {
        printf("%s\n", result);
      }
      break;
    }
    case 'e': {
      result = encode(buf);
      if (result) {
        printf("%s\n", result);
      }
      break;
    }
    }
    free(result);
  end:
    // delim all inputs.
    while (getchar() != '\n')
      ;
  }
}
