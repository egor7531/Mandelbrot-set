#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <immintrin.h>

using namespace sf;

//=============================================================================================================================================================================================================
Text print_fps(void);
void check_events(RenderWindow* window, float* center, float* scale);

VertexArray get_points1(float* center, float* scale);
VertexArray get_points2(float* center, float* scale);
VertexArray get_points3(float* center, float* scale);
VertexArray get_points4(float* center, float* scale);
//=============================================================================================================================================================================================================
const int   WIDTH   = 800, 
            HEIGHT  = 600,
            nMax    = 256;
const float dx      = 1/(float)WIDTH,
            dy      = 1/(float)HEIGHT;

const int SIZE      = 4;

const float R2_MAX  = 100.f;
const float COEFF   = 100.f;
//=============================================================================================================================================================================================================
inline void mm_set_ps       (float mm[SIZE], float val0, float val1, float val2, float val3)
{
    mm[0] = val0; mm[1] = val1; mm[2] = val2; mm[3] = val3;
}

inline void mm_set_ps1      (float mm[SIZE], float val)                                          { for(int i = 0; i < SIZE; i++) mm[i] = val;                       }
inline void mm_cpy_ps       (float mm[SIZE], const float  mm2[SIZE])                             { for(int i = 0; i < SIZE; i++) mm[i] = mm2[i];                    }

inline void mm_add_ps       (float mm[SIZE],   const float mm1[SIZE], const float mm2[SIZE])     { for(int i = 0; i < SIZE; i++) mm[i] = mm1[i] + mm2[i];           }
inline void mm_sub_ps       (float mm[SIZE],   const float mm1[SIZE], const float mm2[SIZE])     { for(int i = 0; i < SIZE; i++) mm[i] = mm1[i] - mm2[i];           }
inline void mm_mul_ps       (float mm[SIZE],   const float mm1[SIZE], const float mm2[SIZE])     { for(int i = 0; i < SIZE; i++) mm[i] = mm1[i] * mm2[i];           }

inline void mm_sub_epi32    (int   mm[SIZE],   const int   mm1[SIZE], const int   mm2[SIZE])     { for(int i = 0; i < SIZE; i++) mm[i] = mm1[i] - mm2[i];           }

//inline void mm_cmple_ps     (int  cmp[SIZE],   const float mm1[SIZE], const float mm2[SIZE])     { for(int i = 0; i < SIZE; i++) if(mm1[i] <= mm2[i]) cmp[i] = -1;   }
inline __m128 mm_cmple_ps     (__m128 mm1, __m128 mm2)     { __m128 cmp; for(int i = 0; i < SIZE; i++) if((float)mm1[i] <= (float)mm2[i]) cmp[i] = -1;   return cmp;}

inline int  mm_movemask_ps  (const int cmp[SIZE])
{
    int mask = 0;
    for(int i = 0; i < SIZE; i++) 
    {
        if(cmp[i])
            mask = (mask << i) + 1;
    }
    
    return mask;
}
//=============================================================================================================================================================================================================

int main()
{   
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "The Mandelbrot set");

    float center[] = {0.f, 0.f}; 
    float  scale[] = {1.f, 1.f}; 
    clock_t lastTime = clock();

    while (window.isOpen())
    {
        check_events(&window, center, scale);
        window.clear();

        clock_t t = clock();  
        VertexArray points = get_points1(center, scale); 
        double timeFunction = ((double)(clock() - t)) / CLOCKS_PER_SEC;  
        
        window.draw(points);
        //window.draw(print_fps());
        window.display();
        
        clock_t currentTime = clock();
        double fps = CLOCKS_PER_SEC / ((double)(currentTime - lastTime));
        lastTime = currentTime;
        printf("FPS: %.2f Time: %.2lf\n", fps, timeFunction);
    }

    return 0;
}

