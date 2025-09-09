# Allocation-service

## Описание
Учебный проект, реализующий идеи из книги "Паттерны разработки на Python: TDD, DDD и событийно-ориентированная архитектура" Гарри Персиваля и Боба Грегори.
Реализация на C++ с использованием Poco, PostgreSQL, Redis и паттернов проектирования

## Сборка и запуск проектом
### Основные команды
- **`make test-release/debug`** - собрать и запустить юнит-тесты (Release/Debug).
- **`make e2e-test`** - выполнить end-to-end тесты через `pytest` в venv.
- **`make cmake-release/debug`** - сгенерировать конфигурацию CMake (Release/Debug).
- **`make build-release/debug`** - собрать проект в Release/Debug.
- **`make start-release/debug`** - собрать и запустить сервис в Release/Debug.
- **`make docker-start-release/debug`** - собрать и запустить сервис в внутри контейнера.
- **`make docker-$`** - запустить команду $ в докере.

### Требования для сборки
- C++23
- CMake ≥ 3.12
- Conan 2.x
- PostgreSQL
- Redis
- Python