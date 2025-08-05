#ifndef DROWING_H
#define DROWING_H

#include "list.h"

void drowing_pole_file(int, int);
void drowing_pole_dir(WindowInfo_t *, int, int, int);
void drowing_pole_names(struct dirent *, int, int);
void clear_pole(int, int, int);

#endif
