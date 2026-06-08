gcc -std=c99 -o tetris -g main.c src/*.c src/bot/*.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# ./tetris