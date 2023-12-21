CXX := g++

SRC := main.cpp

OUT := main

# Using pkg-config here because of problems with linking SDL files
SFML_CXXFLAGS := `pkg-config --cflags sfml-graphics sfml-window sfml-system`
SDL_CXXFLAGS := `pkg-config --cflags sdl2 SDL2_mixer`

SFML_LDFLAGS := `pkg-config --libs sfml-graphics sfml-window sfml-system`
SDL_LDFLAGS := `pkg-config --libs sdl2 SDL2_mixer`

CXXFLAGS := -std=c++17 $(SFML_CXXFLAGS) $(SDL_CXXFLAGS)
LDFLAGS := $(SFML_LDFLAGS) $(SDL_LDFLAGS)



$(OUT): $(SRC)
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT)

.PHONY: all
all: $(OUT)

