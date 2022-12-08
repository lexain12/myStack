CXX 	 =	g++ 
CXXFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla 
ASMFILES = Assembler/main.cpp Assembler/assemblerFunc.cpp
CPUFILES = main.cpp CPUfuctions.cpp
CLANGSTZ = clang++ -std=c++17 -fsanitize=address,undefined,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero

all: main

main: dir main.o loglib.o stack.o
	$(CXX) bin/main.o bin/Loglib.o bin/stack.o -o stack.out

dir:
	mkdir -p bin

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -c -o bin/main.o 

loglib.o: logs/Loglib.cpp
	$(CXX) $(CXXFLAGS) logs/Loglib.cpp -c -o bin/Loglib.o

stack.o: stack.cpp
	$(CXX) $(CXXFLAGS) stack.cpp -c -o bin/stack.o

sanitize: 
	$(CLANGSTZ) main.cpp logs/Loglib.cpp stack.cpp -o stack


