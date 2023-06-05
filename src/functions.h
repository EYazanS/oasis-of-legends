#ifndef functions_h
#define functions_h

#define KILOBYTES(Value) ((Value)*1024LL)
#define MEGABYTES(Value) (KILOBYTES(Value) * 1024LL)
#define GIGABYTES(Value) (MEGABYTES(Value) * 1024LL)
#define TERABYTES(Value) (GIGABYTES(Value) * 1024LL)
#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof((Array)[0]))
#define MINIMUM(a, b) ((a < b) ? a : b)
#define MAXIMUM(a, b) ((a > b) ? a : b)

#endif