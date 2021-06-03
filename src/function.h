#ifndef _IS_FUNCTION_HEADER_DECLARED
#define _IS_FUNCTION_HEADER_DECLARED

#include <stdio.h>
#include <stdbool.h>

#include "typedef.h"

/**
 * @brief Get value from kernel API
 * @param charFilePath (string) Path to character device file, a kernel API
 * @param errorCode Pointer to a byte variable, it will be changed on error
 * @return Value, result of query
 */
extern unsigned int queryKernel(const char *charFilePath, byte *errorCode);

/**
 * @brief Get the Screen Size then put it into FBInfo
 * @param fbInfo Pointer to FBInfo object
 * @return Error code, 0 means success
 */
extern int getScreenSize(FBInfo *fbInfo);

/**
 * @brief "pthread_create(...)" function requires struct to pass parameters
 * @param filePath (string) Which file to decode?
 * @param target Pointer to targeted object
 * @return Pointer to packed params
 */
extern DecodeParams *createDecodeParams(const char *filePath, volatile AnimObject *target);

/**
 * @brief Load and decode GIF file
 * @note This function meant for different thread
 * @param params Pointer to packed params
 */
extern void decode(DecodeParams *params);

/**
 * @brief Play and render animation object
 * @param fbDev Pointer to framebuffer device file
 * @param anim Pointer to the animation object
 */
extern void play(FILE *fbDev, const volatile AnimObject *anim);

/**
 * @brief Check existence of a file, without open it first
 * @param filePath (string) Which file to be check?
 * @return true if exist
 * @return false if not exist
 */
extern bool isFileExist(const char *filePath);

/**
 * @brief Will be called if current process interrupted by OS
 * @param signal Just a formality, won't affect any behaviors
 */
extern void interruptHandler(int signal);

#endif // #ifndef _IS_FUNCTION_HEADER_DECLARED
