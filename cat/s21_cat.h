#ifndef S21_CAT_H_
#define S21_CAT_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLAGSCOUNT 6

typedef enum {
  s,
  v,
  T,
  b,
  n,
  E,
} flagsPriority;

typedef struct {
  char *line;
  size_t len;
  size_t buffSize;
  size_t lineNumber;
  bool isOldLineEmplty;
  bool isContinue;
  bool isEOF;
} value;

typedef void (*option_handler_t)(value *value);

void func_s(value *value);
void func_E(value *value);
void func_b(value *value);
void func_n(value *value);
void func_v(value *value);
void func_T(value *value);

value initValue();
option_handler_t *initOptions();
option_handler_t *generateListOfFunc(option_handler_t *funcs, size_t *size);
size_t parseFlags(int argc, char **argv, option_handler_t *funcs);
void insertElementInArray(value *value, int indexInsert, char symbol);
void replaceElementInArray(value *value, size_t indexReplace, char symbol);
void readLine(value *value, FILE *file);
bool isLineEmpty(const char *line);
void addFormatStrInArr(value *value);

FILE *openFile(char *filePath, char *mode);
#endif
