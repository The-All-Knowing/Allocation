# ==============================================================================
# КОНФИГУРАЦИЯ
# ==============================================================================
NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
DOCKER_COMPOSE ?= docker-compose
INSTALL_BIN ?= $(PREFIX)/bin

# NOTE: use Makefile.local to override the options defined above.
-include Makefile.local

# ==============================================================================
# ОБЩИЕ ЦЕЛИ
# ==============================================================================
.PHONY: all
all: test-debug test-release

# ==============================================================================
# PYTHON ОКРУЖЕНИЕ И E2E ТЕСТЫ
# ==============================================================================
# Убедимся, что ~/.local/bin в PATH для pipx, если он установлен глобально
# В CI это лучше делать в шаге GitHub Actions
# export PATH := $(HOME)/.local/bin:$(PATH)

.PHONY: pip-install
pip-install: venv/touchfile

venv/touchfile: tests/requirements.txt
	@echo "Creating Python virtual environment and installing dependencies..."
	python3 -m venv venv
	# Активируем venv для установки зависимостей
	. ./venv/bin/activate && pip install -r ./tests/requirements.txt
	# Если conan и autopep8 нужны в этом venv, установим их здесь
	# . ./venv/bin/activate && pipx install conan && pipx install autopep8
	touch venv/touchfile

.PHONY: e2e-test
e2e-test: pip-install
	@echo "Running E2E tests..."
	# Активируем venv для запуска pytest
	. ./venv/bin/activate && PYTHONPATH=. pytest -v --color=yes

# ==============================================================================
# CONAN
# ==============================================================================
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
	@echo "Installing Conan dependencies for Debug build..."
	conan install . -s build_type=Debug --build=missing

.PHONY: conan-release
conan-release: conan-profile
	@echo "Installing Conan dependencies for Release build..."
	conan install . -s build_type=Release --build=missing

# ==============================================================================
# CMAKE
# ==============================================================================
.PHONY: cmake-debug
cmake-debug: conan-debug
	@echo "Configuring CMake for Debug build..."
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -B build/Debug -S .

.PHONY: cmake-release
cmake-release: conan-release
	@echo "Configuring CMake for Release build..."
	cmake -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -B build/Release -S .

build/Debug/CMakeCache.txt:
	$(MAKE) cmake-debug

build/Release/CMakeCache.txt:
	$(MAKE) cmake-release

# ==============================================================================
# СБОРКА
# ==============================================================================
.PHONY: build-debug
build-debug: build/Debug/CMakeCache.txt
	@echo "Building Debug target 'allocation'..."
	cmake --build build/Debug -j $(NPROCS) --target allocation

.PHONY: build-release
build-release: build/Release/CMakeCache.txt
	@echo "Building Release target 'allocation'..."
	cmake --build build/Release -j $(NPROCS) --target allocation

# ==============================================================================
# ТЕСТИРОВАНИЕ
# ==============================================================================
.PHONY: test-debug
test-debug: build-debug
	@echo "Building and running Unit Tests for Debug build..."
	cmake --build build/Debug -j $(NPROCS) --target allocation_test
	cd build/Debug && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

.PHONY: test-release
test-release: build-release
	@echo "Building and running Unit Tests for Release build..."
	cmake --build build/Release -j $(NPROCS) --target allocation_test
	cd build/Release && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

# ==============================================================================
# ЗАПУСК СЕРВИСА (ЛОКАЛЬНО)
# ==============================================================================
.PHONY: start-debug
start-debug: build-debug
	@echo "Launching service in debugging mode..."
	cmake --build build/Debug --target start-debug

.PHONY: start-release
start-release: build-release
	@echo "Launching service in release mode..."
	cmake --build build/Release --target start-release

.PHONY: service-start-debug service-start-release
service-start-debug service-start-release: service-start-%: start-%

# Более надежный запуск сервиса для локальных E2E тестов
.PHONY: run-service-for-e2e
run-service-for-e2e: build-release
	@echo "Starting service in background for E2E tests..."
	build/Release/allocation &
	@echo "Waiting for service to be ready on http://localhost:8080/ping..."
	@until curl -f http://localhost:8080/ping; do \
		printf "."; \
		sleep 1; \
	done
	@echo "\nService is ready!"

