#include "list_wrapper.h"

template<class value>
bool list_wrap<value>::list_wrap_iterator::operator==(const list_wrap::list_wrap_iterator &other) const
{
    if (_forward != other._forward)
    {
        return false;
    }

    if (_forward)
    {
        return forward_it == other.forward_it;
    }
    else
    {
        return reverse_it == other.reverse_it;
    }
}

template<class value>
value &list_wrap<value>::list_wrap_iterator::operator*()
{
    if (_forward)
    {
        return *forward_it;
    }
    else
    {
        return *reverse_it;
    }
}

template<class value>
void list_wrap<value>::list_wrap_iterator::operator++()
{
    if(_forward)
    {
        ++forward_it;
    }
    else
    {
        ++reverse_it;
    }
}

template<class value>
list_wrap::iterator list_wrap<value>::begin()
{
    if(forward)
    {
        return iterator(wrapped.begin());
    }
    else
    {
        return iterator(wrapped.rbegin());
    }
}

template<class value>
list_wrap::iterator list_wrap<value>::end()
{
    if(forward)
    {
        return iterator(wrapped.end());
    }
    else
    {
        return iterator(wrapped.rend());
    }
}