
default:
	@+make -C build

test:
	./anything2obj -v json -i example/info.json out.o
	gcc -o testing example/testing.c out.o -Wall -g

clean:
	@rm -f build/*.o anything2obj testing out.o
	@echo "Clean!"