Text print_fps(void)
{
    Font font;
    if (!font.loadFromFile("Fonts/ArialRegular.ttf"))
        printf("Don't upload a folder with fonts\n");
        
    Text text;
    text.setString("Hello world");
    text.setFont(font);
    text.setCharacterSize(24); 
    text.setFillColor(Color::Red);
    text.setStyle(Text::Bold | Text::Underlined);
    return text;
}

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
                        //xC += dx * 70.f;
                        break;
                    case Keyboard::X:
                        scaleX -= dx * COEFF;
                        scaleY -= dy * COEFF;
                        //xC -= dx * 70.f;
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

VertexArray get_points1(float* center, float* scale)
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

VertexArray get_points2(float* center, float* scale)
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
        float x0 = ((           - 1000.f) * dx + xC) * scaleX,
              y0 = (( (float)iy -  300.f) * dy + yC) * scaleY; 

        for(int ix = 0; ix < WIDTH; ix += SIZE, x0 += SIZE*dx)
        {
            float X0[SIZE] = {x0, x0 + dx, x0 + 2*dx, x0 + 3*dx};
            float Y0[SIZE] = {y0,      y0,        y0,        y0};

            float X[SIZE] = {}; for (int i = 0; i < SIZE; i++) X[i] = X0[i];
            float Y[SIZE] = {}; for (int i = 0; i < SIZE; i++) Y[i] = Y0[i];
            
            int N[SIZE] = {0, 0, 0, 0};
            for(int n = 0; n < nMax; n++)
            {
                float x2[SIZE] = {}; for(int i = 0; i < SIZE; i++) x2[i] = X[i] * X[i];
                float y2[SIZE] = {}; for(int i = 0; i < SIZE; i++) y2[i] = Y[i] * Y[i];
                float xy[SIZE] = {}; for(int i = 0; i < SIZE; i++) xy[i] = X[i] * Y[i]; 
                
                float r2[SIZE] = {}; for(int i = 0; i < SIZE; i++) r2[i] = x2[i] + y2[i]; 

                int cmp[SIZE] = {};
                for(int i = 0; i < SIZE; i++) if(r2[i] <= R2_MAX) cmp[i] = 1;

                int mask = 0;
                for(int i = 0; i < SIZE; i++) mask |= (cmp[i] << i);
                if(!mask) break;

                for(int i = 0; i < SIZE; i++) N[i] += cmp[i];

                for(int i = 0; i < SIZE; i++) X[i] = x2[i] - y2[i] + X0[i];
                for(int i = 0; i < SIZE; i++) Y[i] = xy[i] + xy[i] + Y0[i];
            }

            for(int i = 0; i < SIZE; i++)
            {
                int I = (int)(sqrtf( sqrtf( (float)N[i] / (float)nMax ) ) * 255.f);
                points[index].position = Vector2f(ix + i, iy);
                points[index].color = Color{(unsigned char)(255-I), (unsigned char)(I%2*64), (unsigned char)I};
                index++;
            }
        }
    }   

    return points;
}

