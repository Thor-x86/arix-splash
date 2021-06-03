#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "globalVars.h"
#include "typedef.h"
#include "function.h"

// Error code reference: https://unix.stackexchange.com/a/326811

// Initialize global variable
volatile bool isStop = false;
FBInfo fbInfo;

// Ready-to-play animation objects
volatile AnimObject startAnim;
volatile AnimObject loopAnim;
volatile AnimObject endAnim;

int main(int argc, char *argv[])
{
  // Check the arguments
  if (argc != 2)
  {
    if (argc > 0)
      fprintf(stderr, "USAGE: %s <path_to_splash_dir>\r\n\r\n", argv[0]);
    else
      fprintf(stderr, "USAGE: splash <path_to_splash_dir>\r\n\r\n");
    return EINVAL;
  }

  // Path to framebuffer device file
  const char *fbPath = "/dev/fb0";

  // Making sure framebuffer available
  if (!isFileExist(fbPath))
  {
    fprintf(stderr, "ERROR: Cannot Access Framebuffer!\r\n(Did you using sudo?)\r\n\r\n");
    return EIO;
  }

  // Query framebuffer's byte-per-scanline value
  {
    byte errorCode = 0;
    fbInfo.bytePerScanline = queryKernel("/sys/class/graphics/fb0/stride", &errorCode);
    if (errorCode != 0)
      return errorCode;
  }

  // Query framebuffer's byte-per-pixel value
  {
    byte errorCode = 0;
    fbInfo.bytePerPixel = queryKernel("/sys/class/graphics/fb0/bits_per_pixel", &errorCode) / 8;
    if (errorCode != 0)
      return errorCode;
  }

  // Query framebuffer's screen size
  {
    int errorCode = getScreenSize(&fbInfo);
    if (errorCode != EXIT_SUCCESS)
      return errorCode;
  }

  // Get splash directory path
  char *splashDir = argv[1];
  size_t splashDirLength = strlen(splashDir);

  // Remove trailing slash, if exist
  if (splashDir[splashDirLength - 1] == '/')
  {
    splashDir[splashDirLength - 1] = '\0';
    splashDirLength--;
  }

  // Resolve "start.gif" file
  char startAnimPath[splashDirLength + 11];
  strcpy(startAnimPath, splashDir);
  strcat(startAnimPath, "/start.gif");

  // Resolve "loop.gif" file
  char loopAnimPath[splashDirLength + 10];
  strcpy(loopAnimPath, splashDir);
  strcat(loopAnimPath, "/loop.gif");

  // Resolve "end.gif" file
  char endAnimPath[splashDirLength + 9];
  strcpy(endAnimPath, splashDir);
  strcat(endAnimPath, "/end.gif");

  // Prepare decoder parameters
  DecodeParams *startAnimDecoderParams = createDecodeParams(startAnimPath, &startAnim);
  DecodeParams *loopAnimDecoderParams = createDecodeParams(loopAnimPath, &loopAnim);
  DecodeParams *endAnimDecoderParams = createDecodeParams(endAnimPath, &endAnim);

  // Decode GIF files, in multithread way
  pthread_t startAnimDecoder, loopAnimDecoder, endAnimDecoder;
  pthread_create(&startAnimDecoder, NULL, (pthread_handler)&decode, (void *)startAnimDecoderParams);
  pthread_create(&loopAnimDecoder, NULL, (pthread_handler)&decode, (void *)loopAnimDecoderParams);
  pthread_create(&endAnimDecoder, NULL, (pthread_handler)&decode, (void *)endAnimDecoderParams);

  // Open access to framebuffer device
  FILE *fbDev = fopen(fbPath, "wb");
  if (fbDev == NULL)
  {
    fprintf(stderr, "ERROR: File object \"/dev/fb0\" is null");
    return EIO;
  }

  // Before play start animation, wait for decoder thread to finish
  pthread_join(startAnimDecoder, NULL);

  // Play start animation
  play(fbDev, &startAnim);

  // Before play loop animation, wait for decoder thread to finish
  pthread_join(loopAnimDecoder, NULL);

  // Prepare "stop signal" interrupt handling
  signal(SIGTERM, interruptHandler);
  signal(SIGQUIT, interruptHandler);
  signal(SIGINT, interruptHandler);

  // Play loop animation until asked for stop
  while (!isStop)
  {
    if (loopAnim.frameCount > 0)
      play(fbDev, &loopAnim);
    else
      sleep(1);
  }

  // Before play end animation, wait for decoder thread to finish
  pthread_join(endAnimDecoder, NULL);

  // Play end animation
  play(fbDev, &endAnim);

  // Close access to framebuffer device, gracefully
  fclose(fbDev);

  // Program exited successfully
  return EXIT_SUCCESS;
}
