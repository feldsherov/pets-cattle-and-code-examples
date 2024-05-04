#include <iostream>

#include "span.h"

int main() {
  Span event_first("some long explanation what is this event");
  Span event_second("another one long explanation");

  AwesomeFunc();
  std::cout << "I am alive!" << std::endl;
  return 0;
}