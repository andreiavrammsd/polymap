// Copyright (C) 2021 Andrei Avram

#ifndef MSD_POLY_MAP_HPP_
#define MSD_POLY_MAP_HPP_

#include <any>
#include <cstdlib>
#include <map>
#include <utility>
#include <variant>

namespace msd {

/**
 * Polymorphic map
 *
 * @tparam Key First type of key.
 * @tparam Keys Other types of keys.
 */
template <typename Key, typename... Keys>
class poly_map {
    struct poly_map_element;

   public:
    /**
     * Checked access by key.
     *
     * @tparam T Type of key.
     *
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    [[nodiscard]] poly_map_element& at(const T& key)
    {
        return elements_.at(key);
    }

    /**
     * Checked access by list of keys.
     *
     * @tparam T Type of first key.
     * @tparam Ts Types of other keys.
     *
     * @param key First key to index map by.
     * @param keys Other keys.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] auto& at(const T& key, const Ts&... keys)
    {
        return elements_.at(key).at(keys...);
    }

    /**
     * Checked access by key. Const overload.
     *
     * @tparam T Type of key.
     *
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] auto& at(const T& key, const Ts&... keys) const
    {
        return const_cast<poly_map*>(this)->at(key, keys...);
    }

    /**
     * Unchecked access by key.
     *
     * @tparam T Type of key.
     *
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    auto& operator[](const T& key)
    {
        return elements_[key];
    }

    /**
     * Iterate over map element by given visitor.
     *
     * @param visitor Visitor with overloads for keys types.
     */
    template <typename V>
    void for_each(V&& visitor)
    {
        elements_.for_each(std::forward<V>(visitor));
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
    [[nodiscard]] bool empty() const noexcept { return elements_.empty(); }

    /**
     * Returns number of elements.
     */
    [[nodiscard]] std::size_t size() const noexcept { return elements_.size(); }

    /**
     * Removes elements from map.
     */
    void clear() noexcept { elements_.elements_.clear(); }

    /**
     * Tests if given path of keys is in map.
     *
     * @param key Key to search for.
     * @param keys List of keys to search for.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] bool contains(const T& key, const Ts&... keys) const
    {
        return elements_.contains(key, keys...);
    }

   private:
    poly_map_element elements_;
};

/**
 * Map value
 */
struct poly_map_value {
    /**
     * Set value in map.
     *
     * @param value Value to set.
     *
     * @throws std::bad_alloc or the exception thrown by the assigned value's constructor.
     */
    template <typename T>
    void set(T&& value)
    {
        value_ = std::forward<T>(value);
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

   private:
    std::any value_;
};

/**
 * Child map
 *
 * @tparam Key First type of key.
 * @tparam Keys Other types of keys.
 */
template <typename Key, typename... Keys>
struct poly_map<Key, Keys...>::poly_map_element {
    std::map<std::variant<Key, Keys...>, poly_map_element> elements_;

    /**
     * Assign value to map.
     *
     * @param v Value to add.
     *
     * @throws std::bad_alloc or the exception thrown by the assigned value's constructor.
     */
    template <typename T>
    auto& operator=(T&& v)
    {
        value_.set(std::forward<T>(v));

        return *this;
    }

    /**
     * Checked access by key.
     *
     * @tparam T Type of key.
     *
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    [[nodiscard]] auto& at(const T& key)
    {
        return elements_.at(key);
    }

    /**
     * Checked access by list of keys.
     *
     * @tparam T Type of first key.
     * @tparam Ts Types of other keys.
     *
     * @param key First key to index map by.
     * @param keys Other keys.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T, typename... Ts>
    [[nodiscard]] poly_map_element& at(const T& key, const Ts&... keys)
    {
        return elements_.at(key).at(keys...);
    }

    /**
     * Unchecked access by key.
     *
     * @tparam T Type of key.
     *
     * @param key Key to index map by.
     *
     * @return Map at given key.
     *
     * @throws std::out_of_range if key not found.
     */
    template <typename T>
    auto& operator[](const T key)
    {
        return elements_[key];
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
        for (auto& element : elements_) {
            const auto visit = [this, &visitor, &element](auto& key) {
                return visitor(key, element.second.value_, elements_);
            };

            if (!std::visit(visit, element.first)) {
                return;
            }

            element.second.for_each(std::forward<V>(visitor));
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
        const_cast<poly_map_element*>(this)->for_each(std::forward<V>(visitor));
    }

    /**
     * Tests if map has no elements.
     */
    [[nodiscard]] bool empty() const noexcept { return elements_.empty(); }

    /**
     * Returns number of elements.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        std::size_t sz = elements_.size();

        for (auto& element : elements_) {
            sz += element.second.size();
        }

        return sz;
    }

    /**
     * Removes elements from map.
     */
    void clear() noexcept { elements_.clear(); }

    /**
     * Tests if given key is in map.
     *
     * @param key Key to search for.
     */
    template <typename T>
    [[nodiscard]] bool contains(const T& key) const
    {
        return elements_.find(key) != elements_.end();
    }

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

        return elements_.at(key).contains(keys...);
    }

   private:
    poly_map_value value_;
};

}  // namespace msd

#endif  // MSD_POLY_MAP_HPP_
