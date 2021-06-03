#include "function.h"
#include "globalVars.h"

void interruptHandler(int signal)
{
  isStop = true;
}
