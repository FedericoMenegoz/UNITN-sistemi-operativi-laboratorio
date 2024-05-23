#ifndef UTILS_H
#define UTILS_H

//color for formatting
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define LBLUE "\033[0;36m"
#define DF "\033[0m"
#define BOLD "\e[1m"
#define UNBOLD "\e[m"

#endif

void red_color(int fd);
void default_color(int fd);
void bold(int fd);
void unbold(int fd);

void perr(char * msg);
int str_to_positive_int(char * ptr);
