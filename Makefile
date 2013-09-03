OBJS := test.o unicode.o

CXXFLAGS := -Wall -std=c++11

MAKEDEPS = g++ $(CXXFLAGS) -MM $< -o $(@:.o=.d) -MT $@ -MP
COMPILE  = g++ $(CXXFLAGS) -c  $< -o $@
LINK     = g++ $(CXXFLAGS) $+ -o $@


.PHONY: all
all: test

.PHONY: clean
clean:
	rm -rf test test.exe *.o *.d


%.o: %.cpp
	$(MAKEDEPS)
	$(COMPILE)

test: $(OBJS)
	$(LINK)


-include $(OBJS:.o=.d)