.PHONY: e2e-test-local
e2e-test-local: run-service-for-e2e e2e-test
	@echo "Stopping service after E2E tests..."
	killall allocation || true # Убиваем процесс, если он запущен

# ==============================================================================
# ОЧИСТКА
# ==============================================================================
.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	@echo "Cleaning build directory build/$*..."
	cmake --build build/$* --target clean

.PHONY: dist-clean
dist-clean:
	@echo "Performing a full clean (build artifacts, caches, venv)..."
	rm -rf build/*
	rm -rf tests/__pycache__/
	rm -rf tests/.pytest_cache/
	rm -rf venv
	rm -rf *.egg-info # Если есть Python пакеты

# ==============================================================================
# УСТАНОВКА
# ==============================================================================
.PHONY: install-debug
install-debug: build-debug
	@echo "Installing Debug build..."
	cmake --install build/Debug -v --component allocation

.PHONY: install-release
install-release: build-release
	@echo "Installing Release build..."
	cmake --install build/Release -v --component allocation

# ==============================================================================
# ФОРМАТИРОВАНИЕ КОДА
# ==============================================================================
.PHONY: format
format:
	@echo "Formatting C++ code with clang-format..."
	find src -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
	@echo "Formatting Python code with autopep8..."
	find tests -name '*.py' -type f | xargs autopep8 -i

# ==============================================================================
# ОКРУЖЕНИЕ (для Docker Compose)
# ==============================================================================
# Эти переменные окружения будут использоваться docker-compose.yml
# и передаваться в контейнеры.
export DB_CONNECTION := postgresql://${POSTGRES_USER}:${POSTGRES_PASSWORD}@service-postgres:5432/${POSTGRES_DB}
export API_URL := http://localhost:8080 # Для локального curl, если проброшен порт
export REDIS_HOST := redis
export REDIS_PORT := 6379

# ==============================================================================
# ВНУТРЕННИЕ СКРЫТЫЕ ЦЕЛИ (ДЛЯ DOCKER)
# ==============================================================================
# Эти цели предназначены для вызова ИЗНУТРИ Docker-контейнера
--in-docker-start-debug --in-docker-start-release: --in-docker-start-%: install-%
	@echo "Starting allocation service inside Docker..."
	$(INSTALL_BIN)/allocation

# ==============================================================================
# ЗАПУСК В DOCKER (С ИСПОЛЬЗОВАНИЕМ DOCKER-COMPOSE)
# ==============================================================================
.PHONY: docker-start-debug docker-start-release
docker-start-debug docker-start-release: docker-start-%:
	@echo "Running service-allocation via docker-compose (foreground)..."
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm service-allocation make -- --in-docker-start-$*

.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-debug docker-start-service-release: docker-start-service-%: docker-start-%

# Фоновые сервисы внутри Docker (для CI или локальной отладки)
.PHONY: docker-start-service-debug-bg docker-start-service-release-bg
docker-start-service-debug-bg:
	@echo "Starting service-allocation via docker-compose (background, debug)..."
	$(DOCKER_COMPOSE) run -d -p 8080:8080 --name service-allocation-debug \
		service-allocation bash -c '$(INSTALL_BIN)/allocation; tail -f /dev/null'

docker-start-service-release-bg:
	@echo "Starting service-allocation via docker-compose (background, release)..."
	$(DOCKER_COMPOSE) run -d -p 8080:8080 --name service-allocation-release \
		service-allocation bash -c '$(INSTALL_BIN)/allocation; tail -f /dev/null'

# ==============================================================================
# DOCKER-ПОМОЩНИКИ (ДЛЯ ЗАПУСКА MAKE-ЦЕЛЕЙ ВНУТРИ КОНТЕЙНЕРА)
# ==============================================================================
.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug \
        docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release \
        docker-conan-debug docker-conan-release docker-format docker-e2e-test
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug \
docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release \
docker-conan-debug docker-conan-release docker-format docker-e2e-test: docker-%:
	@echo "Executing 'make $*' inside service-allocation container..."
	$(DOCKER_COMPOSE) run --rm service-allocation make $*

.PHONY: docker-clean-data
docker-clean-data:
	@echo "Cleaning Docker Compose volumes and data..."
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata