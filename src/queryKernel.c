#include <stdlib.h>
#include <errno.h>

#include "function.h"

unsigned int queryKernel(const char *charFilePath, byte *errorCode)
{
  // Making sure targeted file path is exist
  if (!isFileExist(charFilePath))
  {
    *errorCode = EIO;
    fprintf(stderr, "ERROR: Cannot access \"%s\" character device file\r\n\r\n", charFilePath);
    return 0;
  }

  // Access the kernel API
  FILE *kernelAPI = fopen(charFilePath, "r");

  // Making sure kernel API not null
  if (kernelAPI == NULL)
  {
    *errorCode = EIO;
    fprintf(stderr, "ERROR: File object for \"%s\" is null\r\n\r\n", charFilePath);
    return 0;
  }

  // Read the value from kernel API
  char buffer[8];
  fread(&buffer, 1, 8, kernelAPI);
  unsigned int queriedValue = atoi(buffer);

  // Close the access to prevent unwanted behavior
  fclose(kernelAPI);

  // Return the value
  return queriedValue;
}
