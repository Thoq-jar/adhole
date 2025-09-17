#ifndef LOGGER_H
#define LOGGER_H

void logger_info(const char* format, ...);
void logger_adweb(const char* format, ...);
void logger_error(const char* format, ...);
void logger_blocked(const char* format, ...);
void logger_allowed(const char* format, ...);
void logger_seperator(void);
void logger_n_line(void);

#endif
