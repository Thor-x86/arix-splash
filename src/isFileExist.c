#include <unistd.h>

#include "function.h"

bool isFileExist(const char *filePath)
{
  return (access(filePath, W_OK) == 0);
}
