#ifndef _IS_GLOBALVARS_HEADER_DECLARED
#define _IS_GLOBALVARS_HEADER_DECLARED

#include <stdbool.h>

#include "typedef.h"

/**
 * @brief True if required to play stop animation now, then exit
 */
extern volatile bool isStop;

/**
 * @brief Stores information about Framebuffer (display output)
 */
extern FBInfo fbInfo;

#endif // #ifndef _IS_GLOBALVARS_HEADER_DECLARED
