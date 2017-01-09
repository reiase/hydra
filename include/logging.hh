#ifndef LOGGING_HH
#define LOGGING_HH

#ifndef NOLOGGING

#include <cstdio>
#define LOG(format, ...) fprintf(stderr,"%s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)
#define INFO(format, ...) fprintf(stderr, "%s:%d:%s\t" format "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)

#else

#define INFO(...)

#endif

#endif /* LOGGING_HH */
