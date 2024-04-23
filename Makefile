all: compile link

test: compile link run

compile:
	g++ -c Main.cpp -IC:\SFML-2.6.1\include

link:
	g++ Main.o -o Main -LC:\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system

clean:
	del *.o

run:	
	Main.exe
