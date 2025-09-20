# Предлагаемые исправления критических проблем

## 1. Исправление Race Condition в AbstractUnitOfWork

### Текущая проблема:
```cpp
void AbstractUnitOfWork::Commit()
{
    for (const auto& [product, _] : _tracking.GetSeen()) // Race condition
        if (product->IsModified())
            _tracking.Add(product);
    _isCommited = true; // Race condition
}
```

### Предлагаемое решение:

```cpp
// В AbstractUnitOfWork.hpp
class AbstractUnitOfWork : public Domain::IUnitOfWork
{
private:
    mutable std::mutex _mutex;
    Adapters::Repository::TrackingRepository _tracking;
    std::atomic<bool> _isCommited{false};

public:
    void Commit() override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        
        if (_isCommited.load()) {
            return; // Уже зафиксированы изменения
        }
        
        auto seen_products = _tracking.GetSeen();
        for (const auto& [product, _] : seen_products) {
            if (product && product->IsModified()) {
                _tracking.Add(product);
            }
        }
        
        _isCommited.store(true);
    }
    
    void RollBack() override 
    { 
        std::lock_guard<std::mutex> lock(_mutex);
        _isCommited.store(false);
    }
    
    bool IsCommited() const noexcept override 
    { 
        return _isCommited.load(); 
    }
    
    std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        
        std::vector<Domain::IMessagePtr> newMessages;
        auto seen_products = _tracking.GetSeen();
        
        for (const auto& [product, _] : seen_products) {
            if (!product) continue;
            
            auto messages = product->Messages();
            newMessages.insert(newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }
        
        return newMessages;
    }
};
```

## 2. Исправление Exception Safety в SqlUnitOfWork

### Текущая проблема:
```cpp
void Commit() override
{
    AbstractUnitOfWork::Commit(); // Может выбросить исключение
    _session.commit();             // База данных может быть в inconsistent state
    _session.begin();              // Если это упадет, состояние неопределенно
}
```

### Предлагаемое решение:

```cpp
// В SqlUnitOfWork.hpp
class SqlUnitOfWork final : public AbstractUnitOfWork
{
public:
    void Commit() override
    {
        if (IsCommited()) {
            return; // Уже зафиксированы
        }
        
        try {
            // Сначала подготавливаем изменения в памяти
            AbstractUnitOfWork::Commit();
            
            // Затем фиксируем в базе данных
            _session.commit();
            
        } catch (const std::exception& e) {
            // В случае ошибки откатываем изменения
            try {
                _session.rollback();
                AbstractUnitOfWork::RollBack();
            } catch (...) {
                // Логируем, но не пробрасываем исключение из catch блока
                // TODO: Add logging
            }
            throw; // Пробрасываем исходное исключение
        }
        
        // Начинаем новую транзакцию только после успешного commit
        try {
            _session.begin();
        } catch (const std::exception& e) {
            // Критическая ошибка - не можем начать новую транзакцию
            // TODO: Add critical logging and possibly terminate
            throw std::runtime_error("Failed to begin new transaction after commit");
        }
    }
    
    void RollBack() override
    {
        try {
            _session.rollback();
            AbstractUnitOfWork::RollBack();
        } catch (const std::exception& e) {
            // Логируем ошибку, но не пробрасываем
            // TODO: Add logging
            AbstractUnitOfWork::RollBack(); // Откатываем состояние в памяти
        }
    }
    
    // Добавляем безопасный деструктор
    ~SqlUnitOfWork() noexcept
    {
        try {
            if (!IsCommited()) {
                _session.rollback();
            }
        } catch (...) {
            // В деструкторе не должны пробрасываться исключения
            // TODO: Add critical logging
        }
    }
};
```

## 3. Исправление нарушения абстракции в IUnitOfWork

### Текущая проблема:
```cpp
// Domain слой знает о Poco::Data::Session
[[nodiscard]] virtual Poco::Data::Session GetSession() noexcept = 0;
```

### Предлагаемое решение:

#### Вариант 1: Удалить метод из интерфейса Domain
```cpp
// В IUnitOfWork.hpp - удалить GetSession()
namespace Allocation::Domain
{
    class IUnitOfWork
    {
    public:
        virtual ~IUnitOfWork() = default;
        virtual void Commit() = 0;
        virtual void RollBack() = 0;
        [[nodiscard]] virtual bool IsCommited() const noexcept = 0;
        [[nodiscard]] virtual IRepository& GetProductRepository() = 0;
        [[nodiscard]] virtual std::vector<IMessagePtr> GetNewMessages() noexcept = 0;
        // GetSession() удален!
    };
}
```

