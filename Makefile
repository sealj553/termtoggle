CC       = gcc
LDFLAGS  = -lX11
INCLUDE  = 
CFLAGS   = -std=c11 -Wall -Wextra -Wfatal-errors -O2 -Wno-unused-parameter
CFLAGS  += $(INCLUDE)
TARGET   = termtoggle
SRCFILES = main.c
OBJECTS  = $(patsubst %.c, %.o, $(SRCFILES))

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(TARGET)
