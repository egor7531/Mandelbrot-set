#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <time.h>

#include "MandelbrotSet.h"

using namespace sf;

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
        VertexArray points = get_points_optimization(center, scale); 
        double timeFunction = ((double)(clock() - t)) / CLOCKS_PER_SEC;  
        
        window.draw(points);
        window.display();
        
        clock_t currentTime = clock();
        double fps = CLOCKS_PER_SEC / ((double)(currentTime - lastTime));
        lastTime = currentTime;
        printf("FPS: %.2f Time: %.2lf\n", fps, timeFunction);
    }

    return 0;
}