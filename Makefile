.PHONY:clean

default: b64

ifndef RELEASE
CFLAGS := -g -D _DEBUG -fsanitize=address
else
CFLAGS := -O2
endif

b64: ./b64.c
	clang ./b64.c -o ./b64 -Wall ${CFLAGS}

clean:
	rm -rf ./b64 .cache/ compile_commands.json