VertexArray get_points3(float* center, float* scale)
{  
    assert(center != nullptr);
    assert(scale  != nullptr);

    VertexArray points(Points, WIDTH * HEIGHT);
    int index = 0;
    float xC = center[0], 
          yC = center[1];
    float scaleX = scale[0],
          scaleY = scale[1]; 
    float _3210[SIZE] = {}; mm_set_ps  (_3210, 0.f, 1.f, 2.f, 3.f);
    float r2max[SIZE] = {}; mm_set_ps1 (r2max, R2_MAX);
    
    for(int iy = 0; iy < HEIGHT; iy++) 
    {
        float x0 = ((           - 1000.f) * dx + xC) * scaleX,
              y0 = (( (float)iy -  300.f) * dy + yC) * scaleY; 

        for(int ix = 0; ix < WIDTH; ix += SIZE, x0 += SIZE*dx)
        {
            float DX[SIZE] = {}; mm_set_ps1 (DX, dx); mm_mul_ps (DX, DX, _3210);

            float X0[SIZE] = {}; mm_set_ps1 (X0, x0); mm_add_ps (X0, X0, DX);
            float Y0[SIZE] = {}; mm_set_ps1 (Y0, y0);

            float X[SIZE] = {}; mm_cpy_ps (X, X0);
            float Y[SIZE] = {}; mm_cpy_ps (Y, Y0);
            
            int   N[SIZE] = {0, 0, 0, 0};
            for(int n = 0; n < nMax; n++)
            {
                float x2[SIZE] = {}; mm_mul_ps (x2, X, X);
                float y2[SIZE] = {}; mm_mul_ps (y2, Y, Y);
                
                float r2[SIZE] = {}; mm_add_ps (r2, x2, y2);

                //int cmp[SIZE] = {}; mm_cmple_ps (cmp, r2, r2max);

                int mask ;//= mm_movemask_ps (cmp);
                if(!mask) break;

               // mm_sub_epi32 (N, N, cmp);
                float xy[SIZE] = {}; mm_mul_ps (xy, X, Y);

                mm_sub_ps (X, x2, y2);  mm_add_ps (X, X, X0); 
                mm_add_ps (Y, xy, xy);  mm_add_ps (Y, Y, Y0); 
            }

            for(int i = 0; i < SIZE; i++)
            {
                int I = (int)(sqrtf( sqrtf( (float)N[i] / (float)nMax ) ) * 255.f);
                points[index].position = Vector2f(ix + i, iy);
                points[index].color = Color{(unsigned char)(255-I), (unsigned char)(I%2*64), (unsigned char)I};
                index++;
            }
        }   
    }   

    return points;
}

/*VertexArray get_points4(float* center, float* scale)
{  
    assert(center != nullptr);
    assert(scale  != nullptr);

    VertexArray points(Points, WIDTH * HEIGHT);

    int index = 0;
    float xC = center[0], 
          yC = center[1];
    float scaleX = scale[0],
          scaleY = scale[1]; 

    const __m256 _76543210 = _mm256_set_ps (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
    const __m256 r2max = _mm256_set1_ps(R2_MAX);
    const __m256 _255  = _mm256_set1_ps (255.f);
    const __m256 nmax  = _mm256_set1_ps (nMax);
    
    for(int iy = 0; iy < HEIGHT; iy++) 
    {
        float x0 = ((           - 1000.f) * dx + xC),
              y0 = (( (float)iy -  300.f) * dy + yC) * scaleY; 

        for(int ix = 0; ix < WIDTH; ix += SIZE, x0 += SIZE*dx)
        {
            __m128 X0 = _mm_add_ps (_mm_set_ps1 (x0), _mm_mul_ps (_3210, _mm_set_ps1 (dx)));
            __m128 Y0 =             _mm_set_ps1 (y0);

            __m128 X = X0, Y = Y0;
            
            __m128i N = _mm_setzero_si128();
            for(int n = 0; n < nMax; n++)
            {
                __m128 x2 = _mm_mul_ps (X, X),
                       y2 = _mm_mul_ps (Y, Y);
                
                __m128 r2 = _mm_add_ps (x2, y2);

                __m128 cmp = _mm_cmple_ps (r2, r2max);
                int mask   = _mm_movemask_ps (cmp);
                if(!mask) break;

                N = _mm_sub_epi32 (N, _mm_castps_si128 (cmp));
                
                __m128 xy = _mm_mul_ps (X, Y);

                X = _mm_add_ps (_mm_sub_ps (x2, y2), X0); 
                X = _mm_add_ps (_mm_add_ps (xy, xy), Y0);
            }

            __m128 I = _mm_mul_ps (_mm_sqrt_ps (_mm_sqrt_ps (_mm_div_ps (_mm_cvtepi32_ps (N), nmax))), _255); 
            for(int i = 0; i < SIZE; i++)
            {   
                points[index].position = Vector2f(ix + i, iy);
                points[index].color = Color{(unsigned char)(255-I), (unsigned char)(I%2*64), (unsigned char)I};
                index++;
            }
        }
    }   

    return points;
}*/