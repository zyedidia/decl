decl-verify-amd64: main.o verify.o flags.o
	$(CC) $^ -o $@ -lZydis -static
%.o: %.c
	$(CC) -c $^ -o $@ -DZYDIS_STATIC_BUILD -O2
clean:
	rm -f *.o decl-verify-amd64
