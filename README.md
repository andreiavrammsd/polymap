# Poly map

Polymorphic map container.

A recursive map with infinite levels and multiple types for keys and values.

## Requirements

* C++17
* CMake 3.12+

## Usage

```c++
#include <cassert>
#include <iostream>

#include "msd/poly_map.hpp"

struct visitor {
    template <typename K, typename V, typename M>
    auto operator()(K& key, V&, M&)
    {
        std::cout << key << "\n";
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

    assert(map.contains(1, 2, 3.1));

    map.for_each(visitor{});

    map.clear();
}
```

See [tests](tests/poly_map_test.cpp).

<br>

Developed with [CLion](https://www.jetbrains.com/?from=serializer)

<a href="https://www.jetbrains.com/?from=serializer">![JetBrains](jetbrains.svg)</a>
