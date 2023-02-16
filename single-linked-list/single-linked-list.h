#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

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
    
    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }

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
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ != rhs.node_;
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
			assert(node_ != nullptr);
            this->node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            auto temp = BasicIterator(this->node_);
			assert(node_ != nullptr);
            this->node_ = node_->next_node;
            return temp;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
			assert(node_ != nullptr);
            return this->node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
			assert(node_ != nullptr);
            return &this->node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    SingleLinkedList() = default;
    
    ~SingleLinkedList(){
        Clear();
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0 ? true : false; 
    }
    
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    void Clear() noexcept {
        while(head_.next_node != nullptr){
            auto temp = (head_.next_node)->next_node;
            delete head_.next_node;
            head_.next_node = temp;
        }
        size_ = 0;
    }
    
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        auto it_begin = head_.next_node;
        return BasicIterator<value_type>(it_begin);
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        auto it_end = head_.next_node;
        for(size_t i = 0; i < size_; ++i){
            it_end = it_end->next_node;
        }
        return BasicIterator<value_type>(it_end);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        auto it_begin = head_.next_node;
        return BasicIterator<const value_type>(it_begin);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        auto it_end = head_.next_node;
        for(size_t i = 0; i < size_; ++i){
            it_end = it_end->next_node;
        }
        return BasicIterator<const value_type>(it_end);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        auto it_begin = head_.next_node;
        return BasicIterator<const value_type>(it_begin);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        auto it_end = head_.next_node;
        for(size_t i = 0; i < size_; ++i){
            it_end = it_end->next_node;
        }
        return BasicIterator<const value_type>(it_end);
    }
    
	template <typename Container>
	void FillSingleLinkedList(SingleLinkedList& temp, Container& container) const {
		try{
			Node* node = new Node(*container.begin(), nullptr); 
			temp.head_.next_node = node; 
			++temp.size_;
			for(auto it = std::next(container.begin(), 1); it != container.end(); ++it){ 
				node->next_node = new Node(*it, nullptr); 
				node = node->next_node;
				++temp.size_;
			}
		} catch (...) {
			throw;
		}
	}
		
	SingleLinkedList(std::initializer_list<Type> values) {
		SingleLinkedList temp;
		if(values.size()){
			FillSingleLinkedList(temp, values);
		}
		swap(temp);
	}

	SingleLinkedList(const SingleLinkedList& other) {
		assert(size_ == 0 && head_.next_node == nullptr);
		SingleLinkedList temp;
		if(!other.IsEmpty()){
			FillSingleLinkedList(temp, other);
		}
		swap(temp);
	}
    
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList temp(rhs);
        swap(temp);
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->size_, other.size_);
        std::swap(this->head_.next_node, other.head_.next_node);
    }
    
    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return BasicIterator<value_type>(&head_);
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        Node* head = const_cast<Node*>(&head_);
        return BasicIterator<const value_type>(head);
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return BasicIterator<const value_type>(&head_);
    }
    
    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        try{
			assert(pos.node_ != nullptr);
            Node* new_node = new Node(value, std::next(pos, 1).node_);
            pos.node_->next_node = new_node;
            ++size_;
            return BasicIterator<value_type>(new_node);
        } catch (...) {
            throw;
        }
    }
    
    void PopFront() noexcept {
        if(head_.next_node){
            Node* node = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = node;
            --size_;
        }
    }
    
    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
		assert(pos.node_ != nullptr);
        Node* node = std::next(pos, 1).node_->next_node;
        delete std::next(pos, 1).node_;
        pos.node_->next_node = node;
        --size_;
        return BasicIterator<value_type>(node);
    }
    
private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;
};

    template <typename Type>
    void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
        lhs.swap(rhs);
    }

    template <typename Type>
    bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return std::equal(lhs.begin(), lhs.end(),
                          rhs.begin(), rhs.end());
    }

    template <typename Type>
    bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Type>
    bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                            rhs.begin(), rhs.end());
    }

    template <typename Type>
    bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return lhs == rhs || lhs < rhs;
    }

    template <typename Type>
    bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return rhs < lhs;
    }

    template <typename Type>
    bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
        return lhs == rhs || lhs > rhs;
    }