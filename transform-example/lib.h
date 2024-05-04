#pragma once

struct SomeObject {
  int a = 1;
};

SomeObject* GetSingletonObject();

bool AwesomeFunc(SomeObject* ptr);

bool AwesomeFunc(SomeObject& ref);