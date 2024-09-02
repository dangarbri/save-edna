CFLAGS=-Wall -g
OBJECTS=main.o ecb_extract.o bytes.o sql.o

.PHONY: all clean install

all: save_edna

save_edna: ${OBJECTS}
	$(CC) $^ -o $@

%.o: %.c
	$(CC) ${CFLAGS} -o $@ -c $<

clean:
	rm *.o

install: save_edna
	cp $< /usr/local/bin

