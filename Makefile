BUILD_DIR ?= build
BUILD_TYPE ?= Release

CONAN_FLAGS ?= --build=missing
CMAKE_FLAGS ?= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
               -DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake \
               -DCMAKE_PREFIX_PATH=$(BUILD_DIR)

.PHONY: all debug release conan cmake build test clean

all: release

debug:
	$(MAKE) build BUILD_TYPE=Debug

release:
	$(MAKE) build BUILD_TYPE=Release

conan:
	conan install . --output-folder=$(BUILD_DIR) $(CONAN_FLAGS) -s build_type=$(BUILD_TYPE)

cmake: conan
	cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)

build: cmake
	cmake --build $(BUILD_DIR) --parallel

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)
