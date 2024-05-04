#pragma once

#include <string_view>
#include <string>

struct Span {
public:
    explicit Span(std::string_view s);
private:
  std::string id_;
};

void AwesomeFunc();