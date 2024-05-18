#include <iostream>

#include "lib.h"

int main() {
  SomeObject obj;
  AwesomeFunc(&obj);
  AwesomeFunc(GetSingletonObject());

  std::cout << "I am alive!" << std::endl;
  return 0;
}
