#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdio.h>
#include <time.h>

using namespace sf;

Text print_fps(void);
VertexArray get_points(float* center, float* scale);
void check_events(RenderWindow* window, float* center, float* scale);

const int   WIDTH   = 800, 
            HEIGHT  = 600,
            nMax    = 256;
const float dx      = 1/(float)WIDTH,
            dy      = 1/(float)HEIGHT,
            R2_MAX  = 100.f;
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
        VertexArray points = get_points(center, scale); 
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

VertexArray get_points(float* center, float* scale)
{   
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

void check_events(RenderWindow* window, float* center, float* scale)
{
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
                        xC -= dx * 100.f;
                        break;
                    case Keyboard::Right:
                        xC += dx * 100.f;
                        break;
                    case Keyboard::Up:
                        yC -= dy * 100.f;
                        break;
                    case Keyboard::Down:
                        yC += dy * 100.f;
                        break;
                    case Keyboard::Z:
                        scaleX += dx * 100.f;
                        scaleY += dy * 100.f;
                        break;
                    case Keyboard::X:
                        scaleX -= dx * 100.f;
                        scaleY -= dy * 100.f;
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