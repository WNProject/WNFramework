#include "WNUtils/inc/WNCrashHandler.h"

int __attribute__((noinline)) teste() {
  int* x = 0;
  *x = 5;
  return (*x);
}
int __attribute__((noinline)) testd() {
  volatile int a = teste();
  return (a);
}
int __attribute__((noinline)) testc() {
  volatile int a = testd();
  return (a);
}
int __attribute__((noinline)) testb() {
  volatile int a = testc();
  return (a);
}
int __attribute__((noinline)) testa() {
  volatile int a = testb();
  return (a);
}

int main(int argc, char** argv) {
  WNUtils::InitializeCrashHandler();
  testa();  // this should crash
  return (-1);
}
