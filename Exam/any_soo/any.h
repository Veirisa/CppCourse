//
// Created by anya on 04.03.17.
//

#ifndef ANY_H
#define ANY_H

#include <typeinfo>
#include <utility>
#include <cstdint>
#include <cstdio>
#include <memory>

const size_t MAX_SMALL_SIZE = 16;

struct any {

    typedef void** ptr_type;

    //constructors

    any() : state(NOT_INIT),
            deleter([](void*, bool){}),
            copier([](void*, const void*){}),
            movier([](void*, void*){}),
            allocator([]() -> void* {return nullptr;}),
            type_getter([]() -> const std::type_info& {return typeid(void);}) {};

    template<typename T, typename = typename std::enable_if<!std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, any>::value>::type>
    any(const T& t) {
        typedef typename std::remove_cv<typename std::decay<const T>::type>::type type;
        if (sizeof(type) <= MAX_SMALL_SIZE && std::is_nothrow_move_constructible<type>::value) {
            state = SMALL;
            new (&data) type(t);
        } else {
            state = BIG;
            void* ptr = new type(t);
            *(ptr_type)&data = ptr;
        }
        initial<type>();
    }

    template<typename T, typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, any>::value>::type>
    any(T&& t) {
        typedef typename std::decay<T>::type type;
        if (sizeof(type) <= MAX_SMALL_SIZE && std::is_nothrow_move_constructible<type>::value) {
            state = SMALL;
            new (&data) type(std::forward<T>(t));
        } else {
            state = BIG;
            void* ptr = new type(std::forward<T>(t));
            *(ptr_type)&data = ptr;
        }
        initial<type>();
    };

    //destructor

    ~any() {
        if (state == SMALL) {
            deleter(&data, true);
        }
        if (state == BIG) {
            deleter(*(ptr_type)&data, false);
        }
        state = NOT_INIT;
    }

    //copy constructors

    any(const any& other) : state(other.state),
                            deleter(other.deleter),
                            copier(other.copier),
                            movier(other.movier),
                            allocator(other.allocator),
                            type_getter(other.type_getter) {
        if (state == BIG) {
            void* ptr = allocator();
            copier(ptr, *(ptr_type)&other.data);
            *(ptr_type)&data = ptr;
        } else {
            if (state == SMALL) {
                copier(&data, &other.data);
            }
        }
    }

    any(any&& other) : state(other.state),
                       deleter(other.deleter),
                       copier(other.copier),
                       movier(other.movier),
                       allocator(other.allocator),
                       type_getter(other.type_getter) {
        if (state == BIG) {
            void* ptr = allocator();
            movier(ptr, *(ptr_type)&other.data);
            *(ptr_type)&data = ptr;

        } else {
            if (state == SMALL) {
                movier(&data, &other.data);
            }
        }
    }

    //operator=

    any& operator=(const any& other) {
        any(other).swap(*this);
        return *this;
    }

    any& operator=(any&& other) {
        other.swap(*this);
        any().swap(other);
        return *this;
    }

    template<typename T>
    any& operator=(const T& t) {
        any(t).swap(*this);
        return *this;
    }

    template<typename T>
    any& operator=(T&& t) {
        any(std::move(t)).swap(*this);
        return *this;
    }

    //other


    any& swap(any& other) {
        //
        typename std::aligned_storage<MAX_SMALL_SIZE, MAX_SMALL_SIZE>::type buffer;
        void *ptr = &buffer;

        if (state == SMALL) {
            if (other.state == SMALL) {
                other.movier(ptr, &other.data);
                movier(&other.data, &data);
                other.movier(&data, ptr);
            } else {
                other.movier(ptr, &other.data);
                std::swap(*(ptr_type)&other.data, *(ptr_type)&data);
                other.movier(*(ptr_type)&data, ptr);
            }
        } else {
            if (other.state == SMALL) {
                movier(ptr, &data);
                std::swap(*(ptr_type)&data, *(ptr_type)&other.data);
                movier(*(ptr_type)&other.data, ptr);
            } else {
                std::swap(*(ptr_type)&data, *(ptr_type)&other.data);
            }
        }
        //
        std::swap(state, other.state);
        std::swap(deleter, other.deleter);
        std::swap(copier, other.copier);
        std::swap(movier, other.movier);
        std::swap(allocator, other.allocator);
        std::swap(type_getter, other.type_getter);
        return *this;
    }

