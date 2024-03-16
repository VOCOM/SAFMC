
#pragma once

#include <stdio.h>
#include <wiringPi.h>

class Carrier {
public:
  Carrier();
  ~Carrier();

  void Activate();
  void Release();

private:
  bool active = false;
};