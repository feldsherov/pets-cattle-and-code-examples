#pragma once

struct SomeObject {
  int a = 1;
};

bool AwesomeFunc(SomeObject* ptr);

bool AwesomeFunc(SomeObject& ref);