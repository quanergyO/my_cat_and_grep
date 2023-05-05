#include "s21_cat.h"

int main(int argc, char **argv) {
  option_handler_t *funcs = initOptions();
  size_t flagsCount = parseFlags(argc, argv, funcs);
  size_t fileIndexStart = flagsCount + 1;
  size_t funcsCount = 0;
  funcs = generateListOfFunc(funcs, &funcsCount);

  for (size_t i = fileIndexStart; i < (size_t)argc; ++i) {
    FILE *file = openFile(argv[i], "r");
    value value = initValue();

    while (!value.isEOF) {
      readLine(&value, file);
      for (size_t i = 0; i < funcsCount; ++i) {
        funcs[i](&value);
        if (value.isContinue) {
          break;
        }
      }
      if (value.isContinue) {
        value.isContinue = false;
        continue;
      }
      fputs(value.line, stdout);
      if (value.isEOF) break;
      printf("\n");
    }
    fclose(file);
    free(value.line);
  }

  free(funcs);

  return 0;
}

void readLine(value *value, FILE *file) {
  size_t i = 0;
  for (;; ++i) {
    value->line[i] = fgetc(file);
    if (value->line[i] == EOF) {
      value->len = i + 1;
      value->line[i] = '\0';
      value->isEOF = true;
      return;
    }

    if (value->line[i] == '\n') {
      value->line[i] = '\0';
      value->len = i + 1;
      return;
    }
  }
  value->len = i;
}

value initValue() {
  value value;
  value.len = 0;
  value.isOldLineEmplty = false;
  value.buffSize = 256;
  value.line = (char *)malloc(sizeof(char) * value.buffSize);
  value.lineNumber = 1;
  value.isContinue = false;
  value.isEOF = false;

  return value;
}

void func_b(value *value) {
  if (!isLineEmpty(value->line)) {
    addFormatStrInArr(value);
  }
}

void func_n(value *value) {
  if (value->isEOF) {
    return;
  }
  addFormatStrInArr(value);
}

void func_s(value *value) {
  bool isCurrentLineEmpty = isLineEmpty(value->line);
  if (value->isOldLineEmplty && isCurrentLineEmpty) {
    value->isContinue = true;
  }
  value->isOldLineEmplty = isCurrentLineEmpty;
}

void func_E(value *value) {
  if (value->isEOF) return;
  insertElementInArray(value, value->len - 1, '$');
}

void func_v(value *value) {
  for (size_t i = 0; i < value->len; ++i) {
    if (value->line[i] < -96) {
      replaceElementInArray(value, i, value->line[i] + 192);
      insertElementInArray(value, i, '^');
      insertElementInArray(value, i, '-');
      insertElementInArray(value, i, 'M');
      i += 3;
    } else if (value->line[i] < 0) {
      replaceElementInArray(value, i, value->line[i] + 128);
      insertElementInArray(value, i, '-');
      insertElementInArray(value, i, 'M');
      i += 2;
    } else if (value->line[i] == 9 || value->line[i] == 10 ||
               value->line[i] == 0) {
      continue;
    } else if (value->line[i] < 32) {
      replaceElementInArray(value, i, value->line[i] + 64);
      insertElementInArray(value, i, '^');
      i += 1;
    } else if (value->line[i] < 127) {
      continue;
    } else {
      replaceElementInArray(value, i, '?');
      insertElementInArray(value, i, '^');
    }
  }
}

void func_T(value *value) {
  for (size_t i = 0; i < value->len; ++i) {
    if (value->line[i] == '\t') {
      replaceElementInArray(value, i, '^');
      ++i;
      insertElementInArray(value, i, 'I');
    }
  }
}

void addFormatStrInArr(value *value) {
  if (value->buffSize <= value->len + 8) {
    value->buffSize = value->buffSize * 2;
    value->line = (char *)realloc(value->line, value->buffSize);
  }
  char *tmp = (char *)malloc(sizeof(char) * value->buffSize + 8);
  sprintf(tmp, "%6ld\t", value->lineNumber);
  strcat(tmp, value->line);
  strcpy(value->line, tmp);
  value->len += 7;
  free(tmp);
  value->lineNumber += 1;
}

bool isLineEmpty(const char *line) { return (line[0] == '\0'); }

