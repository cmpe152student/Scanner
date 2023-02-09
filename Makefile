# Simpler makefile with implicit rules and wildcards.
# when all relevant files are the only C++ source-code files
# in the working directory.
# WARNING: Won't notice changes in header files.
app_name = scanner
sources  = $(wildcard *.cpp)
objects  = $(sources:.cpp=.o)
CPP      = clang++
CPPFLAGS = -std=c++20

$(app_name) : $(objects)
	$(CPP) $^ -o $@
clean :
	rm -f $(app_name) $(objects) *~