#### Вариант 2: Создать отдельный интерфейс для доступа к сессии
```cpp
// Новый файл: ISqlUnitOfWork.hpp
namespace Allocation::ServiceLayer::UoW
{
    class ISqlUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        [[nodiscard]] virtual const Poco::Data::Session& GetSession() const noexcept = 0;
    };
}

// В SqlUnitOfWork.hpp
class SqlUnitOfWork final : public ISqlUnitOfWork
{
    [[nodiscard]] const Poco::Data::Session& GetSession() const noexcept override
    {
        return _session;
    }
};
```

## 4. Исправление несоответствия типов для версий

### Текущая проблема:
```cpp
// Product.hpp - использует size_t
[[nodiscard]] size_t GetVersion() const noexcept;

// IUpdatableRepository.hpp - использует int
virtual void Update(ProductPtr product, int oldVersion) = 0;
```

### Предлагаемое решение:

```cpp
// В IUpdatableRepository.hpp
namespace Allocation::Domain
{
    class IUpdatableRepository : public IRepository
    {
    public:
        /// @brief Обновляет агрегат-продукт в репозитории.
        /// @param product Агрегат-продукт для обновления.
        /// @param oldVersion Изначальная версия агрегата, загруженная из репозитория.
        virtual void Update(ProductPtr product, size_t oldVersion) = 0;
    };
}

// В TrackingRepository.hpp
void Update(Domain::ProductPtr product, size_t oldVersion) override;

// В SqlRepository.hpp  
void Update(Domain::ProductPtr product, size_t oldVersion) override;

// В TrackingRepository.cpp
void TrackingRepository::Update(Domain::ProductPtr product, size_t oldVersion)
{
    if (!product) {
        throw std::invalid_argument("Product cannot be null");
    }
    
    // Теперь типы совместимы - оба size_t
    if (product->GetVersion() <= oldVersion) {
        throw std::logic_error(
            "Product version (" + std::to_string(product->GetVersion()) + 
            ") must be greater than old version (" + std::to_string(oldVersion) + ")"
        );
    }
    
    if (!product->IsModified()) {
        return;
    }
    
    try {
        _repo.Update(product, oldVersion);
        product->SetModified(false);
    } catch (const std::exception& e) {
        // TODO: Add logging
        throw;
    }
}
```

## 5. Добавление валидации в TrackingRepository

### Предлагаемое решение:
```cpp
void TrackingRepository::Update(Domain::ProductPtr product, int oldVersion)
{
    if (!product) {
        throw std::invalid_argument("Product cannot be null");
    }
    
    if (oldVersion < 0) {
        throw std::invalid_argument("Old version cannot be negative");
    }
    
    if (product->GetVersion() <= oldVersion) {
        throw std::logic_error(
            "Product version (" + std::to_string(product->GetVersion()) + 
            ") must be greater than old version (" + std::to_string(oldVersion) + ")"
        );
    }
    
    // Проверяем, что продукт действительно был изменен
    if (!product->IsModified()) {
        return; // Нет необходимости обновлять неизмененный продукт
    }
    
    try {
        _repo.Update(product, oldVersion);
        product->SetModified(false);
    } catch (const std::exception& e) {
        // Логируем ошибку и пробрасываем
        // TODO: Add logging
        throw;
    }
}
```

## 6. Улучшение CMake конфигурации

### Текущая проблема:
```cmake
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp)
```

### Предлагаемое решение:
```cmake
# Лучше явно перечислить источники или использовать подкаталоги
set(DOMAIN_SOURCES
    src/Domain/Product/Product.cpp
    src/Domain/Product/Batch.cpp
    # ... остальные файлы
)

set(SERVICE_LAYER_SOURCES
    src/ServiceLayer/UoW/AbstractUnitOfWork.cpp
    src/ServiceLayer/UoW/SqlUnitOfWork.cpp
    # ... остальные файлы
)

set(ADAPTER_SOURCES
    src/Adapters/Repository/SqlRepository.cpp
    src/Adapters/Repository/TrackingRepository.cpp
    # ... остальные файлы
)

set(ALL_SOURCES ${DOMAIN_SOURCES} ${SERVICE_LAYER_SOURCES} ${ADAPTER_SOURCES})
```

## Заключение

Эти исправления направлены на решение наиболее критических проблем:

1. **Thread Safety**: Добавление мьютексов и atomic переменных
2. **Exception Safety**: Proper RAII и exception handling
3. **Architecture**: Соблюдение принципов чистой архитектуры
4. **Type Safety**: Консистентное использование типов для версий
5. **Robustness**: Добавление валидации и проверок

Рекомендуется внедрять эти изменения постепенно, с тщательным тестированием каждого исправления.