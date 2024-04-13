all: compile link

test: compile link run

compile:
	g++ -c *.cpp -IC:\SFML-2.6.1\include

link:
	g++ *.o -o main -LC:\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lmingw32 -lsfml-main

clean:
	del *.o

run:	
	main.exe
