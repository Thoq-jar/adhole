#include "logger.h"
#include <stdio.h>
#include <stdarg.h>


void logger_info(const char* format, ...) {
  printf("\033[90m[\033[94m INFO \033[90m]\033[0m ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void logger_adhole(const char* format, ...) {
  printf("\033[90m[\033[95m ADHOLE \033[90m]\033[0m ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void logger_error(const char* format, ...) {
  printf("\033[90m[\033[91m ERROR \033[90m]\033[0m ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void logger_blocked(const char* format, ...) {
  printf("\033[90m[\033[91m BLOCKED \033[90m]\033[0m ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void logger_allowed(const char* format, ...) {
  printf("\033[90m[\033[92m ALLOWED \033[90m]\033[0m ");
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void logger_seperator(void) {
  printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
}