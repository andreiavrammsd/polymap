#include "msd/poly_map.hpp"

#include <any>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

#include "gtest/gtest.h"

class PolyMapTest : public ::testing::Test {
   protected:
    msd::poly_map<int, double, std::string> map;
    const msd::poly_map<int, double, std::string>& const_map = map;

    const double eps = std::numeric_limits<double>::epsilon();

    void SetUp() override
    {
        map[1] = 22;
        map.at(1) = 23;

        map[1] = std::string{"a"};
        map[1][2] = 9;
        map.at(1, 2) = 8;
        map[1][2][3.1] = 1;
        map[1][2][3.1]["f"] = 199;
        map[1][2][3.1][4.2]["g"] = std::make_pair(1, 2);
    }
};

TEST_F(PolyMapTest, at)
{
    EXPECT_EQ(map.at(1).get<std::string>(), "a");
    EXPECT_THROW((void)map.at(999), std::out_of_range);

    EXPECT_EQ(map.at(1).at(2).get<int>(), 8);
    EXPECT_THROW((void)map.at(1).at(2).at(8), std::out_of_range);

    EXPECT_EQ((map.at(1).at(2).at(3.1).at(4.2).at("g").get<std::pair<int, int>>()), std::make_pair(1, 2));

    EXPECT_EQ(map.at(1, 2).get<int>(), 8);
    EXPECT_EQ((map.at(1, 2, 3.1).get<int>()), 1);
    EXPECT_EQ((map.at(1, 2, 3.1, 4.2, "g").get<std::pair<int, int>>()), std::make_pair(1, 2));
    EXPECT_THROW((void)map.at(1, 2, 8), std::out_of_range);
    EXPECT_THROW((void)map.at(1, 2, 3.1, 4.2, "g", "g"), std::out_of_range);

    EXPECT_EQ(const_map.at(1).get<std::string>(), "a");
    EXPECT_THROW((void)const_map.at(999), std::out_of_range);

    EXPECT_EQ(const_map.at(1).at(2).get<int>(), 8);
    EXPECT_THROW((void)const_map.at(1).at(2).at(8), std::out_of_range);

    EXPECT_EQ(const_map.at(1, 2).get<int>(), 8);
    EXPECT_THROW((void)const_map.at(1, 2, 8), std::out_of_range);
}

TEST_F(PolyMapTest, subscript)
{
    EXPECT_EQ(map[1].get<std::string>(), "a");
    EXPECT_EQ(map[1][2][3.1]["f"].get<int>(), 199);
}

TEST_F(PolyMapTest, get)
{
    EXPECT_EQ(map[1].get<std::string>(), "a");
    EXPECT_FALSE(map[1].empty());
    EXPECT_TRUE(map[99].empty());
    EXPECT_EQ(map[1][2][3.1].get<int>(), 1);
    EXPECT_EQ(map.at(1).at(2).at(3.1).get<int>(), 1);
    EXPECT_THROW((void)map[1].get<int>(), std::bad_cast);

    EXPECT_EQ(const_map.at(1).at(2).at(3.1).get<int>(), 1);
}

struct map_visitor {
    std::vector<std::any> keys;
    std::vector<msd::poly_map_value> values;

    template <typename V, typename M>
    bool operator()(double key, V& value, M&)
    {
        keys.emplace_back(key);
        values.emplace_back(value);
        return true;
    }

    template <typename V, typename M>
    bool operator()(const std::string& key, V& value, M&)
    {
        keys.emplace_back(key);
        values.emplace_back(value);
        return true;
    }

    template <typename K, typename V, typename M>
    bool operator()(K& key, V& value, M&)
    {
        keys.emplace_back(key);
        values.emplace_back(value);
        return true;
    }
};