option_handler_t *initOptions() {
  option_handler_t *funcs =
      (option_handler_t *)malloc(sizeof(option_handler_t) * 6);
  if (funcs == NULL) {
    printf("Exception allocate memory\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < FLAGSCOUNT; ++i) {
    funcs[i] = NULL;
  }
  return funcs;
}

option_handler_t *generateListOfFunc(option_handler_t *funcs, size_t *size) {
  size_t len = 0;
  for (size_t i = 0; i < FLAGSCOUNT; ++i) {
    if (funcs[i] != NULL) {
      len++;
    }
  }
  option_handler_t *listOfFunc =
      (option_handler_t *)malloc(sizeof(option_handler_t) * len);
  if (listOfFunc == NULL) {
    printf("Exception allocate memory\n");
    exit(EXIT_FAILURE);
  }
  size_t index = 0;
  for (size_t i = 0; i < FLAGSCOUNT; ++i) {
    if (funcs[i] != NULL) {
      listOfFunc[index++] = funcs[i];
    }
  }
  free(funcs);
  *size = len;
  return listOfFunc;
}

void replaceElementInArray(value *value, size_t indexReplace, char symbol) {
  value->line[indexReplace] = symbol;
}

void insertElementInArray(value *value, int indexInsert, char symbol) {
  if (value->len + 1 >= value->buffSize) {
    value->buffSize = value->buffSize * 2;
    value->line = (char *)realloc(value->line, value->buffSize);
    if (value->line == NULL) {
      printf("Exception allocate memory\n");
      exit(EXIT_FAILURE);
    }
  }
  for (int i = value->len; i >= indexInsert; --i) {
    value->line[i + 1] = value->line[i];
  }
  value->line[indexInsert] = symbol;
  value->len++;
}

FILE *openFile(char *filePath, char *mode) {
  FILE *file = fopen(filePath, mode);
  if (file == NULL) {
    printf("s21_cat: %s: No such file or directory\n", filePath);
    exit(EXIT_FAILURE);
  }

  return file;
}

size_t parseFlags(int argc, char **argv, option_handler_t *funcs) {
  // stderror
  if (argc < 2) {
    printf("Usage: s21_cat [OPTION]... [FILE]...\n");
    exit(EXIT_FAILURE);
  }
  size_t flagsCount = 0;
  for (size_t i = 1; argv[i][0] == '-'; ++i) {
    if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--number-nonblank")) {
      funcs[b] = func_b;
      funcs[n] = NULL;
      flagsCount++;
    } else if (!strcmp(argv[i], "-e")) {
      funcs[E] = func_E;
      funcs[v] = func_v;
      flagsCount++;
    } else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--number")) {
      if (funcs[b] == NULL) {
        funcs[n] = func_n;
      }
      flagsCount++;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--squeeze-blank")) {
      funcs[s] = func_s;
      flagsCount++;
    } else if (!strcmp(argv[i], "-t")) {
      funcs[T] = func_T;
      funcs[v] = func_v;
      flagsCount++;
    } else if (!strcmp(argv[i], "-T") || !strcmp(argv[i], "--show-tabs")) {
      funcs[T] = func_T;
      flagsCount++;
    } else if (!strcmp(argv[i], "-v") ||
               !strcmp(argv[i], "--show-nonprinting")) {
      funcs[v] = func_v;
      flagsCount++;
    } else if (!strcmp(argv[i], "-E") || !strcmp(argv[i], "--show-ends")) {
      funcs[E] = func_E;
      flagsCount++;
    } else if (!strcmp(argv[i], "--help")) {
      printf("Usage: s21_cat [OPTION]... [FILE]...\n");
      printf("Concatenate FILE(s) to standard output.\n\n");
      printf("With no FILE, or when FILE is -, read standard input.\n\n");
      printf(
          "  -b, --number-nonblank    number nonempty output lines, overrides "
          "-n\n");
      printf("  -e                       equivalent to -vE\n");
      printf("  -E, --show-ends          display $ at end of each line\n");
      printf("  -n, --number             number all output lines\n");
      printf(
          "  -s, --squeeze-blank      suppress repeated empty output lines\n");
      printf("  -t                       equivalent to -vT\n");
      printf("  -T, --show-tabs          display TAB characters as ^I\n");
      printf(
          "  -v, --show-nonprinting   use ^ and M- notation, except for LFD "
          "and TAB\n");
      printf("      --help     display this help and exit\n\n");
      printf("Examples:\n");
      printf(
          "  cat f - g  Output f's contents, then standard input, then g's "
          "contents.\n");
      printf("  cat        Copy standard input to standard output.\n\n");
      printf("QuanergY cat release:\n");
      printf("github: https://github.com/quanergyO\n");
      exit(EXIT_SUCCESS);
    } else {
      printf("s21_cat: invalid option -- '%s'\n", argv[i]);
      printf("Try 's21_cat --help' for more information.\n");
      exit(EXIT_SUCCESS);
    }
  }

  return flagsCount;
}
