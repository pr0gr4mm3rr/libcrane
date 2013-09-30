darwin/libcrane.dylib:
	clang -dynamiclib -framework Foundation crane.m -o darwin/libcrane.dylib

all: darwin/libcrane.dylib

clean:
	rm darwin/libcrane.dylib
