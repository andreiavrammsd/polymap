# Poly map

[![build](https://github.com/andreiavrammsd/polymap/workflows/build/badge.svg)](https://github.com/andreiavrammsd/polymap/actions)

Polymorphic map container.

A recursive map with infinite levels and multiple types for keys and values.

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
    map[1][2] = 8;
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
