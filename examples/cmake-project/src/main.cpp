#include <cassert>

#include "msd/poly_map.hpp"

int main()
{
    msd::poly_map<int> map;

    map[0][0] = 1;
    map[0][1] = 2;
    map[0][2] = 3;

    map[1][1] = 5;
    map[1][2] = 6;

    map[2][0] = 7;
    map[2][1] = 8;
    map[2][2] = 9;

    assert(!map.contains(1, 0));
    assert(map.at(2, 2).get<int>() == 9);
}
