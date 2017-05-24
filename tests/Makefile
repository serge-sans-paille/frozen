SRCS=test_main.cpp test_set.cpp test_map.cpp test_unordered_set.cpp test_unordered_str_set.cpp test_unordered_map.cpp test_unordered_map_str.cpp

TARGET=test_main
CXXFLAGS=-O3 -g -Wall -std=c++14 -march=native
CPPFLAGS=-I../include

all:$(TARGET)

$(TARGET):$(patsubst %.cpp, %.o , $(SRCS))
	$(CXX) $^ -o $@

clean:
	$(RM) *.o $(TARGET)

.PHONY:check

check:$(TARGET)
	./$(TARGET)

test_main.o: test_main.cpp catch.hpp
test_map.o: test_map.cpp ../include/frozen/map.h \
  ../include/frozen/bits/algorithms.h catch.hpp
test_set.o: test_set.cpp ../include/frozen/set.h \
  ../include/frozen/bits/algorithms.h catch.hpp
test_unordered_map.o: test_unordered_map.cpp \
  ../include/frozen/unordered_map.h ../include/frozen/bits/elsa.h \
  ../include/frozen/bits/pmh.h \
  ../include/frozen/bits/algorithms.h catch.hpp
test_unordered_map_str.o: test_unordered_map_str.cpp \
  ../include/frozen/unordered_map.h ../include/frozen/bits/elsa.h \
  ../include/frozen/bits/pmh.h \
  ../include/frozen/bits/algorithms.h ../include/frozen/string.h \
  catch.hpp
test_unordered_set.o: test_unordered_set.cpp \
  ../include/frozen/unordered_set.h ../include/frozen/bits/pmh.h \
  ../include/frozen/bits/algorithms.h ../include/frozen/bits/elsa.h \
  catch.hpp
test_unordered_str_set.o: test_unordered_str_set.cpp \
  ../include/frozen/unordered_set.h ../include/frozen/bits/pmh.h \
  ../include/frozen/bits/algorithms.h ../include/frozen/bits/elsa.h \
  ../include/frozen/string.h \
  catch.hpp
