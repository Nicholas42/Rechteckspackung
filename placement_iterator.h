#ifndef PLACEMENT_ITERATOR_H
#define PLACEMENT_ITERATOR_H

#include <algorithm>
#include <bitset>
#include <functional>
#include <vector>
#include "packing.h"
#include "rectangle.h"
#include "sequence_pair.h"

/**
 * A iterator which iterates over all rotations of rectangles
 */
class rectangle_iterator
{
private:
	std::vector<std::reference_wrapper<rectangle>> _rect_list;
	bool _at_end;
	bool _bounds_only;
public:
	/**
	 * Constructs a rectangle iterator.
	 * @param rect_list A list with references to the rectangles which are to be rotated. These rectangles will be
	 * modified.
	 * @param bounds_only Indicates whether only the bound of the rectangle (only unrotated and rotated by 90 deg) or
	 * all possible rotations and flips (only relevant for pins) should be considered.
	 */
	explicit rectangle_iterator(std::vector<std::reference_wrapper<rectangle>> rect_list_, bool bounds_only_) :
		_rect_list(rect_list_),
		_at_end(false),
		_bounds_only(bounds_only_)
	{}

	/**
	 * Rotates or flips a rectangle to generate the next possibility. When incrementing the last possibility, the rectangles
	 * are returned to the unrotated state.
	 * @return This iterator.
	 */
	rectangle_iterator &operator++();

	/**
	 * Indicates whether the last possibility has been reached.
	 * @return True if there is another possibility, false if the end has been reached.
	 */
	explicit operator bool() const;
};

class placement_iterator
{
private:
	packing & _pack;
	size_t _optimality;
	bool _bounds_only;
	bool _at_end, _new_subset;
	rectangle_iterator _rect_it;
	sequence_pair _sp;
	std::vector<size_t> _positive_subset, _negative_subset;
	std::vector<std::pair<std::list<size_t>::iterator, std::list<size_t>::iterator>> _subset_positions;
	std::vector<std::reference_wrapper<rectangle>> _rect_subset;

	/**
	 * Creates the next combination in lexicographic order. The current subset is found from [begin, middle), 
	 * the rest of the set is found from [middle, end). Returns false if this is the last combination.
	 * [begin, middle) and [middle, end) have to be sorted.
	 */
	template<class It>
	bool _next_combination(It begin, It middle, It end);
	void _next_subset();

public:
	/**
	 * Creates a iterator which iterates over all possible combinations of sequence pairs and rotations considering
	 * the parameters. To iterate k-optimaly, all subsets of size k are iterated and then all permutations of this subset
	 * within the sequence pair are iterated.
	 * @param pack The packing over which the iteration should be performed. The rectangles in this packing will be rotated
	 * and thus modified.
	 * @param optimality The maximum number of rectangles that will be interchanged in each step. If zero the sequence pair
	 * will simply be permuted.
	 * @param bounds_only Indicates whether only the bound of the rectangle (only unrotated and rotated by 90 deg) or
	 * all possible rotations and flips (only relevant for pins) should be considered.
	 */
	placement_iterator(packing & pack, size_t optimality, bool bounds_only);

	/**
	 * Advances the iterator. Rotates rectangles and if neccessary permutes the sequence pair.
	 * @return This iterator.
	 */
	placement_iterator &operator++();

	/**
	* Indicates whether the last possibility has been reached.
	* @return True if there is another possibility, false if the end has been reached.
	*/
	operator bool() const;

	/**
	 * Gets the sequence pair belonging to the current iterator state
	 * @return This sequence pair
	 */
	sequence_pair &operator*();
};

#endif // !PLACEMENT_ITERATOR_H
