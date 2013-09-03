OBJS := test.o unicode.o

GTEST_ROOT := gtest-1.7.0
CXXFLAGS := -Wall -std=c++11 -I $(GTEST_ROOT) -I $(GTEST_ROOT)/include

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

test: $(OBJS) $(GTEST_ROOT)/make/gtest_main.a
	$(LINK) -lpthread


-include $(OBJS:.o=.d)
