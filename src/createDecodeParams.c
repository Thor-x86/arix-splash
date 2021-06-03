#include <stdlib.h>

#include "function.h"

DecodeParams *createDecodeParams(const char *filePath, volatile AnimObject *target)
{
  DecodeParams *result = (DecodeParams *)malloc(sizeof(DecodeParams));
  result->filePath = filePath;
  result->target = target;
  return result;
}
