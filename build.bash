gcc -std=c99 -g main.c src/*.c ~/cold-clear/target/release/libcold_clear.a -I/home/antares/cold-clear/c-api -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -mbmi2 -o tetris
# ./tetris