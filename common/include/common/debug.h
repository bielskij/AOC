#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#include <cstdio>
#include <cinttypes>

#define DEBUG_LEVEL_ERR  1
#define DEBUG_LEVEL_WARN 2
#define DEBUG_LEVEL_LOG  3
#define DEBUG_LEVEL_DBG  4

#ifndef DEBUG_LEVEL
	#define DEBUG_LEVEL DEBUG_LEVEL_ERR
#endif

#if defined(DEBUG) && DEBUG == 1
#if DEBUG_LEVEL >= DEBUG_LEVEL_ERR
	#define ERR(x) { printf("[ERR %s:%d]: ", __func__, __LINE__); printf x; printf("\n"); }
#else
	#define ERR(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
	#define WRN(x) { printf("[WRN %s:%d]: ", __func__, __LINE__); printf x; printf("\n"); }
#else
	#define WRN(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_LOG
	#define LOG(x) { printf("[LOG %s:%d]: ", __func__, __LINE__); printf x; printf("\n"); }
#else
	#define LOG(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DBG
	#define DBG(x) { printf("[DBG %s:%d]: ", __func__, __LINE__); printf x; printf("\n"); }
#else
	#define DBG(x) do { } while (0);
#endif

#else
#define ERR(x) do { } while (0);
#define WRN(x) do { } while (0);
#define LOG(x) do { } while (0);
#define DBG(x) do { } while (0);
#endif

#define PRINTF(x) { printf x; printf("\n"); }

#endif /* COMMON_DEBUG_H_ */
