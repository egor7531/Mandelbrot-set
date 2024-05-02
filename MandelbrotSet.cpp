#include <assert.h>
#include <immintrin.h>
#include <math.h>

#include "MandelbrotSet.h"

void check_events(RenderWindow* window, float* center, float* scale)
{
    assert(center != nullptr);
    assert(scale  != nullptr);

    Event event;
    float xC = center[0], 
          yC = center[1];
    float scaleX = scale[0],
          scaleY = scale[1]; 
          
    while (window->pollEvent(event))
    {
        switch (event.type)
        {
            case Event::Closed:
                window->close();
                break;
            
            case Event::KeyReleased:
                switch(event.key.code)
                { 
                    case Keyboard::Escape:
                        window->close();
                        break;
                    case Keyboard::Left:
                        xC -= dx * COEFF;
                        break;
                    case Keyboard::Right:
                        xC += dx * COEFF;
                        break;
                    case Keyboard::Up:
                        yC -= dy * COEFF;
                        break;
                    case Keyboard::Down:
                        yC += dy * COEFF;
                        break;
                    case Keyboard::Z:
                        scaleX += dx * COEFF;
                        scaleY += dy * COEFF;
                        break;
                    case Keyboard::X:
                        scaleX -= dx * COEFF;
                        scaleY -= dy * COEFF;
                        break;
                    default:
                        break;
                }

            default:
                break;
        }
    }

    center[0] = xC;
    center[1] = yC;

    scale[0] = scaleX;
    scale[1] = scaleY; 
}

VertexArray get_points(float* center, float* scale)
{   
    assert(center != nullptr);
    assert(scale  != nullptr);

    VertexArray points(Points, WIDTH * HEIGHT);
    int index = 0;
    float xC = center[0], 
          yC = center[1];
    float scaleX = scale[0],
          scaleY = scale[1]; 

    for(int iy = 0; iy < HEIGHT; iy++)
    {
        float x0 = ((           - 1000.f) * dx + xC),
              y0 = (( (float)iy -  300.f) * dy + yC) * scaleY; 

        for(int ix = 0; ix < WIDTH; ix++, x0 += dx)
        {
            float X = x0,
                  Y = y0;
            
            int N = 0;
            for( ; N < nMax; N++)
            {
                float x2 = X*X,
                      y2 = Y*Y,
                      xy = X*Y;  
                
                float r2 = x2 + y2;
                if(r2 >= R2_MAX) break;

                X = x2 - y2 + x0;
                Y = xy + xy + y0;
            }

            int I = (int)(sqrtf( sqrtf( (float)N / (float)nMax ) ) * 255.f);
            points[index].position = Vector2f(ix, iy);
            points[index].color = Color{(unsigned char)(255-I), (unsigned char)(I%2*64), (unsigned char)I};
            index++;
        }
    }   

    return points;
}

VertexArray get_points_optimization(float* center, float* scale)
{  
    assert(center != nullptr);
    assert(scale  != nullptr);

    VertexArray points(Points, WIDTH * HEIGHT);

    int index = 0;
    float xC = center[0], 
          yC = center[1];
    float scaleX = scale[0],
          scaleY = scale[1]; 

    const __m256 _76543210  = _mm256_set_ps  (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
    const __m256 r2max      = _mm256_set1_ps (R2_MAX);
    const __m256 _255       = _mm256_set1_ps (255.f);
    const __m256 nmax       = _mm256_set1_ps (nMax);

    for(int iy = 0; iy < HEIGHT; iy++) 
    {
        float x0 = ((           - 1000.f) * dx + xC),
              y0 = (( (float)iy -  300.f) * dy + yC) * scaleY; 

        for(int ix = 0; ix < WIDTH; ix += SIZE, x0 += SIZE*dx)
        {
            __m256 X0 = _mm256_add_ps (_mm256_set1_ps (x0), _mm256_mul_ps (_76543210, _mm256_set1_ps (dx)));
            __m256 Y0 =                _mm256_set1_ps (y0);

            __m256 X = X0, Y = Y0;
            
            __m256i N = _mm256_setzero_si256(); 

            for(int n = 0; n < nMax; n++)
            {
                __m256 x2 = _mm256_mul_ps (X, X),
                       y2 = _mm256_mul_ps (Y, Y);
                
                __m256 r2 = _mm256_add_ps (x2, y2);

                __m256 cmp = _mm256_cmp_ps (r2, r2max, _CMP_LE_OS); 
                int mask   = _mm256_movemask_ps (cmp);
                if (!mask) break;

                N = _mm256_sub_epi32 (N, _mm256_castps_si256 (cmp));
                
                __m256 xy = _mm256_mul_ps (X, Y);

                X = _mm256_add_ps (_mm256_sub_ps (x2, y2), X0); 
                Y = _mm256_add_ps (_mm256_add_ps (xy, xy), Y0);
            }
            
            __m256 I = _mm256_mul_ps (_mm256_sqrt_ps (_mm256_sqrt_ps (_mm256_div_ps (_mm256_cvtepi32_ps (N), nmax))), _255); 
            //int* n = (int*) &N; 

            for(int i = 0; i < SIZE; i++)
            {   
                int* pI = (int*) &I;
                points[index].position = Vector2f(ix + i, iy);
                points[index].color = Color{(unsigned char)(255-pI[i]), (unsigned char)(pI[i]%2*64), (unsigned char)pI[i]};
                index++;
            }
        }
    }   

    return points;
}