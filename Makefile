w5p: w5p.c
	$(CC) -o w5p w5p.c

.PHONY: clean

clean:
	rm -f w5p
