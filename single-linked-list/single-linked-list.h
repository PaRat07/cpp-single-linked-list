#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <new>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
                : value(val)
                , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };
public:
    SingleLinkedList(){}

    SingleLinkedList(const SingleLinkedList<Type>& list) {
        iterator_fill(list.begin(), list.end());
    }

    SingleLinkedList(const std::initializer_list<Type> list) {
        iterator_fill(list.begin(), list.end());
    }

    ~SingleLinkedList() {
        Clear();
    }
    // Возвращает количество элементов в списке за время O(1)
    size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& new_elem) {
        Node* t = head_.next_node;
        try {
            t = new Node(new_elem, head_.next_node);
            ++size_;
        }
        catch(...) {
            throw;
        }
        head_.next_node = t;
    }

    void Clear() noexcept {
        while (head_.next_node) {
            Node* buffer = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            --size_;
            delete buffer;
        }
    }
    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
                :node_(node)
        {}

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
                :node_(other.node_)
        {}

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return rhs.node_ == node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return rhs.node_ == node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] BasicIterator operator++(int) noexcept {
            Iterator buffer(node_);
            ++*this;
            return buffer;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] bool operator>(const SingleLinkedList<Type>& rhs) const noexcept {
        return !lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
    }

    [[nodiscard]] bool operator<(const SingleLinkedList<Type>& rhs) const noexcept {
        return lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
    }

    [[nodiscard]] bool operator==(const SingleLinkedList<Type>& rhs) const noexcept {
        return std::equal(begin(), end(), rhs.begin());
    }

    [[nodiscard]] bool operator!=(const SingleLinkedList<Type>& rhs) const noexcept {
        return !(*this == rhs);
    }

    [[nodiscard]] bool operator>=(const SingleLinkedList<Type>& rhs) const noexcept {
        return *this == rhs or *this > rhs;
    }

    [[nodiscard]] bool operator<=(const SingleLinkedList<Type>& rhs) const noexcept {
        return *this == rhs or *this < rhs;
    }

    void operator=(const SingleLinkedList<Type>& rhs) {
        if (head_.next_node != rhs.head_.next_node) {
            SingleLinkedList<Type> x(rhs);
            swap(x);
        }
    }
    void swap(SingleLinkedList<Type>& rhs) noexcept {
        if (head_.next_node != rhs.head_.next_node) {
            SingleLinkedList<Type> x;
            std::swap(x.head_.next_node, rhs.head_.next_node);
            std::swap(x.size_, rhs.size_);
            std::swap(head_.next_node, x.head_.next_node);
            std::swap(size_, x.size_);
            std::swap(x.head_.next_node, rhs.head_.next_node);
            std::swap(x.size_, rhs.size_);
        }
    }

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return Iterator(head_.next_node);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        return Iterator(nullptr);
    }
// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return Iterator(const_cast<Node*>(&head_));
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(&head_);
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(pos.node_->next_node);
    }

    void PopFront() noexcept {
        EraseAfter(ConstIterator(&head_));
        --size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* after_deleted = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = after_deleted;
        --size_;
        return Iterator(after_deleted);
    }
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;
private:
    template<typename It>
    void iterator_fill(It begin, It end) {
        Clear();
        if (begin == end) { return; }
        for (Node* ptr = &head_ ;begin != end; ++begin, ptr = ptr->next_node) {
            ptr->next_node = new Node(*begin, nullptr);
            ++size_;
        }
    }
};

template<typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) {
    lhs.swap(rhs);
}
