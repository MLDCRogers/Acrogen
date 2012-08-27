CFLAGS='-std=c99'

all:
	$(CC) $(CFLAGS) main.c -o acrogen

clean:
	rm -f acrogen
