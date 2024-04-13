#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <math.h>
#include <iostream>

using namespace sf;

const float dx      = 1/800.f,
            dy      = 1/800.f,
            r2Max   = 100.f;
const int   nMax    = 256;

const int width  = 800, 
          height = 600;
int main()
{   
    RenderWindow window(VideoMode(width, height), "The Mandelbrot set");
    VertexArray points(Points, width * height);
    Clock clock;

    while (window.isOpen())
    {
            Event event;
            while (window.pollEvent(event))
            {
                switch (event.type)
                {
                    case Event::Closed:
                        window.close();
                        break;
                    
                    /*case Event::KeyReleased:
                        if(event.key.code == Keyboard::Return)
                        {
                            if(ifFullScreen)
                            {
                                window.create(VideoMode(width, height), "The Mandelbrot set");
                                ifFullScreen = false;
                            }
                            else
                            {
                                window.create(VideoMode(width, height), "The Mandelbrot set", Style::Fullscreen);
                                ifFullScreen = true; 
                            }
                        }*/
                    default:
                        break;
                }
            }
float lastTime = 0;
    bool ifFullScreen = true;
    float xC = 0.f, yC = 0.f; 
    int index = 0;
            for(int iy = 0; iy < height; iy++)
            {
                float x0 = (           - 400.f) * dx + xC,
                    y0 = ( (float)iy - 300.f) * dy + yC; 

                for(int ix = 0; ix < width; ix++, x0 += dx)
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
                        if(r2 >= r2Max) break;

                        X = x2 - y2 + x0;
                        Y = xy + xy + y0;
                    }
                    int I = (int)(sqrtf( sqrtf( (float)N / (float)nMax ) ) * 255.f);
                    points[index].position = Vector2f(ix, iy);
                    points[index].color = Color{(unsigned char)(255-I), (unsigned char)(I%2*64), (unsigned char)I};
                    index++;
                }
            }   

            window.clear();    
            window.draw(points);
            window.display();
            float currentTime = clock.restart().asSeconds();
            float fps = 1.f / (currentTime - lastTime);
            lastTime = currentTime;
            printf("%.2lf\n", fps);
    }

    return 0;
}

