CXX := g++

SRC := main.cpp

OUT := main

SFML_INCLUDE := /opt/homebrew/Cellar/sfml/2.6.1/include
SFML_LIB := /opt/homebrew/Cellar/sfml/2.6.1/lib

CXXFLAGS := -I$(SFML_INCLUDE)
LDFLAGS := -L$(SFML_LIB) -lsfml-graphics -lsfml-window -lsfml-system

$(OUT): $(SRC)
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT)

.PHONY: all
all: $(OUT)