TEST_F(PolyMapTest, for_each)
{
    auto visitor = map_visitor{};
    map.for_each(visitor);

    EXPECT_EQ(visitor.keys.size(), 6);
    EXPECT_EQ(std::any_cast<int>(visitor.keys[0]), 1);
    EXPECT_EQ(std::any_cast<int>(visitor.keys[1]), 2);
    EXPECT_NEAR(std::any_cast<double>(visitor.keys[2]), 3.1, eps);
    EXPECT_NEAR(std::any_cast<double>(visitor.keys[3]), 4.2, eps);
    EXPECT_EQ(std::any_cast<std::string>(visitor.keys[4]), "g");
    EXPECT_EQ(std::any_cast<std::string>(visitor.keys[5]), "f");

    EXPECT_EQ(visitor.values.size(), 6);
    EXPECT_EQ(visitor.values[0].get<std::string>(), "a");
    EXPECT_EQ(visitor.values[1].get<int>(), 8);
    EXPECT_EQ(visitor.values[2].get<int>(), 1);
    EXPECT_TRUE(visitor.values[3].empty());
    EXPECT_EQ((visitor.values[4].get<std::pair<int, int>>()), std::make_pair(1, 2));
    EXPECT_EQ(visitor.values[5].get<int>(), 199);

    auto const_visitor = map_visitor{};
    const_map.for_each(const_visitor);

    EXPECT_EQ(const_visitor.keys.size(), 6);
    EXPECT_EQ(const_visitor.values.size(), 6);
}

struct stop_visitor {
    std::size_t key_count;
    std::size_t value_count;

    template <typename K, typename V, typename M>
    auto operator()(K&, V&, M&)
    {
        ++key_count;
        ++value_count;
        return false;
    }
};

TEST_F(PolyMapTest, for_each_stop)
{
    auto visitor = stop_visitor{};
    map.for_each(visitor);

    EXPECT_EQ(visitor.key_count, 1);
    EXPECT_EQ(visitor.value_count, 1);

    auto const_visitor = stop_visitor{};
    const_map.for_each(const_visitor);

    EXPECT_EQ(const_visitor.key_count, 1);
    EXPECT_EQ(const_visitor.value_count, 1);
}

struct passed_map_visitor {
    const msd::poly_map<int, double, std::string>& test_map;

    template <typename V, typename M>
    bool operator()(const std::string& key, V&, M& map)
    {
        if (key == "f") {
            auto passed_map_ptr = static_cast<void*>(&map);
            auto test_map_ptr = static_cast<void*>(&test_map.at(1).at(2).at(3.1));
            EXPECT_EQ(passed_map_ptr, test_map_ptr);

            auto passed_map_value = map[key].template get<int>();
            auto test_map_value = test_map.at(1).at(2).at(3.1).at(key).get<int>();
            EXPECT_EQ(passed_map_value, test_map_value);
        }

        if (key == "g") {
            auto passed_map_ptr = static_cast<void*>(&map);
            auto test_map_ptr = static_cast<void*>(&test_map.at(1).at(2).at(3.1).at(4.2));
            EXPECT_EQ(passed_map_ptr, test_map_ptr);

            auto passed_map_value = map[key].template get<std::pair<int, int>>();
            auto test_map_value = test_map.at(1).at(2).at(3.1).at(4.2).at(key).get<std::pair<int, int>>();
            EXPECT_EQ(passed_map_value, test_map_value);
        }

        return true;
    }

    template <typename K, typename V, typename M>
    bool operator()(K&, V&, M&)
    {
        return true;
    }
};

TEST_F(PolyMapTest, for_each_map_passed_to_visitor)
{
    map[1][2].for_each(passed_map_visitor{map});
    const_map.at(1).at(2).for_each(passed_map_visitor{map});
}

struct element_visitor {
    std::vector<std::any>& keys;

    template <typename K, typename V, typename M>
    auto operator()(const K& key, V&, M&)
    {
        keys.emplace_back(key);
        return true;
    }
};

