OS_RAW=$(shell uname)
ifeq (${OS_RAW}, Darwin)
	OS=darwin
else
ifeq (${OS_RAW}, Linux)
	OS=linux
	ARCH_RAW=$(shell arch)
ifeq (${ARCH_RAW},x86_64)
	ARCH=x86-64
else
	ARCH=x86
endif
	ARCH_SUFFIX=-${ARCH}
else
	OS=${OS_RAW}
endif
endif

ifeq (${OS}, "darwin")
	SRC_EXT=m
	SO_EXT=dylib
	CC=clang
	CFLAGS=-dynamiclib -framework Foundation
else
	SRC_EXT=cpp
	SO_EXT=so
	CC=gcc
	CFLAGS=-DXDG_UNIX -shared -fPIC
endif

NAME=crane

TARGET_NAME=${OS}${ARCH_SUFFIX}/lib${NAME}.${SO_EXT}
TTARGET_NAME=${OS}${ARCH_SUFFIX}/test${NAME}
SRC_NAME=${NAME}.${SRC_EXT}

${TARGET_NAME}: ${SRC_NAME}
	#Build for ${OS}${ARCH_SUFFIX}
	${CC} ${CFLAGS} $< -o $@

${TTARGET_NAME}: test${SRC_NAME}
	${CC} -DXDG_UNIX -L${OS}${ARCH_SUFFIX} -l${NAME} -lstdc++ $< -o $@

#darwin/libcrane.dylib:
#	clang -dynamiclib -framework Foundation crane.m -o darwin/libcrane.dylib

#linux-x86-64/libcrane.so:
#	gcc -DXDG_UNIX -shared -fPIC crane.cpp -o linux-x86-64/libcrane.so

all:  ${TARGET_NAME} ${TTARGET_NAME}
#darwin/libcrane.dylib

clean:
#rm darwin/libcrane.dylib
	rm ${TARGET_NAME}
	rm ${TTARGET_NAME}
