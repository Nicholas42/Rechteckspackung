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

std::vector<pos> sequence_pair::place_dimension(dimension dim, const packing & pack) const
{
	std::vector<pos> positions(pack.get_num_rects());

	std::vector<size_t> y_indexes(pack.get_num_rects());
	int i = 0;
	for (auto b : negative_locus)
	{
		y_indexes[b] = i++;
	}

	std::map<size_t, pos> cur_seqs;
	cur_seqs[0] = 0; //This is the node for no seq found

	auto loop_content = [&](const size_t pack_index)
	{
		size_t y_index = y_indexes[pack_index];
		//We insert with length 0 to optimize the greatest index less than search
		//0 is the node for no seq found so we treat the indices one-based in the context of cur_seqs
		const auto cur_node = cur_seqs.insert({ y_index + 1, 0 }).first;
		positions[pack_index] = std::prev(cur_node)->second;
		pos cur_seq_length = positions[pack_index] + pack.get_rect(pack_index).get_dimension(dim);
		cur_seqs[y_index + 1] = cur_seq_length;

		//We go forward until we no longer need to delet nodes
		while (std::next(cur_node) != cur_seqs.end() && std::next(cur_node)->second < cur_seq_length)
		{
			cur_seqs.erase(std::next(cur_node)->first);
		}
	};

	//Reverse iterators have the wrong type so we can't just assign a variable conditionally
	if (dim == dimension::x)
	{
		std::for_each(positive_locus.begin(), positive_locus.end(), loop_content);
	}
	else
	{
		std::for_each(positive_locus.rbegin(), positive_locus.rend(), loop_content);
	}


	return positions;
}

bool sequence_pair::apply_to(packing & pack)
{
	if (pack.get_num_rects() != positive_locus.size())
	{
		throw new std::invalid_argument("Sequence pair length does not match packing");
	}

	auto x_coords = place_dimension(dimension::x, pack);
	auto y_coords = place_dimension(dimension::y, pack);

	//We are placing rectangles starting (0,0), but the placmenet area might be different
	auto x_offset = pack.get_chip_base().get_pos(dimension::x);
	auto y_offset = pack.get_chip_base().get_pos(dimension::y);

	for (size_t i = 0; i < x_coords.size(); i++)
	{
		pack.move_rect(i, point(x_coords[i] + x_offset, y_coords[i] + y_offset, true));

		assert(i == 0 || !pack.get_rect(i).intersects(pack.get_rect(0)));

		//TODO: This is ugly
		if (!(pack.get_chip_base().contains(pack.get_rect(i).base) 
			&& pack.get_rect(i).get_max(dimension::x) <= pack.get_chip_base().get_max(dimension::x) 
			&& pack.get_rect(i).get_max(dimension::y) <= pack.get_chip_base().get_max(dimension::y)))
		{
			return false;
		}
	}

	return true;
}
