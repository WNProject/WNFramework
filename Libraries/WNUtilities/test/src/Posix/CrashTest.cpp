#include "WNUtilities/inc/WNCrashHandler.h"

int _WN_NO_INLINE teste() {
  int* x = 0;

  *x = 5;

  return *x;
}
int WN_NO_INLINE testd() {
  volatile int a = teste();

  return a;
}
int WN_NO_INLINE testc() {
  volatile int a = testd();

  return a;
}
int WN_NO_INLINE testb() {
  volatile int a = testc();

  return a;
}
int WN_NO_INLINE testa() {
  volatile int a = testb();

  return a;
}

int main(int argc, char** argv) {
  wn::utilities::initialize_crash_handler();

  testa();  // this should crash

  return -1;
}
