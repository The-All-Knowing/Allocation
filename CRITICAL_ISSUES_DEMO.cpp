/*
 * Демонстрация критических проблем найденных в коде
 * Этот файл НЕ ДОЛЖЕН компилироваться с основным проектом!
 * Это только демонстрация проблем для ревью.
 */

#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

// Имитация проблематичного кода из AbstractUnitOfWork

class MockProduct {
public:
    bool IsModified() const { return modified_; }
    void SetModified(bool mod) { modified_ = mod; }
private:
    bool modified_ = false;
};

class MockTrackingRepository {
public:
    void Add(std::shared_ptr<MockProduct> product) {
        // Симуляция долгой операции
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::vector<std::pair<std::shared_ptr<MockProduct>, int>> GetSeen() {
        return seen_products_;
    }
    
    void AddProduct(std::shared_ptr<MockProduct> product) {
        seen_products_.push_back({product, 1});
    }
    
private:
    std::vector<std::pair<std::shared_ptr<MockProduct>, int>> seen_products_;
};

// Проблематичная реализация без thread safety
class ProblematicUnitOfWork {
public:
    ProblematicUnitOfWork() : tracking_(), is_committed_(false) {
        // Добавляем тестовые продукты
        for(int i = 0; i < 5; ++i) {
            auto product = std::make_shared<MockProduct>();
            product->SetModified(true);
            tracking_.AddProduct(product);
        }
    }
    
    // ПРОБЛЕМА: Race condition - два потока могут одновременно изменять is_committed_
    void Commit() {
        std::cout << "Thread " << std::this_thread::get_id() << " starting commit\n";
        
        // ПРОБЛЕМА: GetSeen() может вернуть разные результаты в разных потоках
        for (const auto& [product, _] : tracking_.GetSeen()) {
            if (product->IsModified()) {
                // ПРОБЛЕМА: Одновременный доступ к tracking_ без синхронизации
                tracking_.Add(product);
            }
        }
        
        // ПРОБЛЕМА: Race condition при записи
        is_committed_ = true;
        
        std::cout << "Thread " << std::this_thread::get_id() << " finished commit\n";
    }
    
    bool IsCommitted() const { return is_committed_; }
    
private:
    MockTrackingRepository tracking_;
    bool is_committed_;  // ПРОБЛЕМА: нет атомарного доступа
};

// Демонстрация race condition
void demonstrateRaceCondition() {
    std::cout << "=== Демонстрация Race Condition ===\n";
    
    ProblematicUnitOfWork uow;
    
    std::vector<std::thread> threads;
    
    // Запускаем несколько потоков одновременно
    for(int i = 0; i < 3; ++i) {
        threads.emplace_back([&uow]() {
            uow.Commit();
        });
    }
    
    // Ждем завершения всех потоков
    for(auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final committed state: " << uow.IsCommitted() << "\n";
    std::cout << "=== Конец демонстрации ===\n\n";
}

// Демонстрация проблемы с exception safety
class ProblematicSession {
public:
    void commit() {
        if(should_fail_) {
            throw std::runtime_error("Database commit failed!");
        }
        std::cout << "Session committed successfully\n";
    }
    
    void rollback() {
        std::cout << "Session rolled back\n";
    }
    
    void begin() {
        std::cout << "New transaction started\n";
    }
    
    void setShouldFail(bool fail) { should_fail_ = fail; }
    
private:
    bool should_fail_ = false;
};

class ProblematicSqlUnitOfWork {
public:
    ProblematicSqlUnitOfWork() : session_() {}
    
    // ПРОБЛЕМА: Отсутствие exception safety
    void Commit() {
        std::cout << "Starting SQL commit...\n";
        
        // Если AbstractUnitOfWork::Commit() выбросит исключение,
        // база данных останется в inconsistent state
        
        // AbstractUnitOfWork::Commit(); // может выбросить исключение
        
        session_.commit();  // ПРОБЛЕМА: если это упадет, состояние неопределенно
        session_.begin();   // ПРОБЛЕМА: если это упадет, состояние неопределенно
        
        std::cout << "SQL commit completed\n";
    }
    
    void setShouldFail(bool fail) { session_.setShouldFail(fail); }
    
private:
    ProblematicSession session_;
};

void demonstrateExceptionSafety() {
    std::cout << "=== Демонстрация проблем с Exception Safety ===\n";
    
    ProblematicSqlUnitOfWork sql_uow;
    sql_uow.setShouldFail(true);
    
    try {
        sql_uow.Commit();
    } catch(const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
        std::cout << "ПРОБЛЕМА: База данных может остаться в inconsistent state!\n";
    }
    
    std::cout << "=== Конец демонстрации ===\n\n";
}

int main() {
    std::cout << "ДЕМОНСТРАЦИЯ КРИТИЧЕСКИХ ПРОБЛЕМ В КОДЕ ALLOCATION SERVICE\n";
    std::cout << "=========================================================\n\n";
    
    demonstrateRaceCondition();
    demonstrateExceptionSafety();
    
    std::cout << "ЗАКЛЮЧЕНИЕ:\n";
    std::cout << "1. Race conditions могут привести к непредсказуемому поведению\n";
    std::cout << "2. Отсутствие exception safety может привести к data corruption\n";
    std::cout << "3. Необходимо добавить мьютексы и proper exception handling\n";
    
    return 0;
}