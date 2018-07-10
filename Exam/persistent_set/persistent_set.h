#ifndef PERSISTENT_SET_H
#define PERSISTENT_SET_H

#include <memory>
#include <vector>

class persistent_set {
	struct persistent_node;
	typedef std::shared_ptr<persistent_node> node_ptr;
	node_ptr root;

public:
	// Вы можете определить этот тайпдеф по вашему усмотрению.
	typedef int value_type;

	// Bidirectional iterator.
	struct iterator;

	// Создает пустой persistent_set.
	persistent_set();

	// Создает копию указанного persistent_set-а.
	persistent_set(persistent_set const &);

	// Изменяет this так, чтобы он содержал те же элементы, что и rhs.
	// Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
	persistent_set &operator=(persistent_set const &rhs);

	// Деструктор. Вызывается при удалении объектов persistent_set.
	// Инвалидирует все итераторы ссылающиеся на элементы этого persistent_set
	// (включая итераторы ссылающиеся на элементы следующие за последними).
	~persistent_set();

	// Поиск элемента.
	// Возвращает итератор на элемент найденный элемент, либо end().
	iterator find(value_type);

	// Вставка элемента.
	// 1. Если такой ключ уже присутствует, вставка не производиться, возвращается итератор
	//    на уже присутствующий элемент и false.
	// 2. Если такого ключа ещё нет, производиться вставка, возвращается итератор на созданный
	//    элемент и true.
	// Если вставка произведена, инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
	std::pair<iterator, bool> insert(value_type);

	// Удаление элемента.
	// Инвалидирует все итераторы, принадлежащие persistent_set'у this, включая end().
	void erase(iterator);

	// Возващает итератор на элемент с минимальный ключом.
	iterator begin() const;

	// Возващает итератор на элемент следующий за элементом с максимальным ключом.
	iterator end() const;

private:
	std::vector<node_ptr> find_node(node_ptr cur, value_type const &value);
	node_ptr ascend_and_rebuild(std::vector<node_ptr> const &desc_seq, node_ptr new_tree, bool del,
		value_type const &which) const;
};

class persistent_set::persistent_node {
	bool ending;
	node_ptr left_child = nullptr;
	node_ptr right_child = nullptr;
	persistent_set::value_type value;

	friend class persistent_set;
	friend class persistent_set::iterator;

	friend bool operator==(persistent_set::iterator, persistent_set::iterator);
	friend bool operator!=(persistent_set::iterator, persistent_set::iterator);
public:
	persistent_node();
	persistent_node(value_type value);
};

class persistent_set::iterator {
	std::vector<persistent_set::node_ptr> descent_sequence;
	const persistent_set* parent_set;

	iterator(std::vector<node_ptr> found, persistent_set const *p);

	friend class persistent_set;

public:
	// Элемент на который сейчас ссылается итератор.
	// Разыменование итератора end() неопределено.
	// Разыменование невалидного итератора неопределено.
	value_type const& operator*() const;

	// Переход к элементу со следующим по величине ключом.
	// Инкремент итератора end() неопределен.
	// Инкремент невалидного итератора неопределен.
	iterator& operator++();
	iterator operator++(int);

	// Переход к элементу со следующим по величине ключом.
	// Декремент итератора begin() неопределен.
	// Декремент невалидного итератора неопределен.
	iterator& operator--();
	iterator operator--(int);
	~iterator() = default;

	friend bool operator==(persistent_set::iterator, persistent_set::iterator);
	friend bool operator!=(persistent_set::iterator, persistent_set::iterator);
};


// Сравнение. Итераторы считаются эквивалентными если одни ссылаются на один и тот же элемент.
// Сравнение с невалидным итератором не определено.
// Сравнение итераторов двух разных контейнеров не определено.
bool operator==(persistent_set::iterator, persistent_set::iterator);
bool operator!=(persistent_set::iterator, persistent_set::iterator);

#endif //PERSISTENT_SET_H
