#ifndef _IS_TYPEDEF_HEADER_DEFINED
#define _IS_TYPEDEF_HEADER_DEFINED

/**
 * @brief Represents a single byte
 */
typedef unsigned char byte;

/**
 * @typedef pthread_handler
 * @brief Shorthand of thread handler from pthread.h
 */
typedef void *(*pthread_handler)(void *);

/**
 * @brief Data structure of information about framebuffer characteristic
 */
typedef struct FBInfo
{
  unsigned short int bytePerScanline;
  unsigned short int bytePerPixel;
  unsigned short int screenWidth;
  unsigned short int screenHeight;
} FBInfo;

/**
 * @brief Data structure of a single pixel
 */
typedef struct PixelObject
{
  byte red;
  byte green;
  byte blue;

  // Will be AA part of 0xBBGGRRAA
  byte transparency;

  // Before write this pixel, skip until number of pixels as commanded by GIF file
  unsigned int skip;
} PixelObject;

/**
 * @brief Data structure of a single image
 */
typedef struct FrameObject
{
  PixelObject *pixels;
  unsigned int pixelCount;

  // Delay in milliseconds before display this frame
  unsigned long delay;
} FrameObject;

/**
 * @brief Data structure of decoded GIF animation
 */
typedef struct AnimObject
{
  FrameObject *frames;
  unsigned short frameCount;
} AnimObject;

/**
 * @brief Use "createDecodeParams(...)" to create new instance of this
 */
typedef struct DecodeParams
{
  const char *filePath;
  volatile AnimObject *target;
} DecodeParams;

#endif // #ifndef _IS_TYPEDEF_HEADER_DEFINED
