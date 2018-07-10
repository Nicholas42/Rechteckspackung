#ifndef RECHTECKSPACKUNG_LIST_WRAPPER_H
#define RECHTECKSPACKUNG_LIST_WRAPPER_H

#include <list>

template<class value>
class list_wrap
{
private:
    const std::list<value> &wrapped;
    const bool forward;

    class list_wrap_iterator
    {
    private:
        std::list<value>::iterator forward_it;
        std::list<value>::reverse_iterator reverse_it;
        const bool _forward;
    public:
        explicit list_wrap_iterator(std::list<value>::iterator forward_it_) :
                forward_it(forward_it_),
                _forward(true)
        {}

        explicit list_wrap_iterator(std::list<value>::reverse_iterator reverse_it_) :
                reverse_it(reverse_it_),
                _forward(true)
        {}

        bool operator==(const list_wrap_iterator &other) const;
        value &operator*();
        void operator++();
    };

public:
    using iterator = list_wrap_iterator;

    list_wrap(const std::list<value> &to_wrap, bool forward_) :
            wrapped(to_wrap),
            forward(forward_)
    {}

    iterator begin();
    iterator end();
};

#endif //RECHTECKSPACKUNG_LIST_WRAPPER_H
