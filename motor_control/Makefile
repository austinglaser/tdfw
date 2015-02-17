CXX		:= g++
CPPFLAGS	:= -Wall -Wextra -g

PC		:= pkg-config
PCFLAGS		:= --cflags --libs
PCPKGS		:= glib-2.0
CPPFLAGS += $(shell $(PC) $(PCFLAGS) $(PCPKGS))

all: test

test: test.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.o test
