/*
 * Тест для демонстрации и проверки исправления race condition
 * в AbstractUnitOfWork.
 * 
 * Этот файл демонстрирует как можно тестировать thread safety.
 */

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// Имитируем интерфейсы для тестирования
class MockProduct {
public:
    MockProduct() : modified_(false), version_(1) {}
    
    bool IsModified() const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return modified_; 
    }
    
    void SetModified(bool mod) { 
        std::lock_guard<std::mutex> lock(mutex_);
        modified_ = mod; 
    }
    
    int GetVersion() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return version_;
    }
    
    void IncrementVersion() {
        std::lock_guard<std::mutex> lock(mutex_);
        version_++;
        modified_ = true;
    }
    
private:
    mutable std::mutex mutex_;
    bool modified_;
    int version_;
};

class MockTrackingRepository {
public:
    void Add(std::shared_ptr<MockProduct> product) {
        std::lock_guard<std::mutex> lock(mutex_);
        add_count_.fetch_add(1);
        // Имитируем некоторую работу
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    std::vector<std::pair<std::shared_ptr<MockProduct>, int>> GetSeen() {
        std::lock_guard<std::mutex> lock(mutex_);
        return seen_products_;
    }
    
    void AddSeenProduct(std::shared_ptr<MockProduct> product) {
        std::lock_guard<std::mutex> lock(mutex_);
        seen_products_.push_back({product, product->GetVersion()});
    }
    
    int GetAddCount() const { return add_count_.load(); }
    
private:
    std::mutex mutex_;
    std::vector<std::pair<std::shared_ptr<MockProduct>, int>> seen_products_;
    std::atomic<int> add_count_{0};
};

// Небезопасная версия (демонстрирует проблему)
class UnsafeUnitOfWork {
public:
    UnsafeUnitOfWork() : is_committed_(false) {
        // Добавляем тестовые продукты
        for(int i = 0; i < 10; ++i) {
            auto product = std::make_shared<MockProduct>();
            product->SetModified(true);
            tracking_.AddSeenProduct(product);
        }
    }
    
    void Commit() {
        // НЕБЕЗОПАСНО: нет синхронизации!
        auto seen_products = tracking_.GetSeen();
        for (const auto& [product, _] : seen_products) {
            if (product->IsModified()) {
                tracking_.Add(product);
            }
        }
        is_committed_ = true; // Race condition!
    }
    
    bool IsCommitted() const { return is_committed_; }
    MockTrackingRepository& GetTracking() { return tracking_; }
    
private:
    MockTrackingRepository tracking_;
    bool is_committed_; // НЕ thread-safe!
};

// Безопасная версия (исправленная)
class SafeUnitOfWork {
public:
    SafeUnitOfWork() : is_committed_(false) {
        // Добавляем тестовые продукты
        for(int i = 0; i < 10; ++i) {
            auto product = std::make_shared<MockProduct>();
            product->SetModified(true);
            tracking_.AddSeenProduct(product);
        }
    }
    
    void Commit() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (is_committed_.load()) {
            return; // Уже зафиксировано
        }
        
        auto seen_products = tracking_.GetSeen();
        for (const auto& [product, _] : seen_products) {
            if (product && product->IsModified()) {
                tracking_.Add(product);
            }
        }
        is_committed_.store(true);
    }
    
    bool IsCommitted() const { return is_committed_.load(); }
    MockTrackingRepository& GetTracking() { return tracking_; }
    
private:
    mutable std::mutex mutex_;
    MockTrackingRepository tracking_;
    std::atomic<bool> is_committed_;
};

class ThreadSafetyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Настройка тестов
    }
    
    void TearDown() override {
        // Очистка после тестов  
    }
};

