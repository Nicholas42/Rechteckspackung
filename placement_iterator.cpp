#include "placement_iterator.h"

rectangle_iterator &rectangle_iterator::operator++()
{
	_at_end = true;
	for (auto rect_ref : _rect_list)
	{
		auto &rect = rect_ref.get();
		rect.rotate(rotation::rotated_90);

		if (_bounds_only && rect.rot == rotation::rotated_180)
		{
			rect.rotate(rotation::rotated_180);
		}

		if (rect.rot != rotation::rotated_0)
		{
			_at_end = false;
			break;
		}

		if (!_bounds_only)
		{
			rect.flip();

			if (rect.flipped)
			{
				_at_end = false;
				break;
			}
		}
	}
	return *this;
}

rectangle_iterator::operator bool() const
{
	return !_at_end;
}

template<class It>
bool placement_iterator::_next_combination(It begin, It middle, It end)
{
	if (begin == middle || middle == end)
	{
		return false;
	}

	It backward_scan = middle;
	It forward_scan = middle;
	It last = end - 1;

	while ((--backward_scan != begin) && (*backward_scan > *last));

	bool last_permutation = (backward_scan == begin) && (*begin > *last);

	if (!last_permutation)
	{
		while ((forward_scan != last) && (*backward_scan > *forward_scan))
		{
			forward_scan++;
		}

		begin = backward_scan;
		std::iter_swap(begin, forward_scan);
		begin++;
		forward_scan++;
	}

	if (begin != middle && forward_scan != end)
	{
		backward_scan = middle;
		last = forward_scan;
		while (backward_scan != begin && forward_scan != end)
		{
			backward_scan--;
			std::iter_swap(backward_scan, forward_scan);
			forward_scan++;
		}

		std::reverse(begin, backward_scan);
		std::reverse(begin, middle);

		std::reverse(forward_scan, end);
		std::reverse(last, end);
	}

	return !last_permutation;
}

placement_iterator::placement_iterator(packing & pack, size_t optimality, bool bounds_only) :
	_rect_it(std::vector<std::reference_wrapper<rectangle>>(), _bounds_only),
	_pack(pack),
	_optimality(optimality),
	_bounds_only(bounds_only),
	_at_end(false),
	_new_subset(false),
	_sp(_pack.get_num_rects())
{
	if (_optimality >= _pack.get_num_rects())
	{
		throw std::out_of_range("optimality");
	}

	if (_optimality == 0)
	{
		std::vector<std::reference_wrapper<rectangle>> rect_list;
		for (size_t i = 0; i < _pack.get_num_rects(); i++)
		{
			rect_list.push_back(_pack.get_rect(i));
		}
		_rect_it = rectangle_iterator(rect_list, _bounds_only);
	}
	else
	{
		_negative_subset = std::vector<size_t>(_sp.negative_locus.begin(), _sp.negative_locus.end());
		_positive_subset = std::vector<size_t>(_sp.positive_locus.begin(), _sp.positive_locus.end());
		_subset_positions = std::vector<std::pair<std::list<size_t>::iterator, std::list<size_t>::iterator>>(_optimality);
		auto _pos_it = _sp.positive_locus.begin(), _neg_it = _sp.negative_locus.begin();
		for (size_t i = 0; i < _optimality; i++)
		{
			_subset_positions[i].first = _pos_it;
			_subset_positions[i].second = _neg_it;
			_pos_it++;
			_neg_it++;
		}
	}
}

void placement_iterator::_next_subset()
{
	_new_subset = false;

	for (size_t i = 0; i < _optimality; i++)
	{
		std::cout << _positive_subset[i] << " ";
	}
	std::cout << std::endl;

	_at_end = !_next_combination(_positive_subset.begin(), _positive_subset.begin() + _optimality, _positive_subset.end());
	_negative_subset.assign(_positive_subset.begin(), _positive_subset.end());

	size_t j = 0;
	for (auto it = _sp.positive_locus.begin(); it != _sp.positive_locus.end(); it++)
	{
		if (j == _optimality)
		{
			break;
		}

		if (*it == _positive_subset[j])
		{
			_subset_positions[j].first = it;
			j++;
		}
	}

	j = 0;
	for (auto it = _sp.negative_locus.begin(); it != _sp.negative_locus.end(); it++)
	{
		if (j == _optimality)
		{
			break;
		}

		if (*it == _negative_subset[j])
		{
			_subset_positions[j].second = it;
			j++;
		}
	}
}

placement_iterator & placement_iterator::operator++()
{
	if (_optimality == 0)
	{
		if (!(++_rect_it) && !std::next_permutation(_sp.negative_locus.begin(), _sp.negative_locus.end()))
		{
			_at_end = !std::next_permutation(_sp.positive_locus.begin(), _sp.positive_locus.end());
		}
	}
	else
	{
		if (_new_subset)
		{
			_next_subset();
		}

		if (!(++_rect_it))
		{
			//Permute subsets
			if (!std::next_permutation(_negative_subset.begin(), _negative_subset.begin() + _optimality))
			{
				_new_subset = !std::next_permutation(_positive_subset.begin(), _positive_subset.begin() + _optimality);
			}

			//Write permutation of subsets to loci
			for (size_t i = 0; i < _subset_positions.size(); i++)
			{
				(*_subset_positions[i].first) = _positive_subset[i];
				(*_subset_positions[i].second) = _negative_subset[i];
			}
		}
	}

	return *this;
}

placement_iterator::operator bool() const
{
	return !_at_end;
}

sequence_pair & placement_iterator::operator*()
{
	return _sp;
}
