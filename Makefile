CXX ?= c++
CXXFLAGS := -std=c++17 -O2 -Isrc
LDFLAGS :=

all: test

build/offline_synth: tests/offline_synth.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

build/test_thiran: tests/test_thiran_phase.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

test: build/offline_synth build/test_thiran
	@echo "== offline_synth =="
	@./build/offline_synth | head -n 8
	@echo "== test_thiran =="
	@./build/test_thiran

clean:
	rm -rf build
