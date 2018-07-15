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

//Source (with modifications): http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2639.pdf
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

	//Find the greatest element in the subset which is smaller than the greatest element not in the subset
	//We call this element e
	while ((--backward_scan != begin) && (*backward_scan > *last));

	//If all elements in the subset are greater than the elements not in the subset we have reached the last subset
	bool last_permutation = (backward_scan == begin) && (*begin > *last);

	if (!last_permutation)
	{
		//Find the smallest element not in the subset greater than e. We call this element f
		while ((forward_scan != last) && (*backward_scan > *forward_scan))
		{
			forward_scan++;
		}

		//Swap the values e and f
		std::iter_swap(backward_scan, forward_scan);

		//[begin, backward_scan] is untouched so ignore
		begin = backward_scan + 1;

		//We have nothing more to do if forward_scan is pointing to last
		forward_scan++;
	}

	//This always happens if last_permutation is true. If so we just reverse the whole permutation and skip the loop
	//Else this means that we need to transfer multiple elements between subset and not-subset
	if (begin != middle && forward_scan != end)
	{
		//Swap the elements coming behind f until excluive middle out of the subset, so that the sequence behind
		//f has the smallest possible values out of all elements not in the subset (though it is in reverse order)
		backward_scan = middle;
		last = forward_scan;
		while (backward_scan != begin && forward_scan != end)
		{
			backward_scan--;
			std::iter_swap(backward_scan, forward_scan);
			forward_scan++;
		}

		//The swapped parts are reversed
		std::reverse(begin, backward_scan);
		std::reverse(begin, middle);

		std::reverse(forward_scan, end);
		std::reverse(last, end);
	}

	return !last_permutation;
}

placement_iterator::placement_iterator(packing & pack, size_t optimality, bool bounds_only) :
	_pack(pack),
	_optimality(optimality),
	_bounds_only(bounds_only),
	_at_end(false),
	_new_subset(false),
	_rect_it(std::vector<std::reference_wrapper<rectangle>>(), bounds_only),
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
			rect_list.push_back(_pack.get_rect((int)i));
		}
		_rect_it = rectangle_iterator(rect_list, _bounds_only);
	}
	else
	{
		//Initialize for first subset
		_negative_subset = std::vector<size_t>(_sp.negative_locus.begin(), _sp.negative_locus.end());
		_positive_subset = std::vector<size_t>(_sp.positive_locus.begin(), _sp.positive_locus.end());
		_subset_positions = std::vector<std::pair<std::list<size_t>::iterator, std::list<size_t>::iterator>>(_optimality);
		auto _pos_it = _sp.positive_locus.begin(), _neg_it = _sp.negative_locus.begin();
		for (size_t i = 0; i < _optimality; i++)
		{
			_rect_subset.push_back(std::ref(_pack.get_rect((int)_positive_subset[i])));
			_subset_positions[i].first = _pos_it;
			_subset_positions[i].second = _neg_it;
			_pos_it++;
			_neg_it++;
		}
		_rect_it = rectangle_iterator(_rect_subset, _bounds_only);
	}
}

void placement_iterator::_next_subset()
{
	_new_subset = false;

	//Generate new subset
	_at_end = !_next_combination(_positive_subset.begin(), _positive_subset.begin() + _optimality, _positive_subset.end());
	_negative_subset.assign(_positive_subset.begin(), _positive_subset.end());

	//Set iterators in sequence pair for new subset
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

	//Tell rectangle iterator which rectangles to permute
	for (size_t i = 0; i < _optimality; i++)
	{
		_rect_subset[i] = std::ref(_pack.get_rect((int)_positive_subset[i]));
	}
	_rect_it = rectangle_iterator(_rect_subset, _bounds_only);
}

placement_iterator & placement_iterator::operator++()
{
	if (_optimality == 0) //Optimize globally
	{
		if (!(++_rect_it) && !std::next_permutation(_sp.negative_locus.begin(), _sp.negative_locus.end()))
		{
			_at_end = !std::next_permutation(_sp.positive_locus.begin(), _sp.positive_locus.end());
		}
	}
	else //Optimize k-locally
	{
		if (_new_subset)
		{
			_next_subset();
		}

		//Rotate rectangles
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
