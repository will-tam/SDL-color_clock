CC=gcc
CFLAGS=`sdl2-config --cflags` -Wall -g #-03
LIBS=`sdl2-config --libs` -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

PRG=color_clock

OBJ=$(PRG).o

all: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(PRG)
	
$(OBJ): $(PRG).c

clean:
	@rm *.o || true
	@rm *.*~ || true
	@rm *.~ || true
	@rm *~ || true

clean_all:
	@rm *.o $(PRG) || true
	@rm *.*~ || true
	@rm *.~ || true
	@rm *~ || true
