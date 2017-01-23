#ifndef LOGGING_H
#define LOGGING_H

#define NONE_LEVEL 0x00
#define ERROR_LEVEL 0x01
#define WARN_LEVEL 0x02
#define INFO_LEVEL 0x03
#define TRACE_LEVEL 0x04

#ifndef LOG_LEVEL
#define LOG_LEVEL ERROR_LEVEL
#endif

#define LOG(format, ...) fprintf(stderr, format "\n", __VA_ARGS__)

#ifndef LOG_HEADER
#define LOG_HEADER "%s:%d"
#define LOG_HEADVAR __FUNCTION__, __LINE__
#endif

#if LOG_LEVEL >= TRACE_LEVEL
#define TRACE(msg, args...) LOG(LOG_HEADER " |DEBUG| " msg, LOG_HEADVAR, ## args)
#else
#define TRACE(msg, args...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define INFO(msg, args...) LOG(LOG_HEADER " |INFO| " msg, LOG_HEADVAR, ## args)
#else
#define INFO(msg, args...)
#endif

#if LOG_LEVEL >= WARN_LEVEL
#define WARN(msg, args...) LOG(LOG_HEADER " |WARN| " msg, LOG_HEADVAR, ## args)
#else
#define WARN(msg, args...)
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define ERROR(msg, args...) LOG(LOG_HEADER " |ERROR| " msg, LOG_HEADVAR, ## args)
#else
#define ERROR(msg, args...)
#endif

#endif /* LOGGING_HH */
