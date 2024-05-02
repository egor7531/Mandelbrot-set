all: compile link

test: compile link run

compile:
	g++ -mavx512vl -c *.cpp -IC:\SFML-2.6.1\include

link:
	g++ *.o -o Main -LC:\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system

clean:
	del *.o

run:	
	Main.exe
