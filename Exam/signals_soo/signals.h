#ifndef SIGNALS_H
#define SIGNALS_H

#include <functional>
#include <list>
#include <memory>

template <typename T>
class signals_soo;

template <typename ...Args>
class signals_soo<void(Args...)> {

    enum condition {
        DISCONNECTED,
        CONNECTED,
        NEW_CONNECTED
    };

    typedef std::function<void(Args...)> func;

    struct connection_obj {
        connection_obj(func new_element) : element(new_element), state(NEW_CONNECTED) {}

        void disconnect() {
            state = DISCONNECTED;
        }

        void connect() {
            state = CONNECTED;
        }

        condition connection_state() const {
            return state;
        }

        void operator()(Args ...args) const {
            element(args...);
        }

    private:

        func element;
        condition state;
    };

    typedef std::shared_ptr<connection_obj> ptr_obj;

    struct connection {
        connection(ptr_obj ptr) : ptr(ptr) {}

        void disconnect() {
            ptr->disconnect();
        }

    private:

        ptr_obj ptr;
    };

    size_t size;

    inline bool is_small() const {
        return size <= 1;
    }

    union {
        ptr_obj* element;
        std::list<ptr_obj>* elements;
    };

    void change_signals() {
        if (is_small()) {
            if (size == 1) {
                if ((*element)->connection_state() == NEW_CONNECTED) {
                    (*element)->connect();
                }
                if ((*element)->connection_state() == DISCONNECTED) {
                    delete element;
                    --size;
                }
            }
        } else {
            for (auto it = (*elements).begin(); it != (*elements).end();) {
                if ((*it)->connection_state() == DISCONNECTED) {
                    it = (*elements).erase(it);
                    --size;
                } else {
                    if ((*it)->connection_state() == NEW_CONNECTED) {
                        (*it)->connect();
                    }
                    ++it;
                }
            }
            if (is_small()) {
                std::list<ptr_obj>* buf = elements;
                if (size == 1) {
                    element = new ptr_obj(std::move(*(*buf).begin()));
                }
                delete buf;
            }
        }
    }

public:

    signals_soo() :size(0), element(new ptr_obj()) {}

    connection connect(func new_element) {
        ptr_obj ptr = std::make_shared<connection_obj>(new_element);

        if (is_small()) {
            if (size == 0) {
                element = new ptr_obj(ptr);
            } else {
                ptr_obj* buf = element;
                elements = new std::list<ptr_obj>();
                (*elements).emplace_back(std::move(*buf));
                (*elements).emplace_back(ptr);
                delete buf;
            }
        } else {
            (*elements).emplace_back(ptr);
        }
        ++size;

        return connection(ptr);
    }

    void disconnect_all() {
        for (auto it = (*elements).begin(); it != (*elements).end(); ++it) {
            (*it)->disconnect();
        }
    }

    void operator()(Args ...args) {
        change_signals();
        if (is_small()) {
            if (size == 1) {
                (*(*element))(std::forward<Args>(args)...);
            }
        } else {
            auto it = (*elements).begin();
            while (it != (*elements).end() && (*it)->connection_state() != NEW_CONNECTED) {
                (*(*it))(std::forward<Args>(args)...);
                ++it;
            }
        }
    }
};

#endif
