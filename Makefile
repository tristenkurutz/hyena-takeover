# Compiler and tools
CXX = g++
CXXFLAGS = -std=c++17 -O2

# Source, image, header
SRC = wallpaper.cpp
IMG = wallpaper.jpg
DATA_HEADER = wallpaper_data.h

# Output binaries
OUT_LINUX = compiles/yeen-linux
OUT_MAC = compiles/yeen-mac
OUT_WIN = compiles/yeen.exe

# Detect OS
OS ?= $(shell uname | tr '[:upper:]' '[:lower:]')

# Windows cross-compiler
MINGW_CXX = x86_64-w64-mingw32-g++
MINGW_CXXFLAGS = -std=c++17 -O2 -static-libgcc -static-libstdc++ -luser32

# Commands, not files
.PHONY: all clean

all: mkdir $(OUT_LINUX) $(OUT_MAC) $(OUT_WIN)
	@echo "Build complete!"

mkdir:
	@echo "Making compiles directory if needed..."
	@[ -d compiles ] || mkdir compiles

# Image -> Byte array compile
$(DATA_HEADER): $(IMG)
	@echo "Image to byte array compile..."
	xxd -i $(IMG) > $(DATA_HEADER)

# Linux build
$(OUT_LINUX): $(SRC) $(DATA_HEADER)
	@echo "Building Linux executable..."
	$(CXX) $(CXXFLAGS) $(SRC) -o $@

# Mac build
$(OUT_MAC): $(SRC) $(DATA_HEADER)
	@echo "Building macOS executable..."
	clang++ $(CXXFLAGS) $(SRC) -o $@

# Windows build with MinGW-w64
$(OUT_WIN): $(SRC) $(DATA_HEADER)
	@echo "Building Windows executable..."
	$(MINGW_CXX) $(MINGW_CXXFLAGS) $(SRC) -o $@

# Clean up!
clean:
	@echo "Cleaning up..."
	rm -f $(OUT_LINUX) $(OUT_MAC) $(OUT_WIN) $(DATA_HEADER)
