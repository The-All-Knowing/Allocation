CMAKE_COMMON_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CMAKE_DEBUG_FLAGS ?= -DUSERVER_SANITIZE='addr ub'
CMAKE_RELEASE_FLAGS ?=
CMAKE_OS_FLAGS ?= -DUSERVER_FEATURE_CRYPTOPP_BLAKE2=0 -DUSERVER_FEATURE_REDIS_HI_MALLOC=1
NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
DOCKER_COMPOSE ?= docker-compose

ifeq ($(KERNEL),Darwin)
CMAKE_COMMON_FLAGS += -DUSERVER_NO_WERROR=1 -DUSERVER_CHECK_PACKAGE_VERSIONS=0 \
  -DUSERVER_DOWNLOAD_PACKAGE_CRYPTOPP=1 \
  -DOPENSSL_ROOT_DIR=$(shell brew --prefix openssl) \
  -DUSERVER_PG_INCLUDE_DIR=$(shell pg_config --includedir) \
  -DUSERVER_PG_LIBRARY_DIR=$(shell pg_config --libdir) \
  -DUSERVER_PG_SERVER_LIBRARY_DIR=$(shell pg_config --pkglibdir) \
  -DUSERVER_PG_SERVER_INCLUDE_DIR=$(shell pg_config --includedir-server)
endif

# NOTE: use Makefile.local to override the options defined above.
-include Makefile.local

CMAKE_DEBUG_FLAGS += -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS += -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS)

.PHONY: all
all: test-debug test-release

# start conan
.PHONY: conan-debug
conan-debug:
	conan install . -s build_type=Debug --build=missing

.PHONY: conan-release
conan-release:
	conan install . -s build_type=Release --build=missing

.PHONY: cmake-debug
cmake-debug: conan-debug
	cmake --preset conan-debug

.PHONY: cmake-release
cmake-release: conan-release
	cmake --preset conan-release

build/%/CMakeCache.txt:
	$(MAKE) cmake-$*

# Build using cmake
.PHONY: build-debug build-release
build-debug build-release: build-%: build/%/CMakeCache.txt
	cmake --build build/$* -j $(NPROCS) --target allocation

# Test
.PHONY: test-debug test-release
test-debug test-release: test-%: build-%
	cmake --build build/$* -j $(NPROCS) --target allocation_unittest
	cd build/$* && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)
# pycodestyle tests пока оставим, как перенесу часть тестов тогда будет разговор

# Start the service (via testsuite service runner)
.PHONY: start-debug
start-debug: build-debug
	cmake --build build/Debug --target start-debug

.PHONY: start-release
start-release: build-release
	cmake --build build/Release --target start-release

.PHONY: service-start-debug service-start-release
service-start-debug service-start-release: service-start-%: start-%

# Cleanup data
.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	cmake --build build/$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build/*
#rm -rf tests/__pycache__/ пока тесты на С++
#rm -rf tests/.pytest_cache/

# Install
.PHONY: install-debug install-release
install-debug install-release: install-%: build-%
	cmake --install build/$* -v --component allocation

# Format the sources
.PHONY: format
format:
	find src -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
#find tests -name '*.py' -type f | xargs autopep8 -i пока нет кода на python

# Set environment for --in-docker-start
export DB_CONNECTION := postgresql://${POSTGRES_USER}:${POSTGRES_PASSWORD}@service-postgres:5432/${POSTGRES_DB}

# Internal hidden targets that are used only in docker environment
--in-docker-start-debug --in-docker-start-release: --in-docker-start-%: install-%
	psql ${DB_CONNECTION} -f ./postgresql/data/initial_data.sql
	/home/user/.local/bin/allocation \
		--config /home/user/.local/etc/allocation/static_config.yaml \
		--config_vars /home/user/.local/etc/allocation/config_vars.docker.yaml

# Build and run service in docker environment
.PHONY: docker-start-debug docker-start-release
docker-start-debug docker-start-release: docker-start-%:
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm allocation make -- --in-docker-start-$*

.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-debug docker-start-service-release: docker-start-service-%: docker-start-%

# Start targets makefile in docker environment
.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release: docker-%:
	$(DOCKER_COMPOSE) run --rm allocation make $*

# Stop docker container and remove PG data
.PHONY: docker-clean-data
docker-clean-data:
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata