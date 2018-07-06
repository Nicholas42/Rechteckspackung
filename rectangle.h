#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <optional>
#include <iostream>
#include <cassert>
#include <tuple> // tie
#include <algorithm>
#include "net.h"
#include "common.h"


struct rectangle
{
    // Basepoint, may be not set
    point base;

    // Point contains width as x and height as y
    point size;
    int id = 0;

    bool blockage = false;
    bool flipped = false;
    rotation rot = rotation::rotated_0;

    rectangle() = default;

    rectangle(point min, point max) :
            base(min),
            size(max.x - min.x, max.y - min.y, true)
    {
        assert(size.x >= 0 && size.y >= 0);
    }

    /**
     * Checks whether the rectangle is rotated by 90 or 270 degrees, so the width and height are interchanged
     * @return rot == rotated_90 || rot == rotated_270
     */
    bool rotated() const;

    /**
     * For the following methods, the left and lower edge of the rectangle belongs to the rectangle, but the
     * right and upper edge does not belong to the rectangle.
     */

    /**
     * Checks if the given point is contained in the rectangle.
     * @param p point to check
     * @return true if the rectangle contains the position of the point in each dimension.
     */
    bool contains(const point p) const;

    /**
     * Checks if the given position is inside the rectangle in the given dimension.
     * @param to_check position to check
     * @param dim dimension that we consider
     * @return true if the rectangle contains the position in the given dimension.
     */
    bool contains(const pos to_check, dimension dim) const;

    /**
     * alias for contains(to_check, dimension::x)
     */
    bool contains_x(const pos to_check) const;

    /**
     * alias for contains(to_check, dimension::y)
     */
    bool contains_y(const pos to_check) const;

    /**
     * Gets width or height of the rectangle. The rectangle does not need to be placed.
     * @param dim specifies if width or height is returned
     * @return width if dim is dimension::x, height otherwise
     */
    pos get_dimension(dimension dim) const;

    /**
     * Gets the basepoint of the rectangle in the specified dimension. Only works on already placed rectangles.
     * @param dim The dimension to consider
     * @param other If true, the other dimension is taken. This can be useful if we need the other dimension but do not
     * know in which dimension we are.
     * @return base.coord(dim, other)
     */
    pos get_pos(dimension dim, bool other = false) const;

    /**
     * Getter for the right or upper boundary of the rectangle. Only works on already placed rectangles.
     * Observes rotation.
     * @param dim The dimension of the boundary to return
     * @param other If true the other dimension is taken.
     * @return Either the right or the upper boundary
     */
    pos get_max(dimension dim, bool other = false) const;

    point get_max_point() const;

    /**
     * Checks if the rectangle is already placed.
     * @return base.set
     */
    bool placed() const;

    /**
     * Checks if the rectangle intersects the specified rectangle. Both rectangle contains lower and left but not
     * upper and right border.
     * @param rect The rectangle to check intersection with
     * @return True if the rectangle have an inner point in common
     */
    bool intersects(const rectangle &rect) const;

    /**
     * Compares this rectangle with the specified and returns which is smaller in the specified dimension.
     * @param rect The rectangle to compare with
     * @param dim The dimension in which we compare
     * @return True if the basepoint of this rectangle has a smaller coordinate in the specified dimension than the
     * basepoint of rect.
     */
    bool smaller(const rectangle &rect, dimension dim) const;
    bool operator<(const rectangle &rect) const;

    /**
     * Returns the relative position of a pin on this rectangle in the given
     * dimension. Flipping and rotation are observed. Only works on already
     * placed rectangles and if the pin belongs to the rectangle.
     * @param p The pin which position shall be returned. Has to be on this rectangle.
     * @param dim The dimension of the position to return.
     * @return The relative position of the pin in this dimension.
     */
    pos get_relative_pin_position(const pin &p, dimension dim) const;

    /**
     * Returns the relative position of a pin on this rectangle. Flipping and
     * rotation are observed. Only works if the pin belongs to the rectangle.
     * The rectangle does not need to be already placed.
     * @param p The pin which position shall be returned. Has to be on this rectangle.
     * @return The relative position of the pin as a pair of pos.
     */
    point get_relative_pin_position(const pin &p) const;

    /**
     * Returns the absolute position of a pin on this rectangle. Flipping and
     * rotation are observed. Only works on already placed rectangles and if
     * the pin belongs to the rectangle.
     * @param p The pin which position shall be returned. Has to be on this rectangle.
     * @return The absolute position of the pin as a pair of pos.
     */
    point get_absolute_pin_position(const pin &p) const;

    /**
     * Rotates the rectangle by the passed rotation. This does NOT set the rotation but rather
     * increments it by the passed rotation.
     * @param rotate rotation by that we increase our current rotation
     */
    void rotate(const rotation rotate);

    /**
     * Flips the rectangle, i.e. inverts the flipped variable.
     */
    void flip();

    /**
     * Returns a new rectangle that is the intersection of this rectangle with the specified rectangle.
     * Warning: May return an invalid rectangle if the rectangles do not intersect.
     * @param other The rectangle with that we want to intersect
     * @return A rectangle which has the area of the intersection.
     */
    rectangle intersection(const rectangle &other) const;

    /**
     * A static wrapper for the left-of-comparison
     * @param left
     * @param right
     * @return left.left_of(right)
     */
    static bool compare(const rectangle &left, const rectangle &right);
};

std::ostream &operator<<(std::ostream &out, const rectangle &rect);

std::istream &operator>>(std::istream &in, rectangle &rect);

#endif // RECTANGLE_H