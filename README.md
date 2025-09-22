# Allocation Service

## Описание
Учебный проект реализованный на C++, адаптирующий идеи из книги *"Паттерны разработки на Python: TDD, DDD и событийно-ориентированная архитектура"* Гарри Персиваля и Боба Грегори. 

Сервис предназначен для управления распределением позиций заказов в партиях поставок. Подробное описание проекта доступно в [Wiki проекта](https://github.com/The-All-Knowing/Allocation/wiki).

## Запуск
   ```bash
   git clone https://github.com/The-All-Knowing/Allocation.git
   cd Allocation
   docker-compose up
   ```

## Сборка проекта
### Основные команды 
- **`make conan-release(debug)`**: Загружает/собирает зависимости.
- **`make cmake-release(debug)`**: Генерирует конфигурацию для сборки.
- **`make build-release(debug)`**: Запускает сборку проекта.
- **`make start-release(debug)`**: Запускает сервис.
- **`make test-release(debug)`**: Запускает юнит- и интеграционные тесты.
- **`make e2e-test`**: Выполняет end-to-end тесты.
- **`make docker-<command>`**: Выполняет указанную команду `<command>` в Docker-контейнере (например, `make docker-build-release`).

> **Примечание**: Полный список команд доступен в `Makefile`.

### Зависимости
- C++23
- CMake ≥ 3.16
- Conan 2.x
- PostgreSQL
- Redis
- Python ≥ 3.x
- pycodestyle и clang-format

### Сборка
```bash
make build-release
```

### Переменные окружения
Перед запуском настройте переменные окружения перечисленные в файле `.env` или при запуске передайте конфиг файл.
 
```bash
allocation --config ./configs/Allocation.ini
```

## Тестирование
Требует инициализации переменных окружения из `.env`.

- **Юнит- и интеграционные тесты**
  ```bash
  make test-release
  ```

- **End-to-end тесты**
  ```bash
  make e2e-test
  ```
