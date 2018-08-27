
INTERLINES=32
LLVM_CONFIG=llvm-config

WFLAGS_BASE=-Wall -Wextra -Wmissing-field-initializers -Wunused-parameter \
			-Wold-style-definition -Wmissing-declarations -Wmissing-prototypes \
			-Wredundant-decls -Wmissing-noreturn -Wshadow -Wpointer-arith \
			-Wwrite-strings -Winline -Wformat-nonliteral -Wformat-security \
			-Wswitch-default -Winit-self -Wnested-externs -Wstrict-prototypes \
			-Wmissing-include-dirs -Wundef -Wmissing-format-attribute \
			-Wswitch-enum -Wswitch -Waggregate-return -pedantic

LLVM_MODULES = bitwriter core ipo mcjit scalaropts support vectorize x86codegen

CFLAGS=-g -std=c99 -I../include -I../../include/priv -I../../include $(WFLAGS) \
	   `$(LLVM_CONFIG) --cppflags` -D_POSIX_C_SOURCE=200809 \
	   -DSTENCIL_INTERLINES=$(INTERLINES) -O3 -mno-avx
LDFLAGS = `$(LLVM_CONFIG) --ldflags` -g -Wl,--as-needed -Wl,-z,now,-z,relro
LDLIBS = ../libdbrew-llvm.a ../../libdbrew.a \
        `$(LLVM_CONFIG) --libs $(LLVM_MODULES)` \
        -lstdc++ -lm -lz -ldl -lpthread -ltinfo

.PHONY: all
all: benchmark

benchmark: benchmark.c timer.c kernels.c

.PHONY: clean
clean:
	rm -f benchmark

