all: main 

main: main.c playerlist.c game.c msglist.c
	  gcc -o main main.c playerlist.c game.c msglist.c -lncurses -lpthread



clean: 
		rm main