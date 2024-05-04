#include "span.h"

Span::Span(std::string_view s) : id_{s} {
}

void AwesomeFunc() {
  Span awesome_event("explanation on awesomeness");
}