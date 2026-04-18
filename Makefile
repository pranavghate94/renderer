build: clean
	cmake -B build
	cmake --build build

run:
	./build/renderer

clean:
	rm -rf build