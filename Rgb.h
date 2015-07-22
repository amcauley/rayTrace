#pragma once
#ifndef RGB_H
#define RGB_H

class Rgb {
public:
  float r, g, b;
  Rgb();
  Rgb(float, float, float);
  Rgb operator+ (const Rgb c);
  Rgb operator* (const float c);
};

#endif //RGB_H