darwin/libcrane.dylib:
	clang -dynamiclib -framework Foundation crane.m -o darwin/libcrane.dylib

linux-x86-64/libcrane.so:
	gcc -shared -fPIC crane.c -o linux-x86-64/libcrane.so

all:  linux-x86-64/libcrane.so
	#darwin/libcrane.dylib

clean:
	#rm darwin/libcrane.dylib
	rm linux-x86-64/libcrane.so
