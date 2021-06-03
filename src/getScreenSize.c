#include <stdlib.h>
#include <errno.h>

#include "function.h"

int getScreenSize(FBInfo *fbInfo)
{
  // Path to kernel API file
  const char *filePath = "/sys/class/graphics/fb0/virtual_size";

  // Making sure targeted file path is exist
  if (!isFileExist(filePath))
  {
    fprintf(stderr, "ERROR: Cannot access \"%s\" character device file\r\n\r\n", filePath);
    return EIO;
  }

  // Access the kernel API
  FILE *kernelAPI = fopen(filePath, "r");

  // Making sure kernel API not null
  if (kernelAPI == NULL)
  {
    fprintf(stderr, "ERROR: File object for \"%s\" is null\r\n\r\n", filePath);
    return EIO;
  }

  // Read the value from kernel API
  char buffer[16];
  fread(&buffer, 1, 16, kernelAPI);

  // Close the access to prevent unwanted behavior
  fclose(kernelAPI);

  // Shared buffer index
  byte bufferIndex = 0;

  // Parse for screen width
  {
    char paramBuffer[16];
    for (byte i = 0; i < 16; i++)
    {
      if (buffer[bufferIndex] == ',')
      {
        bufferIndex++;
        break;
      }
      paramBuffer[i] = buffer[bufferIndex];
      bufferIndex++;
    }
    fbInfo->screenWidth = atoi(paramBuffer);
  }

  // Parse for screen height
  {
    char paramBuffer[16];
    for (byte i = 0; i < 16; i++)
    {
      if (buffer[bufferIndex] == ',')
      {
        bufferIndex++;
        break;
      }
      paramBuffer[i] = buffer[bufferIndex];
      bufferIndex++;
    }
    fbInfo->screenHeight = atoi(paramBuffer);
  }

  // Query success
  return EXIT_SUCCESS;
}
