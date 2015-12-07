#include "Rgb.hpp"

Rgb::Rgb()
{
  r = g = b = 0;
}

Rgb::Rgb(float ah, float as, float av)
{
  r = ah;
  g = as;
  b = av;
}

Rgb Rgb::operator+ (const Rgb c)
{
  return Rgb(r + c.r, g +c.g, b + c.b);
}

Rgb Rgb::operator- (float c)
{
  return Rgb(r - c, g - c, b - c);
}

Rgb Rgb::operator* (const float c)
{
  return Rgb(r*c, g*c, b*c);
}