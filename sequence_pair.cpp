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

std::vector<pos> sequence_pair::place_dimension(dimension dim, const locusIt x_begin, const locusIt x_end, const locusIt y_begin, const locusIt y_end, const packing & pack) const
{
	std::vector<pos> positions(pack.get_num_rects());

	std::vector<size_t> y_indexes(pack.get_num_rects());
	int i = 0;
	for (auto it = y_begin; it != y_end; it++)
	{
		y_indexes[*it] = i++;
	}

	std::map<size_t, pos> cur_seqs;
	cur_seqs[0] = 0;

	for (auto it = x_begin; it != x_end; it++)
	{
		size_t pack_index = *it;
		size_t y_index = y_indexes[pack_index];
		auto cur_node = cur_seqs.insert({ pack_index + 1, 0 });
		positions[pack_index] = (--cur_node.first)->second;
		pos cur_seq_length = positions[pack_index] + pack.get_rect(pack_index).get_dimension(dim);
		cur_seqs[pack_index + 1] = cur_seq_length;
		while ((++cur_node.first)->second < cur_seq_length)
		{
			cur_seqs.erase((++cur_node.first)->first);
		}
	}

	return positions;
}

void sequence_pair::apply_to(packing & pack)
{
	if (pack.get_num_rects() != positive_locus.size())
	{
		throw new std::invalid_argument("Sequence pair length does not match packing");
	}

	//Get coordinates by finding LCS in (X, Y) and (X^R, Y)
	auto x_coords = place_dimension(dimension::x, positive_locus.begin(), positive_locus.end(), negative_locus.begin(), negative_locus.end(), pack);
	auto y_coords = place_dimension(dimension::y, positive_locus.end(), positive_locus.begin(), negative_locus.begin(), negative_locus.end(), pack);

	for (size_t i = 0; i < x_coords.size(); i++)
	{
		pack.move_rect(i, point(x_coords[i], y_coords[i], true));
	}
}
