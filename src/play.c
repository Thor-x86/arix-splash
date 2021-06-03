#include <unistd.h>
#include <stdlib.h>

#include "function.h"

void play(FILE *fbDev, const volatile AnimObject *anim)
{
  // For each frame
  for (unsigned short frameIndex = 0; frameIndex < anim->frameCount; frameIndex++)
  {
    FrameObject *currentFrame = &(anim->frames[frameIndex]);

    // Pre-display delay, if exist
    if (currentFrame->delay > 0)
      usleep(currentFrame->delay * 1000);

    // Reset framebuffer cursor
    fseek(fbDev, 0, SEEK_SET);

    // For each pixel
    for (unsigned int pixelIndex = 0; pixelIndex < currentFrame->pixelCount; pixelIndex++)
    {
      PixelObject *currentPixel = &(currentFrame->pixels[pixelIndex]);

      // Skip pixels, if needed
      if (currentPixel->skip > 1)
        fseek(fbDev, (long)currentPixel->skip, SEEK_CUR);

      // Write pixel colors in BGRA format
      byte pixelBuffer[4];
      pixelBuffer[0] = currentPixel->blue;
      pixelBuffer[1] = currentPixel->green;
      pixelBuffer[2] = currentPixel->red;
      pixelBuffer[3] = currentPixel->transparency;
      fwrite(pixelBuffer, 1, 4, fbDev);
    }

    // Flush to framebuffer, to trigger frame update
    fflush(fbDev);
  }
}
