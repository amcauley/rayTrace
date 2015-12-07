#include "Physics.hpp"
#include "Params.hpp"
#include <math.h>
#include <iostream>
#include <assert.h>

/* Handles reflection/refraction math.
    INPUT:
      inVecNormd  : normalized incidence vector
      norm        : normalized normal vector
      nIn         : index of refraction of input medium
      nOut        : index of refraction of output/transmission medium
    OUTPUT:
      mirrorVec   : reflected/mirrored vector
      glassVec    : refracted vector, NULL ptr if total internal reflection
      R           : reflection strength, 0.0 - 1.0. R + <transmission strength> = 1.0

Nice doc about the math: http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
*/
void physRefraction(Vec3& inVecNormd, Vec3& norm, float nIn, float nOut, Vec3& mirrorVec, Vec3** glassVec, float& R)
{
  float cosThetaI = inVecNormd.dot(norm*(-1.0f));
  float inOutRatio = nIn / nOut;
  float sin2ThetaT = inOutRatio*inOutRatio*(1.0f - cosThetaI*cosThetaI);

  /* Find reflection ray. This will exist even if total internal reflection (TIR) occurs. */
  mirrorVec = inVecNormd - norm*(inVecNormd.dot(norm))*2.0f;

  /* Note that we include the "=" case in our check.That case would have result in a ray completely perpendicular
    to the surface normal, which seems like a type of edge case we don't really need to care about handling. */
  if (sin2ThetaT >= 1.0f)
  {
    /* TIR */
    *glassVec = NULL;
    return;
  }

  float cosThetaT = (float)sqrt(1.0f - sin2ThetaT);

  *glassVec = new Vec3;
  **glassVec = inVecNormd*inOutRatio + norm*(inOutRatio*cosThetaI - cosThetaT);

  float Rperp = (nIn*cosThetaI - nOut*cosThetaT) / (nIn*cosThetaI + nOut*cosThetaT);
  float Rpar  = (nOut*cosThetaI - nIn*cosThetaT) / (nOut*cosThetaI + nIn*cosThetaT);

  Rperp *= Rperp;
  Rpar *= Rpar;

  R = (Rpar + Rperp)*0.5f;

  if ((R > 1.0f) || (R < 0.0f))
  {
    std::cout << "R ("<< R << ") out of range!\n";
    assert(0);
  }
}