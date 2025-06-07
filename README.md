# Allocation

## Описание
Учебный проект, реализующий идеи из книги "Паттерны разработки на Python: TDD, DDD и событийно-ориентированная архитектура" Гарри Персиваля и Боба Грегори.
Реализация на C++ с использованием Poco, PostgreSQL и паттернов проектирования


### Сборка проекта
```bash
conan install . -of build --build=missing
cmake --preset conan-default
cd build
make
```