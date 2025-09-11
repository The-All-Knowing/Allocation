# ==============================================================================
# Конфигурация
# ==============================================================================
NPROCS           ?= $(shell nproc)
CLANG_FORMAT     ?= clang-format
DOCKER_COMPOSE   ?= docker-compose

# NOTE: переопределения кладём в Makefile.local
-include Makefile.local

# ==============================================================================
# Общие цели
# ==============================================================================
.PHONY: all
all: test-debug test-release

# ==============================================================================
# Python окружение и E2E тесты
# ==============================================================================
.PHONY: pip-install
pip-install: venv/touchfile

venv/touchfile: tests/requirements.txt
	@echo ">>> Creating Python virtual environment and installing dependencies..."
	python3 -m venv venv
	. ./venv/bin/activate && pip install -r ./tests/requirements.txt
	touch $@

.PHONY: e2e-test
e2e-test: pip-install
	@echo ">>> Running E2E tests..."
	. ./venv/bin/activate && PYTHONPATH=. pytest -v --color=yes

# ==============================================================================
# Conan
# ==============================================================================
.PHONY: conan-profile
conan-profile:
	@if [ ! -f "$$HOME/.conan2/profiles/default" ]; then \
		echo ">>> Conan default profile not found. Creating..."; \
		conan profile detect --force; \
	else \
		echo ">>> Conan default profile exists."; \
	fi

.PHONY: conan-debug conan-release
conan-debug:   conan-profile
	conan install . -s build_type=Debug --build=missing
conan-release: conan-profile
	conan install . -s build_type=Release --build=missing

# ==============================================================================
# CMake конфигурация
# ==============================================================================
.PHONY: cmake-debug cmake-release
cmake-debug:   conan-debug
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake \
	      -DCMAKE_BUILD_TYPE=Debug -B build/Debug -S .
cmake-release: conan-release
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake \
	      -DCMAKE_BUILD_TYPE=Release -B build/Release -S .

build/Debug/CMakeCache.txt:   cmake-debug
build/Release/CMakeCache.txt: cmake-release

# ==============================================================================
# Сборка
# ==============================================================================
.PHONY: build-debug build-release
build-debug:   build/Debug/CMakeCache.txt
	cmake --build build/Debug   -j $(NPROCS) --target allocation
build-release: build/Release/CMakeCache.txt
	cmake --build build/Release -j $(NPROCS) --target allocation

# ==============================================================================
# Тесты
# ==============================================================================
.PHONY: test-debug test-release
test-debug: build-debug
	cmake --build build/Debug -j $(NPROCS) --target allocation_test
	cd build/Debug && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

test-release: build-release
	cmake --build build/Release -j $(NPROCS) --target allocation_test
	cd build/Release && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

# ==============================================================================
# Запуск сервиса
# ==============================================================================
.PHONY: start-debug start-release
start-debug:   build-debug
	cmake --build build/Debug   --target start-debug
start-release: build-release
	cmake --build build/Release --target start-release

.PHONY: service-start-debug service-start-release
service-start-%: start-%

# ==============================================================================
# Очистка
# ==============================================================================
.PHONY: clean-debug clean-release
clean-%:
	cmake --build build/$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build/*
	rm -rf tests/__pycache__/ tests/.pytest_cache/

# ==============================================================================
# Установка
# ==============================================================================
.PHONY: install-debug install-release
install-debug:   build-debug
	cmake --install build/Debug   -v --component allocation
install-release: build-release
	cmake --install build/Release -v --component allocation

# ==============================================================================
# Форматирование
# ==============================================================================
.PHONY: format
format:
	find src   -name '*pp'   -type f | xargs $(CLANG_FORMAT) -i
	find tests -name '*.py'  -type f | xargs autopep8 -i

# ==============================================================================
# Docker окружение
# ==============================================================================
export DB_CONNECTION := postgresql://${POSTGRES_USER}:${POSTGRES_PASSWORD}@${POSTGRES_HOST}:${POSTGRES_PORT}/${POSTGRES_DB}

--in-docker-start-%: install-%
	psql ${DB_CONNECTION} -f ./postgresql/schemas/db-1.sql
	/home/user/.local/bin/allocation

.PHONY: docker-start-debug docker-start-release
docker-start-%:
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm service-allocation make -- --in-docker-start-$*

.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-%: docker-start-%

.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug \
        docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release
docker-%:
	$(DOCKER_COMPOSE) run --rm service-allocation make $*

.PHONY: docker-clean-data
docker-clean-data:
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata
