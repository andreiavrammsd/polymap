// Copyright (C) 2020 Andrei Avram

#ifndef POLY_MAP_HPP_
#define POLY_MAP_HPP_

#include <any>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <variant>

struct value {
    std::any value_;

    template <typename T>
    [[nodiscard]] auto get() const
    {
        return std::any_cast<T>(value_);
    }

    [[nodiscard]] bool empty() const noexcept { return !value_.has_value(); }
};

template <typename... Keys>
struct poly_map_item {
    std::map<std::variant<Keys...>, poly_map_item> items_;
    value value_;

    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }

    template <typename T>
    auto& operator[](const T key)
    {
        return items_[key];
    }

    template <typename T>
    auto& at(const T& key)
    {
        return items_.at(key);
    }

    template <typename T>
    [[nodiscard]] auto& at(const T& key) const
    {
        return items_.at(key);
    }

    template <typename T>
    auto& operator=(T&& v) noexcept
    {
        value_.value_ = std::forward<T>(v);
        return *this;
    }

    template <typename T>
    [[nodiscard]] auto get() const
    {
        try {
            return std::any_cast<T>(value_.value_);
        }
        catch (const std::bad_any_cast&) {
            throw std::bad_cast{};
        }
    }

    template <typename T>
    [[nodiscard]] auto operator()() const
    {
        try {
            return std::any_cast<T>(value_.value_);
        }
        catch (const std::bad_any_cast&) {
            throw std::bad_cast{};
        }
    }

    template <typename T>
    [[nodiscard]] bool contains(const T& key) const
    {
        return items_.find(key) != items_.end();
    }

    template <typename T, typename... Ts>
    [[nodiscard]] bool contains(const T& key, const Ts&... keys) const
    {
        if (items_.find(key) == items_.end()) {
            return false;
        }
        return items_.at(key).contains(keys...);
    }

    void clear() noexcept { items_.clear(); }

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

    template <typename V>
    void for_each(V&& visitor) const
    {
        const_cast<poly_map_item*>(this)->for_each(std::forward<V>(visitor));
    }
};

template <typename... Keys>
class poly_map {
    static_assert(sizeof...(Keys) > 0, "no key type provided");

   public:
    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }

    template <typename T>
    auto& operator[](const T& key)
    {
        return items_[key];
    }

    template <typename T>
    auto& at(const T& key)
    {
        return items_.template at(key);
    }

    template <typename T>
    [[nodiscard]] auto& at(const T& key) const
    {
        return items_.template at(key);
    }

    template <typename T, typename... Ts>
    [[nodiscard]] bool contains(const T& key, const Ts&... keys) const
    {
        return items_.contains(key, keys...);
    }

    void clear() noexcept { items_.items_.clear(); }

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

    template <typename V>
    void for_each(V&& visitor) const
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

   private:
    poly_map_item<Keys...> items_;
};

#endif  // POLY_MAP_HPP_
