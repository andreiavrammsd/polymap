#include <iostream>
#include <string>

#include "msd/poly_map.hpp"

// https://en.cppreference.com/w/cpp/utility/variant/visit#Example
template <class... Ts>
struct visitor : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
visitor(Ts...) -> visitor<Ts...>;

int main()
{
    msd::poly_map<int, double, std::string> map;

    map[1] = "a";
    map[1][2] = 8;
    map[1][2][3.1] = 1;
    map[1][2][3.1]["f"] = 199;
    map[1][2][3.1][4.2]["g"] = 199;

    const auto lambda_visitor = visitor{
        [](const double key, auto& value, auto&) {
            std::cout << "double key " << key << " " << (value.empty() ? "has no value" : "has a value") << "\n";
            return true;
        },

        [](const std::string& key, auto& value, auto&) {
            std::cout << "string key " << key << " " << (value.empty() ? "has no value" : "has a value") << "\n";
            return true;
        },

        [](auto key, auto& value, auto&) {
            std::cout << "other key " << key << " " << (value.empty() ? "has no value" : "has a value") << "\n";
            return true;
        },
    };

    map.for_each(lambda_visitor);
}
