#pragma once

#include <vector>
#include <algorithm>

namespace vrock::utils
{
    template <class T>
    class Queryable
    {
    public:
        virtual auto select(std::function<bool(T)>) -> Queryable<T> = 0;
    };

    template <class T>
    class List : Queryable<T>
    {
    public:
        auto select(std::function<bool(T)> fn) -> Queryable<T> final
        {
            auto vec = std::vector<T>();
            std::copy_if(vec_.begin(), vec_.end(), vec.begin(), fn);
        }

    private:
        std::vector<T> vec_;
    };
} // namespace vrock::utils