    bool empty() const {
        return state == NOT_INIT;
    }

    const std::type_info& type() const {
        return type_getter();
    }

    //casts

    template <typename T>
    friend T* any_cast(any* oper);

    template <typename T>
    friend const T* any_cast(const any* oper);

    template <typename T>
    friend T any_cast(const any& oper);

    template <typename T>
    friend T any_cast(any& oper);

    template <typename T>
    friend T any_cast(any&& oper);

private:

    enum condition {
        NOT_INIT,
        SMALL,
        BIG
    };

    template <typename T>
    void initial() {
        deleter = call_delete<T>;
        copier = call_copy<T>;
        movier = call_move<T>;
        allocator = call_allocate<T>;
        type_getter = call_type<T>;
    }

    void* any_ptr() const {
        if (state == SMALL) {
            return (void*)&data;
        }
        if (state == BIG) {
            return *(ptr_type)&data;
        }
        return nullptr;
    }

    //1: state
    condition state;

    //2: data
    std::aligned_storage<MAX_SMALL_SIZE, MAX_SMALL_SIZE>::type data;

    //3: deleter
    typedef void deleter_type(void*, bool);
    deleter_type* deleter;

    template <typename T>
    static void call_delete(void* ptr, bool is_small) {
        if (is_small) {
            ((T*) ptr)->~T();
        } else {
            delete ((T *) ptr);
        }
    }

    //4: copier
    typedef void copier_type(void*, const void*);
    copier_type* copier;

    template <typename T>
    static void call_copy(void* ptr1, const void* ptr2) {
        new (ptr1) T(*(T const*)ptr2);
    }

    //5: movier
    typedef void movier_type(void*, void*);
    movier_type* movier;

    template <typename T>
    static void call_move(void* ptr1, void* ptr2) {
        new (ptr1) T(std::move(*(T*)ptr2));
    }

    //6: allocator
    typedef void* allocator_type();
    allocator_type* allocator;

    template <typename T>
    static void* call_allocate() {
        return new typename std::aligned_storage<sizeof(T), alignof(T)>::type;
    }

    //7: type_getter
    typedef const std::type_info& type_getter_type();
    type_getter_type* type_getter;

    template <typename T>
    static const std::type_info& call_type() {
        return typeid(T);
    }
};

template<typename T>
T* any_cast(any* oper) {
    return oper->empty() || oper->type() != typeid(T) ? nullptr : (T*)(oper->any_ptr()) ;
}

template<typename T>
const T* any_cast(const any* oper) {
    return oper->empty() || oper->type() != typeid(T) ? nullptr : (const T*)(oper->any_ptr());
}

template<typename T>
T any_cast(any& oper) {
    typedef typename std::remove_reference<T>::type type;
    if (oper.empty() || oper.type() != typeid(T)) {
        throw std::bad_cast();
    }
    return *((type*)(oper.any_ptr()));
}

template<typename T>
T any_cast(any&& oper) {
    typedef typename std::remove_reference<T>::type type;
    if (oper.empty() || oper.type() != typeid(T)) {
        throw std::bad_cast();
    }
    return *((type*)(oper.any_ptr()));
}

template<typename T>
T any_cast(const any& oper) {
    typedef typename std::add_const<typename std::remove_reference<T>::type>::type type;
    if (oper.empty() || oper.type() != typeid(T)) {
        throw std::bad_cast();
    }
    return *((type*)(oper.any_ptr()));
}

#endif
