// Copyright (C) 2021 Andrei Avram

#ifndef MSD_POLY_MAP_HPP_
#define MSD_POLY_MAP_HPP_

#include <any>
#include <functional>
#include <map>
#include <variant>

namespace msd {

template <typename... Keys>
class poly_map {
    static_assert(sizeof...(Keys) > 0, "No key type provided");

   public:
    /**
     * Checked access to map by key.
     *
     * @tparam T Type of key.
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    [[nodiscard]] auto& at(const T& key)
    {
        return items_.template at(key);
    }

    /**
     * Checked access to map by key. Const overload.
     *
     * @tparam T Type of key.
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    [[nodiscard]] auto& at(const T& key) const
    {
        return const_cast<poly_map*>(this)->at(key);
    }

    /**
     * Unchecked access to map by key.
     *
     * @tparam T Type of key.
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    auto& operator[](const T& key)
    {
        return items_[key];
    }

    /**
     * Iterate over map element by given visitor.
     *
     * @param visitor Visitor with overloads for keys types.
     */
    template <typename V>
    void for_each(V&& visitor)
    {
        for (auto& item : items_.items_) {
            auto visit =
                std::bind(std::ref(visitor), std::placeholders::_1, std::ref(item.second.value_), std::ref(items_));
            if (!std::visit(visit, item.first)) {
                return;
            }
        }
        items_.for_each(std::forward<V>(visitor));
    }

    /**
     * Iterate over map element by given visitor. Const overload.
     *
     * @param visitor Visitor with overloads for keys types.
     */
    template <typename V>
    void for_each(V&& visitor) const
    {
        const_cast<poly_map*>(this)->for_each(std::forward<V>(visitor));
    }

    /**
     * Tests if map has no elements.
     */
    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }

    /**
     * Removes elements from map.
     */
    void clear() noexcept { items_.items_.clear(); }

    /**
     * Tests if given path of keys is in map.
     *
     * @param key Key to search for.
     * @param keys List of keys to search for.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] bool contains(const T& key, const Ts&... keys) const
    {
        return items_.contains(key, keys...);
    }

   private:
    struct poly_map_item;
    poly_map_item items_;
};

/**
 * Map value
 */
struct value {
    std::any value_;

    /**
     * Get value from map.
     *
     * @tparam T Type to cast value to.
     *
     * @return Casted value.
     *
     * @throws std::bad_cast if cannot cast stored value to given type.
     */
    template <typename T>
    [[nodiscard]] auto get() const
    {
        try {
            return std::any_cast<T>(value_);
        }
        catch (const std::bad_any_cast&) {
            throw std::bad_cast{};
        }
    }

    /**
     * Tests if value has been set.
     */
    [[nodiscard]] bool empty() const noexcept { return !value_.has_value(); }
};

/**
 * Child map
 *
 * @tparam Keys Types of keys.
 */
template <typename... Keys>
struct poly_map<Keys...>::poly_map_item {
    std::map<std::variant<Keys...>, poly_map_item> items_;
    value value_;

    /**
     * Add value to map.
     *
     * @tparam T Type of value.
     * @param v Value to add.
     */
    template <typename T>
    auto& operator=(T&& v) noexcept
    {
        value_.value_ = std::forward<T>(v);
        return *this;
    }

    /**
     * Checked access to map by key.
     *
     * @tparam T Type of key.
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    [[nodiscard]] auto& at(const T& key)
    {
        return items_.at(key);
    }

    /**
     * Unchecked access to map by key.
     *
     * @tparam T Type of key.
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    auto& operator[](const T key)
    {
        return items_[key];
    }

    /**
     * Get value from map.
     *
     * @tparam T Type to cast value to.
     *
     * @return Casted value.
     *
     * @throws std::bad_cast if cannot cast stored value to given type.
     */
    template <typename T>
    [[nodiscard]] auto get() const
    {
        return value_.template get<T>();
    }

    /**
     * Iterate over map element by given visitor.
     *
     * @param visitor Visitor with overloads for keys types.
     */
    template <typename V>
    void for_each(V&& visitor)
    {
        for (auto& it : items_) {
            for (auto& item : it.second.items_) {
                auto visit = std::bind(std::ref(visitor), std::placeholders::_1, std::ref(item.second.value_),
                                       std::ref(it.second.items_));
                if (!std::visit(visit, item.first)) {
                    return;
                }
            }
            it.second.for_each(std::forward<V>(visitor));
        }
    }

    /**
     * Iterate over map element by given visitor. Const overload.
     *
     * @param visitor Visitor with overloads for keys types.
     */
    template <typename V>
    void for_each(V&& visitor) const
    {
        const_cast<poly_map_item*>(this)->for_each(std::forward<V>(visitor));
    }

    /**
     * Tests if given key is in map.
     *
     * @param key Key to search for.
     */
    template <typename T>
    [[nodiscard]] bool contains(const T& key) const
    {
        return items_.find(key) != items_.end();
    }

    /**
     * Tests if map has no elements.
     */
    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }

    /**
     * Removes elements from map.
     */
    void clear() noexcept { items_.clear(); }

    /**
     * Tests if given path of keys is in map.
     *
     * @param key Key to search for.
     * @param keys List of keys to search for.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] bool contains(const T& key, const Ts&... keys) const
    {
        if (!contains(key)) {
            return false;
        }
        return items_.at(key).contains(keys...);
    }
};

}  // namespace msd

#endif  // MSD_POLY_MAP_HPP_
