# Explicit Rules
CXX = g++-13

# Compiler Rules
CPPFLAGS = -DLINUX -std=c++20
LDFLAGS = 

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: BoneyJenkinsBot

%.o: %.c
	$(CXX) -c $(CPPFLAGS) $< -o $@

BoneyJenkinsBot: $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ -ldpp

clean:
	rm src/*.o
