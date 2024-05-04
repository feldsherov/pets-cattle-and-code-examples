#include "lib.h"

bool AwesomeFunc(SomeObject* ptr) {
  return ptr->a == 0;
}

bool AwesomeFunc(SomeObject& ref) {
  return ref.a == 0;
}