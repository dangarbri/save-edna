_CFLAGS = -Wall -g
OBJECTS=main.o ecb_extract.o sql.o

.PHONY: all clean install

all: save_edna

save_edna: ${OBJECTS}
	$(CC) ${CFLAGS} ${_CFLAGS} $^ -o $@

%.o: %.c
	$(CC) ${CFLAGS} ${_CFLAGS} -o $@ -c $<

clean:
	rm *.o
	rm save_edna

install: save_edna
	cp $< /usr/local/bin

