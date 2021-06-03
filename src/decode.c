#include <stdlib.h>

#include "function.h"
#include "globalVars.h"
#include "gif_lib.h"

void decode(DecodeParams *params)
{
  // Unpack the parameters
  const char *filePath = params->filePath;
  volatile AnimObject *target = params->target;

  // Check file existence
  if (!isFileExist(filePath))
  {
    fprintf(stderr, "File \"%s\" not found\r\n", filePath);
    return;
  }

  // Open access to GIF file with GIFLIB
  GifFileType *gifObject = NULL;
  {
    int errorCode = 0;
    gifObject = DGifOpenFileName(filePath, &errorCode);

    // Check for errors
    if (gifObject == NULL)
    {
      if (errorCode != 0)
        fprintf(stderr, "[%s] GIFLIB ERROR: %s\r\n", filePath, GifErrorString(errorCode));
      else
        fprintf(stderr, "[%s] ERROR: GIF Object is NULL without error, probably bug from GIFLIB\r\n", filePath);
      return;
    }
  }

  // Read entire data, this will block the current (worker) thread
  DGifSlurp(gifObject);

  // Make sure GIF size doesn't exceed screen size
  if (gifObject->SWidth > fbInfo.screenWidth || gifObject->SHeight > fbInfo.screenHeight)
  {
    fprintf(
        stderr, "[%s] ERROR: GIF Resolution Size (%dx%d) is bigger than display (%dx%d)\r\n",
        filePath,
        gifObject->SWidth, gifObject->SHeight,
        fbInfo.screenWidth, fbInfo.screenHeight);
    return;
  }

  // Prepare animation object
  target->frameCount = gifObject->ImageCount;
  if (target->frames != NULL)
    free(target->frames);
  target->frames = malloc(sizeof(FrameObject) * target->frameCount);

  // Prepare each frame
  for (unsigned short frameIndex = 0; frameIndex < target->frameCount; frameIndex++)
  {
    FrameObject *eachFrame = &(target->frames[frameIndex]);
    GifImageDesc *eachGifFrame = &(gifObject->SavedImages[frameIndex].ImageDesc);

    // Get GIF canvas size
    eachFrame->pixelCount = ((unsigned int)eachGifFrame->Width) * ((unsigned int)eachGifFrame->Height);

    // Create array of pixels
    eachFrame->pixels = malloc(sizeof(PixelObject) * eachFrame->pixelCount);
  }

  // Calculate offset to let image stays at center of screen
  unsigned int imageByteOffset = 0;
  {
    unsigned int screenWidth = fbInfo.screenWidth;
    unsigned int imageWidth = gifObject->SWidth;
    unsigned int offsetX = (screenWidth - imageWidth) / 2;

    unsigned int screenHeight = fbInfo.screenHeight;
    unsigned int imageHeight = gifObject->SHeight;
    unsigned int offsetY = (screenHeight - imageHeight) / 2;

    imageByteOffset = (offsetY * fbInfo.bytePerScanline) + (offsetX * fbInfo.bytePerPixel);
  }

  // Calculate how many bytes need to be skipped after last pixel of each line
  unsigned int newlineSkipSize = 0;
  {
    unsigned int screenWidth = fbInfo.screenWidth;
    unsigned int imageWidth = gifObject->SWidth;
    newlineSkipSize = screenWidth - imageWidth;
    newlineSkipSize *= fbInfo.bytePerPixel;
  }

  // Remember, we can actually skip the pixel for efficiency
  unsigned int skipPixel = 0;

  // Map pixels to ready-to-use object
  for (unsigned short frameIndex = 0; frameIndex < gifObject->ImageCount; frameIndex++)
  {
    SavedImage *gifFrame = &(gifObject->SavedImages[frameIndex]);
    FrameObject *targetFrame = &(target->frames[frameIndex]);

    // Get per-frame Color Map, with global Color Map as fallback
    ColorMapObject *colorMap = NULL;
    if (gifFrame->ImageDesc.ColorMap != NULL)
      colorMap = gifFrame->ImageDesc.ColorMap;
    else if (gifObject->SColorMap != NULL)
      colorMap = gifObject->SColorMap;
    else
      // Assume frame is empty if there's no ColorMap at all
      continue;

    // Get Graphic Control parameters
    GraphicsControlBlock gcb;
    DGifSavedExtensionToGCB(gifObject, frameIndex, &gcb);

    // Use those parameters
    targetFrame->delay = gcb.DelayTime * 10;
    byte indexOfTransparent = gcb.TransparentColor;

    // Put the frame at center of screen
    skipPixel = imageByteOffset;
    skipPixel += gifFrame->ImageDesc.Top * fbInfo.bytePerScanline;
    skipPixel += gifFrame->ImageDesc.Left * fbInfo.bytePerPixel;

    // Resolve size of newline byte size for current frame
    unsigned int perFrameNewlineSize = newlineSkipSize;
    perFrameNewlineSize += (gifObject->SWidth - gifFrame->ImageDesc.Width) * fbInfo.bytePerPixel;

    // Track position of X axis, to decide for inserting newline or not
    unsigned short posX = 0;

    // Trimmed means without transparent pixel
    unsigned int trimmedPixelIndex = 0;

    // Process each pixel
    for (unsigned int pixelIndex = 0; pixelIndex < targetFrame->pixelCount; pixelIndex++)
    {
      byte colorMapIndex = gifFrame->RasterBits[pixelIndex];

      // Current pixel at first position of a line? Put newline before it
      if (posX >= gifFrame->ImageDesc.Width - 1)
      {
        posX = 0;
        skipPixel += perFrameNewlineSize;
      }
      else
      {
        posX++;
      }

      // Current pixel is transparent? skip it
      if (colorMapIndex == indexOfTransparent || (colorMapIndex == 0 && gcb.TransparentColor == 255))
      {
        skipPixel += fbInfo.bytePerPixel;
        continue;
      }

      // Prepare pointers
      GifColorType *fromGIF = &(colorMap->Colors[colorMapIndex]);
      PixelObject *toObject = &(targetFrame->pixels[trimmedPixelIndex]);

      // Map the color
      toObject->red = fromGIF->Red;
      toObject->green = fromGIF->Green;
      toObject->blue = fromGIF->Blue;

      // Do something with this if you found the way to map transparency
      toObject->transparency = 0;

      // Ask "play.cpp" to skip number of pixels before displaying this pixel
      toObject->skip = skipPixel;

      // Pixel is mapped, then increment this index
      trimmedPixelIndex++;

      // Reset skipPixel variable
      skipPixel = 0;
    }

    // Resolve pixel count without transparent pixel
    unsigned int trimmedPixelCount = trimmedPixelIndex + 1;

    // Due to pixel skipping, we have to trim the memory usage
    PixelObject *oldPixels = targetFrame->pixels;

    // Create new pixels array with trimmed size
    targetFrame->pixels = malloc(sizeof(PixelObject) * trimmedPixelIndex);

    // Bulk copy the pixels
    for (unsigned int pixelIndex = 0; pixelIndex < trimmedPixelIndex; pixelIndex++)
    {
      targetFrame->pixels[pixelIndex] = oldPixels[pixelIndex];
    }

    // Set pixel count as trimmed count
    targetFrame->pixelCount = trimmedPixelCount;

    // Remove old pixels from memory
    free(oldPixels);
  }

  // GIF Object cleanup
  free(gifObject);
}
