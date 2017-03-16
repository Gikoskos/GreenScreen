CC = gcc
CFLAGS = -ggdb -Wall -std=c11 -O3
LINKER = -lgdi32 -luxtheme -lmsimg32 -ladvapi32
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
EXE = GreenScreen.exe


$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -mwindows $^ -o $@ $(LINKER)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	@del -f *.o $(EXE)
