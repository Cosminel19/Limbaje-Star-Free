build:
	g++ star.cpp -lm -Wall -Wextra -std=c++17 -o star

run:
	./star

clean:
	rm -f star

pack:
	zip -r 333CA_BuciuCosmin_Tema_LFA.zip star.cpp Makefile