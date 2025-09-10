NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
DOCKER_COMPOSE ?= docker-compose

# Пути
INSTALL_BIN ?= /root/.local/bin/allocation

# NOTE: use Makefile.local to override the options defined above.
-include Makefile.local

.PHONY: all
all: test-debug test-release

# Python env
.PHONY: pip-install
pip-install: venv/touchfile

venv/touchfile: tests/requirements.txt
	python3 -m venv venv
	./venv/bin/pip install -r ./tests/requirements.txt
	touch venv/touchfile

.PHONY: e2e-test
e2e-test: pip-install
	./venv/bin/pytest -v --color=yes

# Conan
.PHONY: conan-profile
conan-profile:
	@if [ ! -f "$$HOME/.conan2/profiles/default" ]; then \
		echo "Conan default profile not found. Creating..."; \
		conan profile detect --force; \
	else \
		echo "Conan default profile exists."; \
	fi

.PHONY: conan-debug
conan-debug: conan-profile
	conan install . -s build_type=Debug --build=missing

.PHONY: conan-release
conan-release: conan-profile
	conan install . -s build_type=Release --build=missing

# CMake
.PHONY: cmake-debug
cmake-debug: conan-debug
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -B build/Debug -S .

.PHONY: cmake-release
cmake-release: conan-release
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -B build/Release -S .

build/Debug/CMakeCache.txt:
	$(MAKE) cmake-debug

build/Release/CMakeCache.txt:
	$(MAKE) cmake-release

# Build
build-debug: build/Debug/CMakeCache.txt
	cmake --build build/Debug -j $(NPROCS) --target allocation

build-release: build/Release/CMakeCache.txt
	cmake --build build/Release -j $(NPROCS) --target allocation

# Unit tests
.PHONY: test-debug
test-debug: build-debug
	cmake --build build/Debug -j $(NPROCS) --target allocation_test
	cd build/Debug && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

.PHONY: test-release
test-release: build-release
	cmake --build build/Release -j $(NPROCS) --target allocation_test
	cd build/Release && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

# Service start
.PHONY: start-debug
start-debug: build-debug
	cmake --build build/Debug --target start-debug

.PHONY: start-release
start-release: build-release
	cmake --build build/Release --target start-release

.PHONY: service-start-debug service-start-release
service-start-debug service-start-release: service-start-%: start-%

# Cleanup
.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	cmake --build build/$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build/*
	rm -rf tests/__pycache__/
	rm -rf tests/.pytest_cache/
	rm -rf venv

# Install
.PHONY: install-debug
install-debug: build-debug
	cmake --install build/Debug -v --component allocation

.PHONY: install-release
install-release: build-release
	cmake --install build/Release -v --component allocation

# Format
.PHONY: format
format:
	find src -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
	find tests -name '*.py' -type f | xargs autopep8 -i

# Environment
export DB_CONNECTION := postgresql://${POSTGRES_USER}:${POSTGRES_PASSWORD}@service-postgres:5432/${POSTGRES_DB}

# Internal hidden targets (docker)
--in-docker-start-debug --in-docker-start-release: --in-docker-start-%: install-%
	$(INSTALL_BIN)

# Run in docker
.PHONY: docker-start-debug docker-start-release
docker-start-debug docker-start-release: docker-start-%:
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm service-allocation make -- --in-docker-start-$*

.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-debug docker-start-service-release: docker-start-service-%: docker-start-%

# Docker helpers
.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release: docker-%:
	$(DOCKER_COMPOSE) run --rm service-allocation make $*

.PHONY: docker-clean-data
docker-clean-data:
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata
