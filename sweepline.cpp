#include <vector>
#include <set>
#include "rectangle.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>

// TODO: Maybe return a proof for the intersection
bool sweepline(std::vector<rectangle> &rects)
{
    // We want to sweep over the rectangles from left to right
    sort(rects.begin(), rects.end(), rectangle::compare);
    
    // The sweeping line is always ordered from bottom to top
    // TODO: maybe just pointer on the rectangles? Now, we copy each rectangle once
    std::set<rectangle> line;

    for(auto rec : rects)
    {
        std::pair<std::set<rectangle>::iterator, bool> in = line.insert(rec);
        assert(in.second);

        std::set<rectangle>::iterator it = in.first;

        // First we want to find the next rectangle below which is still active
        while(it != line.begin() && !std::prev(it)->contains_x(rec.x))
        {
            line.erase(std::prev(it));
        }

        if(it != line.begin() && rec.intersects(*std::prev(it)))
        {
            return false;
        }

        it++;
        // Since this does not intersect our rectangle we can go on to the rectangles above
        while(it != line.end() && !it->contains_x(rec.x))
        {
            // This looks pretty different than the other direction, due to
            // a) line.end() points behind the end of line
            // b) line.erase returns an iterator pointing on the element behind the deleted one
            it = line.erase(it);
        }

        if(it != line.end() && it->intersects(rec))
        {
            return false;
        }        
        // So this rectangle does not intersect with one already there, therefore, we can go on
    }

    return true;
}

int main()
{
    std::vector<rectangle> v;
    std::ifstream file("Instances/Solutions/sol6a");
    
    rectangle r;
    int counter = 0;
    while(file >> r)
    {
        r.id = counter++;
        v.push_back(r);
    }
    
    std::cout << sweepline(v) << std::endl;
}