TEST_F(ThreadSafetyTest, UnsafeUnitOfWorkDemonstratesRaceCondition) {
    UnsafeUnitOfWork uow;
    
    const int num_threads = 5;
    const int iterations_per_thread = 10;
    
    std::vector<std::thread> threads;
    std::atomic<int> successful_commits{0};
    
    // Запускаем несколько потоков, каждый пытается сделать commit
    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&uow, &successful_commits, iterations_per_thread]() {
            for(int j = 0; j < iterations_per_thread; ++j) {
                try {
                    uow.Commit();
                    if(uow.IsCommitted()) {
                        successful_commits.fetch_add(1);
                    }
                } catch(...) {
                    // Игнорируем исключения для этого теста
                }
            }
        });
    }
    
    // Ждем завершения всех потоков
    for(auto& t : threads) {
        t.join();
    }
    
    // В unsafe версии результат может быть непредсказуемым
    std::cout << "Unsafe version - Add count: " << uow.GetTracking().GetAddCount() << std::endl;
    std::cout << "Unsafe version - Successful commits: " << successful_commits.load() << std::endl;
    
    // Не делаем строгих assertion'ов, поскольку результат непредсказуем
    // Это демонстрация проблемы, а не проверка корректности
}

TEST_F(ThreadSafetyTest, SafeUnitOfWorkHandlesConcurrentAccess) {
    SafeUnitOfWork uow;
    
    const int num_threads = 5;
    const int iterations_per_thread = 10;
    
    std::vector<std::thread> threads;
    std::atomic<int> total_attempts{0};
    
    // Запускаем несколько потоков, каждый пытается сделать commit
    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&uow, &total_attempts, iterations_per_thread]() {
            for(int j = 0; j < iterations_per_thread; ++j) {
                total_attempts.fetch_add(1);
                try {
                    uow.Commit();
                } catch(const std::exception& e) {
                    FAIL() << "Safe version should not throw: " << e.what();
                }
            }
        });
    }
    
    // Ждем завершения всех потоков
    for(auto& t : threads) {
        t.join();
    }
    
    // В safe версии должен быть предсказуемый результат
    EXPECT_TRUE(uow.IsCommitted());
    EXPECT_EQ(uow.GetTracking().GetAddCount(), 10); // Должно быть ровно 10 (по одному на каждый продукт)
    
    std::cout << "Safe version - Add count: " << uow.GetTracking().GetAddCount() << std::endl;
    std::cout << "Safe version - Total attempts: " << total_attempts.load() << std::endl;
}

TEST_F(ThreadSafetyTest, SafeUnitOfWorkIdempotentCommit) {
    SafeUnitOfWork uow;
    
    // Первый commit
    uow.Commit();
    EXPECT_TRUE(uow.IsCommitted());
    int first_add_count = uow.GetTracking().GetAddCount();
    
    // Повторные commit'ы не должны делать дополнительную работу
    uow.Commit();
    uow.Commit();
    
    EXPECT_TRUE(uow.IsCommitted());
    EXPECT_EQ(uow.GetTracking().GetAddCount(), first_add_count);
}

// Этот тест можно запустить с флагом --gtest_repeat=100 для проверки стабильности
TEST_F(ThreadSafetyTest, StressTestSafeUnitOfWork) {
    SafeUnitOfWork uow;
    
    const int num_threads = 10;
    std::vector<std::thread> threads;
    
    // Все потоки одновременно пытаются сделать commit
    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&uow]() {
            uow.Commit();
        });
    }
    
    // Ждем завершения всех потоков
    for(auto& t : threads) {
        t.join();
    }
    
    // Результат должен быть консистентным
    EXPECT_TRUE(uow.IsCommitted());
    EXPECT_EQ(uow.GetTracking().GetAddCount(), 10);
}

/*
 * Для запуска этих тестов:
 * 
 * g++ -std=c++23 -pthread -lgtest -lgtest_main ThreadSafetyTest.cpp -o thread_safety_test
 * ./thread_safety_test
 * 
 * Или для stress testing:
 * ./thread_safety_test --gtest_repeat=100 --gtest_filter="*StressTest*"
 */