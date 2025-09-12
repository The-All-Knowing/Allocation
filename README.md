# Allocation Service

## Содержание
- [Описание](#описание)
- [Требования](#требования)
- [Установка](#установка)
- [Сборка проекта](#сборка-проекта)
- [Запуск](#запуск)
- [Тестирование](#тестирование)

## Описание
Учебный проект, реализующий идеи из книги *"Паттерны разработки на Python: TDD, DDD и событийно-ориентированная архитектура"* Гарри Персиваля и Боба Грегори. Сервис управляет распределением продуктов и партиями товаров, используя асинхронные сообщения. Реализован на C++ с использованием библиотек Poco, PostgreSQL, Redis и паттернов проектирования, таких как DDD и CQRS.

### Требования
Для сборки и запуска проекта нужны:
- C++23
- CMake ≥ 3.12
- Conan 2.x
- PostgreSQL ≥ 12
- Redis
- Python ≥ 3.x (для e2e тестов)
- pycodestyle и clang-format (для проверки стиля кода)

## Установка
1. Склонируйте репозиторий:
   ```bash
   git clone https://github.com/The-All-Knowing/Allocation.git
   cd Allocation
   ```
2. Настройте переменные окружения (см. раздел [Переменные окружения](#переменные-окружения) в [Запуск](#запуск)).

## Сборка проекта
### Основные команды
- **`make test-release`** или **`make test-debug`**: Собирает и запускает юнит- и интеграционные тесты в режиме Release или Debug.
- **`make e2e-test`**: Выполняет end-to-end тесты с использованием `pytest` (требуется активированное Python virtualenv).
- **`make cmake-release`** или **`make cmake-debug`**: Генерирует конфигурацию CMake для сборки в режиме Release или Debug.
- **`make build-release`** или **`make build-debug`**: Собирает проект в режиме Release или Debug.
- **`make start-release`** или **`make start-debug`**: Собирает и запускает сервис в указанном режиме.
- **`make conan-release`** или **`make conan-debug`**: Загружает и собирает зависимости через Conan.
- **`make docker-<command>`**: Выполняет указанную команду `<command>` в Docker-контейнере (например, `make docker-build-release`).

> **Примечание**: Полный список команд доступен в `Makefile`. Если команда включает `/debug`, используйте её для отладочной сборки (например, `make test-release/debug` подразумевает выбор между `test-release` и `test-debug`).

## Запуск
Для запуска сервиса с использованием Docker выполните:
```bash
docker-compose up
```
Это поднимет контейнеры для сервиса, PostgreSQL и Redis.

Альтернативно, для запуска в Docker через Make:
```bash
make docker-start-service-release
```

Для запуска без Docker:
```bash
make start-release
```

### Переменные окружения
Перед запуском настройте следующие переменные окружения (например, в файле `.env` или напрямую в командной строке). Они используются для подключения к базам данных и могут быть переопределены в конфигурационном файле.

- `POSTGRES_HOST`: Хост PostgreSQL (по умолчанию `localhost`).
- `POSTGRES_PORT`: Порт PostgreSQL (по умолчанию `5432`).
- `POSTGRES_DB`: Название БД (по умолчанию `allocation`).
- `POSTGRES_USER`: Название пользователя (по умолчанию `user`).
- `POSTGRES_PASSWORD`: Пароль подключения к БД (по умолчанию `password`).
- `REDIS_HOST`: Хост Redis (по умолчанию `localhost`).
- `REDIS_PORT`: Порт Redis (по умолчанию `6379`).

> **Примечание**: Полный список переменных окружения доступен в файле `.env`. Параметры также можно передавать через ключ `--config`, например:  
> ```bash
> allocation --config ./configs/Allocation.ini
> ```

Пример:
```bash
export POSTGRES_HOST=localhost
export POSTGRES_PORT=5432
export POSTGRES_DB=allocation
export POSTGRES_USER=user
export POSTGRES_PASSWORD=password
export REDIS_HOST=localhost
export REDIS_PORT=6379
make start-release
```

## Тестирование
- **Юнит- и интеграционные тесты**:
  ```bash
  make test-release
  ```
  Для отладки используйте `make test-debug`.
- **End-to-end тесты**:
  ```bash
  make e2e-test
  ```