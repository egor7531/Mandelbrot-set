#ifndef MANDELBROTSET_H_INCLUDED
#define MANDELBROTSET_H_INCLUDED

#include <SFML/Graphics.hpp>

using namespace sf;

const int   WIDTH   = 800, 
            HEIGHT  = 600,
            nMax    = 256;
const float dx      = 1/(float)WIDTH,
            dy      = 1/(float)HEIGHT;

const int SIZE      = 8;

const float R2_MAX  = 100.f;
const float COEFF   = 100.f;

void check_events(RenderWindow* window, float* center, float* scale);
VertexArray get_points(float* center, float* scale);
VertexArray get_points_optimization(float* center, float* scale);

#endif // MANDELBROTSET_H_INCLUDED
