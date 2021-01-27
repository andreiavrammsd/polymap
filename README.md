# Poly map

[![build](https://github.com/andreiavrammsd/polymap/workflows/build/badge.svg)](https://github.com/andreiavrammsd/polymap/actions)

### Polymorphic map container.

A recursive map that can have any shape and can hold multiple types for keys and values.

* Header-only library.
* Key types are provided at construct time as template arguments. They must meet the type conditions for [std::variant](https://en.cppreference.com/w/cpp/utility/variant).
* Value types must be copy constructible as the map takes ownership on the objects assigned.
* Can be visualised as a tree.

## Requirements

* C++17
* CMake 3.17+

## Usage

```c++
#include <cassert>
#include <iostream>
#include <string>

#include "msd/poly_map.hpp"

struct visitor {
    template <typename V, typename M>
    auto operator()(const double key, V&, M&)
    {
        std::cout << "double = " << key << "\n";
        return true;
    }

    template <typename K, typename V, typename M>
    auto operator()(K& key, V&, M&)
    {
        std::cout << "other = " << key << "\n";
        return true;
    }
};

int main() {
    msd::poly_map<int, double, std::string> map;

    map[1] = 22;
    map.at(1) = 23;
    
    map[1] = std::string{"a"};
    map[1][2] = 9;
    map.at(1, 2) = 8;
    map[1][2][3.1] = 1;
    map[1][2][3.1]["f"] = 199;
    map[1][2][3.1][4.2]["g"] = std::make_pair(1, 2);

    assert(map[1][2].get<int>() == 8);

    map.for_each(visitor{});

    assert(!map[1].empty());

    assert(map.size() == 6);
    assert(map[1].size() == 5);

    assert(map.contains(1, 2, 3.1));

    map[1][2].clear();
    map.clear();
}
```

See [tests](tests/poly_map_test.cpp).

<br>

Developed with [CLion](https://www.jetbrains.com/?from=serializer)

<a href="https://www.jetbrains.com/?from=serializer">![JetBrains](jetbrains.svg)</a>
