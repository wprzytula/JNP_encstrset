CFLAGS=-Wall -Wextra -std=c11 -O2
CPPFLAGS=-Wall -Wextra -std=c++17 -O2
NDEBUG="-DNDEBUG"

all: encstrset_test1 encstrset_test2 

ndebug: CPPFLAGS += -DNDEBUG
ndebug: CFLAGS += -DNDEBUG
ndebug: all

encstrset.o: encstrset.cc encstrset.h
	g++ $(CPPFLAGS) -c -o $@ $< 
encstrset_test1.o: encstrset_test1.c encstrset.h
	gcc $(CFLAGS) -c -o $@ $<
encstrset_test2.o: encstrset_test2.cc encstrset.h
	g++ $(CPPFLAGS) -c -o $@ $< 
encstrset_test1: encstrset_test1.o encstrset.o
	g++ $(CPPFLAGS) -o $@ $^
encstrset_test2: encstrset_test2.o encstrset.o
	g++ $(CPPFLAGS) -o $@ $^

clean:
	rm -f *.o encstrset_test1 encstrset_test2

