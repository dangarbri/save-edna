CFLAGS=-Wall -g
OBJECTS=main.o ecb_extract.o bytes.o sql.o

.PHONY: all clean install

all: ecb_extract

ecb_extract: ${OBJECTS}
	$(CC) $^ -o $@

%.o: %.c
	$(CC) ${CFLAGS} -o $@ -c $<

clean:
	rm *.o

install: ecb_extract
	cp ecb_extract /usr/local/bin

