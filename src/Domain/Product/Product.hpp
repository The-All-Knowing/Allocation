#pragma once

#include "Precompile.hpp"

#include "Batch.hpp"
#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Агрегат продукта, содержащий партии и реализующий бизнес-логику распределения.
    class Product
    {
    public:
        /// @brief Создаёт продукт.
        /// @param SKU Артикул продукта.
        /// @param batches Список партий продукта.
        /// @param versionNumber Номер версии продукта.
        /// @param isNew Новый агрегат или уже существовал.
        explicit Product(const std::string& SKU, const std::vector<Batch>& batches = {},
            size_t versionNumber = 0, bool isNew = true);

        /// @brief Проверяет, был ли продукт изменён.
        /// @return true — если продукт изменён, иначе false.
        [[nodiscard]] bool IsModified() const noexcept;

        /// @brief Добавляет партию к продукту.
        /// @param batch Добавляемая партия.
        /// @return true — если партия добавлена, иначе false.
        bool AddBatch(const Batch& batch) noexcept;

        /// @brief Добавляет несколько партий к продукту.
        /// @param batches Список партий для добавления.
        /// @return true — если партии добавлены, иначе false.
        /// @note Либо добавляет все, либо не добавляет ни одной.
        bool AddBatches(const std::vector<Batch>& batches) noexcept;

        /// @brief Аллоцирует строку заказа на партию продукта.
        /// @param line Строка заказа.
        /// @return Ссылка на партию, если аллокация успешна, иначе std::nullopt.
        std::optional<std::string> Allocate(const OrderLine& line);

        /// @brief Изменяет количество в партии.
        /// @param ref Ссылка на партию.
        /// @param qty Новое количество.
        void ChangeBatchQuantity(const std::string& ref, size_t qty);

        /// @brief Возвращает все партии продукта.
        /// @return Список партий.
        [[nodiscard]] std::vector<Batch> GetBatches() const noexcept;

        /// @brief Возвращает партию продукта по её ссылке.
        /// @param reference Ссылка на партию.
        /// @return Партия, если найдена, иначе std::nullopt.
        [[nodiscard]] std::optional<Batch> GetBatch(const std::string& reference) const noexcept;

        /// @brief Возвращает ссылки изменённых партий.
        /// @return Список ссылок изменённых партий.
        [[nodiscard]] std::vector<std::string> GetModifiedBatches() const noexcept;

        /// @brief Возвращает номер версии продукта.
        /// @return Номер версии.
        [[nodiscard]] size_t GetVersion() const noexcept;

        /// @brief Возвращает артикул продукта.
        /// @return Артикул.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Возвращает сообщения, сгенерированные продуктом.
        /// @return Список сообщений.
        [[nodiscard]] const std::vector<Domain::IMessagePtr>& Messages() const noexcept;

        /// @brief Очищает сообщения продукта.
        void ClearMessages() noexcept;

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        std::unordered_set<std::string> _modifiedBatchRefs;
        std::vector<Domain::IMessagePtr> _messages;
        size_t _versionNumber;
        bool _isModify{false};
    };

    using ProductPtr = std::shared_ptr<Product>;

    /// @brief Проверяет равенство двух продуктов.
    /// @param lhs Левый операнд.
    /// @param rhs Правый операнд.
    /// @return true — если продукты равны, иначе false.
    bool operator==(const Product& lhs, const Product& rhs) noexcept;

    /// @brief Проверяет равенство двух умных указателей на продукт.
    /// @param lhs Левый указатель.
    /// @param rhs Правый указатель.
    /// @return true — если оба указателя nullptr или продукты равны, иначе false.
    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept;
}
