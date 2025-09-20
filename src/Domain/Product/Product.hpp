#pragma once

#include "Batch.hpp"
#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Агрегат-продукт, содержит партии заказа с общим артикулом продукции. 
    /// Реализует бизнес-логику распределения позиций заказа в партиях заказа.
    class Product
    {
    public:
        /// @brief Создаёт агрегат-продукт.
        /// @param sku Артикул продукции.
        /// @param batches Список партий заказа продукции.
        /// @param versionNumber Номер версии агрегата.
        /// @param isNew Новый агрегат или уже существовал.
        explicit Product(const std::string& sku, const std::vector<Batch>& batches = {},
            size_t versionNumber = 0, bool isNew = true);

        /// @brief Устанавливает флаг изменённости.
        /// @param modified Новый флаг изменённости.
        void SetModified(bool modified) noexcept;

        /// @brief Проверяет, был ли агрегат изменён.
        /// @return true — если агрегат изменён, иначе false.
        [[nodiscard]] bool IsModified() const noexcept;

        /// @brief Добавляет партию заказа к агрегату.
        /// @param batch Добавляемая партия заказа.
        /// @throw std::invalid_argument Выбрасывается, если артикул партии не совпадает с артикулом
        /// агрегата.
        /// @return true — если партия добавлена, иначе false.
        bool AddBatch(const Batch& batch);

        /// @brief Добавляет несколько партий заказа к агрегату.
        /// @param batches Добавляемые партии заказа.
        /// @throw std::invalid_argument Выбрасывается, если артикул партии не совпадает с артикулом
        /// агрегата.
        /// @return true — если партии добавлены, иначе false.
        /// @note Либо добавляет все партии, либо не добавляет ни одной.
        bool AddBatches(const std::vector<Batch>& batches);

        /// @brief Распределяет позицию заказа в партии заказа агрегата.
        /// @param line Позиция заказа для распределения.
        /// @throw std::invalid_argument Выбрасывается, если артикул позиции заказа не совпадает с
        /// артикулом продукции агрегата.
        /// @return Ссылка на партию заказа, если распределение успешно, иначе std::nullopt.
        std::optional<std::string> Allocate(const OrderLine& line);

        /// @brief Изменяет количество продукции в партии заказа.
        /// @param ref Ссылка на партию заказа.
        /// @param qty Новое количество продукции.
        /// @return true - если партия найдена и количество изменено, иначе false.
        bool ChangeBatchQuantity(const std::string& ref, size_t qty);

        /// @brief Возвращает все партии заказа агрегата.
        /// @return Партии заказа агрегата.
        [[nodiscard]] std::vector<Batch> GetBatches() const noexcept;

        /// @brief Возвращает партию заказа по её ссылке.
        /// @param reference Ссылка на партию заказа.
        /// @return Партия заказа, если найдена, иначе std::nullopt.
        [[nodiscard]] std::optional<Batch> GetBatch(const std::string& reference) const noexcept;

        /// @brief Возвращает ссылки изменённых партий заказа.
        /// @return Ссылки изменённых партий заказа.
        [[nodiscard]] std::vector<std::string> GetModifiedBatches() const noexcept;

        /// @brief Возвращает номер версии агрегата.
        /// @return Номер версии.
        [[nodiscard]] size_t GetVersion() const noexcept;

        /// @brief Возвращает артикул продукции агрегата.
        /// @return Артикул продукции.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Возвращает сообщения, сгенерированные во время выполнения бизнес-логики.
        /// @return Доменные сообщения.
        [[nodiscard]] const std::vector<Domain::IMessagePtr>& Messages() const noexcept;

        /// @brief Очищает сообщения агрегата.
        void ClearMessages() noexcept;

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        std::unordered_set<std::string> _modifiedBatchRefs;
        std::vector<Domain::IMessagePtr> _messages;
        size_t _versionNumber;
        bool _isModified;
    };

    using ProductPtr = std::shared_ptr<Product>;

    /// @brief Проверяет равенство двух агрегатов.
    /// @param lhs Левый операнд.
    /// @param rhs Правый операнд.
    /// @return true — если агрегаты равны, иначе false.
    bool operator==(const Product& lhs, const Product& rhs) noexcept;

    /// @brief Проверяет равенство двух агрегатов.
    /// @param lhs Левый операнд.
    /// @param rhs Правый операнд.
    /// @return true — если оба указателя nullptr или агрегаты равны, иначе false.
    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept;
}
