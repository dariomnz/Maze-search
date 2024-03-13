#ifndef Debug_H
#define Debug_H

#include "stdio.h"

#define DEBUG_MSG(fmt, ...) fprintf(stderr, "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define DEBUG_ERROR(fmt, ...) fprintf(stderr, "[%s:%d] %s() ERROR: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif // Debug_H