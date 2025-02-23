build: 
	gcc -Wall -Wno-missing-braces -std=c99 -g ./src/*.c -lSDL2 -lm -o renderer

build_opt: 
	gcc -Wall -Wno-missing-braces -std=c99 -g -O2 ./src/*.c -lSDL2 -lm -o renderer


run:
	./renderer

clean:
	rm renderer
