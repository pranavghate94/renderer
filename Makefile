build: clean
	cmake -B build
	cmake --build build

debug: clean
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

run:
	./build/renderer

clean:
	rm -rf build