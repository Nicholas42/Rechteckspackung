#include "sequence_pair.h"

sequence_pair_iterator &sequence_pair_iterator::operator++()
{
    if(!std::next_permutation(_sp.negative_locus.begin(), _sp.negative_locus.end()))
    {
        _at_end = !std::next_permutation(_sp.positive_locus.begin(), _sp.positive_locus.end());
    }
    return *this;
}

sequence_pair_iterator::operator bool() const
{
    return !_at_end;
}

std::ostream &operator<<(std::ostream &out, const sequence_pair & sp)
{
    out << "Positive Locus: ";

    for(auto i : sp.positive_locus)
    {
        out << i << ", ";
    }

    out << std::endl << "Negative Locus: ";

    for(auto i : sp.negative_locus)
    {
        out << i << ", ";
    }
    out << std::endl;

    return  out;
}

sequence_pair &sequence_pair_iterator::operator*()
{
    return _sp;
}