TEST_F(PolyMapTest, for_each_element)
{
    std::vector<std::any> keys;

    map[1][2][3.1].for_each(element_visitor{keys});
    EXPECT_EQ(keys.size(), 3);
    EXPECT_NEAR(std::any_cast<double>(keys[0]), 4.2, eps);
    EXPECT_EQ(std::any_cast<std::string>(keys[1]), "g");
    EXPECT_EQ(std::any_cast<std::string>(keys[2]), "f");

    keys.clear();
    map.at(1).at(2).at(3.1).for_each(element_visitor{keys});
    EXPECT_EQ(keys.size(), 3);
    EXPECT_NEAR(std::any_cast<double>(keys[0]), 4.2, eps);
    EXPECT_EQ(std::any_cast<std::string>(keys[1]), "g");
    EXPECT_EQ(std::any_cast<std::string>(keys[2]), "f");

    keys.clear();
    const_map.at(1).at(2).for_each(element_visitor{keys});
    EXPECT_EQ(keys.size(), 4);
    EXPECT_NEAR(std::any_cast<double>(keys[0]), 3.1, eps);
    EXPECT_NEAR(std::any_cast<double>(keys[1]), 4.2, eps);
    EXPECT_EQ(std::any_cast<std::string>(keys[2]), "g");
    EXPECT_EQ(std::any_cast<std::string>(keys[3]), "f");
}

TEST_F(PolyMapTest, for_each_stop_element)
{
    auto visitor = stop_visitor{};
    map[1].for_each(visitor);

    EXPECT_EQ(visitor.key_count, 1);
    EXPECT_EQ(visitor.value_count, 1);

    auto const_visitor = stop_visitor{};
    const_map.for_each(const_visitor);

    EXPECT_EQ(const_visitor.key_count, 1);
    EXPECT_EQ(const_visitor.value_count, 1);
}

TEST_F(PolyMapTest, empty)
{
    EXPECT_FALSE(map.empty());
    EXPECT_TRUE(map[1][2][3.1][4.2]["f"].empty());
    EXPECT_FALSE(map[1][2][3.1][4.2].empty());
    EXPECT_TRUE(map.at(1).at(2).at(3.1).at(4.2).at("f").empty());
    EXPECT_FALSE(map.at(1).at(2).at(3.1).at(4.2).empty());

    EXPECT_FALSE(const_map.empty());
    EXPECT_TRUE(const_map.at(1).at(2).at(3.1).at(4.2).at("f").empty());
    EXPECT_FALSE(const_map.at(1).at(2).at(3.1).at(4.2).empty());

    msd::poly_map<int> empty_map;
    EXPECT_TRUE(empty_map.empty());
}

TEST_F(PolyMapTest, size)
{
    EXPECT_EQ(map.size(), 6);
    EXPECT_EQ(const_map.size(), 6);
    EXPECT_EQ(map[999].size(), 0);

    EXPECT_EQ(map[1][2][3.1].size(), 3);
    EXPECT_EQ(const_map.at(1).at(2).at(3.1).size(), 3);
    EXPECT_EQ(map.at(999).size(), 0);

    msd::poly_map<int> empty_map;
    EXPECT_EQ(empty_map.size(), 0);
}

TEST_F(PolyMapTest, clear)
{
    EXPECT_TRUE(map.contains(1, 2));
    map[1].clear();
    EXPECT_FALSE(map.contains(1, 2));

    EXPECT_TRUE(map.contains(1));
    map.clear();
    EXPECT_FALSE(map.contains(1));
}

TEST_F(PolyMapTest, contains)
{
    EXPECT_TRUE(map.contains(1));
    EXPECT_TRUE(map.contains(1, 2));
    EXPECT_TRUE(map.contains(1, 2, 3.1));
    EXPECT_TRUE(map.contains(1, 2, 3.1, 4.2, "g"));
    EXPECT_FALSE(map.contains(1, 2, 3.1, 4.2, "g", "x"));
    EXPECT_FALSE(map.contains(9));
    EXPECT_FALSE(map.contains(1, 9));
    EXPECT_FALSE(map.contains(1, 2, 3.1, 9));
    EXPECT_FALSE(map.contains(2, 1));
    EXPECT_FALSE(map.contains(3.1, 2, 1));
    EXPECT_FALSE(map[1][2].contains(2));
    EXPECT_TRUE(map[1][2].contains(3.1));
    EXPECT_FALSE(map.at(1).at(2).contains(2));
    EXPECT_TRUE(map.at(1).at(2).contains(3.1));

    EXPECT_TRUE(const_map.contains(1));
    EXPECT_FALSE(const_map.at(1).at(2).contains(2));
    EXPECT_TRUE(const_map.at(1).at(2).contains(3.1, 4.2));
}
