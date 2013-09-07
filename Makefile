SRCS := $(wildcard tests/*.cpp unicode/*.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)
EXE  := tests/test-unicode


GTEST_ROOT    := gtest-1.7.0
GTEST_LIB_DIR := $(GTEST_ROOT)/make
GTEST_LIB     := $(GTEST_LIB_DIR)/gtest_main.a
CXXFLAGS := -Wall -std=c++11 -I . -I $(GTEST_ROOT) -I $(GTEST_ROOT)/include -g

MAKEDEPS = @ g++ $(CXXFLAGS) -MM $< -o $(@:.o=.d) -MT $@ -MP
COMPILE  =   g++ $(CXXFLAGS) -c  $< -o $@
LINK     =   g++ $(CXXFLAGS) $+ -o $@ -lpthread


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

$(EXE): $(OBJS) $(GTEST_LIB)
	$(LINK)

$(GTEST_LIB):
	make -C $(GTEST_LIB_DIR)

-include $(DEPS)
