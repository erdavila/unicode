OBJS := tests/test-utf8-encoder.o unicode/unicode.o
EXE  := tests/test-unicode

DEPS := $(OBJS:.o=.d)

GTEST_ROOT := gtest-1.7.0
CXXFLAGS := -Wall -std=c++11 -I . -I $(GTEST_ROOT) -I $(GTEST_ROOT)/include

MAKEDEPS = @ g++ $(CXXFLAGS) -MM $< -o $(@:.o=.d) -MT $@ -MP
COMPILE  =   g++ $(CXXFLAGS) -c  $< -o $@
LINK     =   g++ $(CXXFLAGS) $+ -o $@


.PHONY: all
all: $(EXE)

.PHONY: clean
clean:
	rm -rf $(EXE) $(OBJS) $(DEPS)

.PHONY: test
test: $(EXE)
	$<


%.o: %.cpp
	$(MAKEDEPS)
	$(COMPILE)

$(EXE): $(OBJS) $(GTEST_ROOT)/make/gtest_main.a
	$(LINK) -lpthread


-include $(DEPS)
