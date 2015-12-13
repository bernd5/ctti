//
// Created by manu343726 on 4/08/15.
//

#ifndef CTTI_HASH_HPP
#define CTTI_HASH_HPP

#include <functional>
#include "detail/hash.hpp"
#include "detail/pretty_function.hpp"
#include "detail/string.hpp"

namespace ctti
{
    struct type_id_t
    {
        constexpr type_id_t(const detail::string& name) :
            name_{name}
        {}

        type_id_t& operator=(const type_id_t&) = default;

        constexpr detail::hash_t hash() const
        {
            return name_.hash();
        }

        // note: name().c_str() isn't null-terminated properly!
        constexpr detail::string name() const
        {
            return name_;
        }

        friend constexpr bool operator==(const type_id_t& lhs, const type_id_t& rhs)
        {
            return lhs.hash() == rhs.hash();
        }

        friend constexpr bool operator!=(const type_id_t& lhs, const type_id_t& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        detail::string name_;
    };

    struct unnamed_type_id_t
    {
        constexpr unnamed_type_id_t(const detail::hash_t hash) :
            _hash{hash}
        {}

        unnamed_type_id_t& operator=(const unnamed_type_id_t&) = default;

        constexpr detail::hash_t hash() const
        {
			return _hash;
        }

        friend constexpr bool operator==(const unnamed_type_id_t& lhs, const unnamed_type_id_t& rhs)
        {
            return lhs.hash() == rhs.hash();
        }

        friend constexpr bool operator!=(const unnamed_type_id_t& lhs, const unnamed_type_id_t& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        detail::hash_t _hash;
    };

    using type_index = unnamed_type_id_t; // To mimic std::type_index when using maps

    namespace detail
    {
        template<typename T>
        constexpr ctti::type_id_t type_id()
        {
            static_assert(CTTI_PRETTY_FUNCTION_END - CTTI_PRETTY_FUNCTION_BEGIN <= max_string_length, CTTI_PRETTY_FUNCTION);

            // one-liner required by MSVC :(
            return detail::make_string<CTTI_PRETTY_FUNCTION_BEGIN, CTTI_PRETTY_FUNCTION_END>(CTTI_PRETTY_FUNCTION);
        }

        template<typename T>
        constexpr ctti::unnamed_type_id_t unnamed_type_id()
        {
            // one-liner required by MSVC :(
            return sid_hash(CTTI_PRETTY_FUNCTION_END - CTTI_PRETTY_FUNCTION_BEGIN, CTTI_PRETTY_FUNCTION + CTTI_PRETTY_FUNCTION_BEGIN);
        }
    }

    /**
     * Returns type information at compile-time for a value
     * of type T. Decay is applied to argument type first, use
     * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
     */
    template<typename T>
    constexpr type_id_t type_id(T&&)
    {
        return detail::type_id<typename std::decay<T>::type>();
    }

    /**
     * Returns type information at compile-time for type T.
     */
    template<typename T>
    constexpr type_id_t type_id()
    {
        return detail::type_id<T>();
    }

    /**
     * Returns unnamed type information at compile-time for a value
     * of type T. Decay is applied to argument type first, use
     * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
     */
    template<typename T>
    constexpr unnamed_type_id_t unnamed_type_id(T&&)
    {
        return detail::unnamed_type_id<typename std::decay<T>::type>();
    }

    /**
     * Returns unnamed type information at compile-time for type T.
     */
    template<typename T>
    constexpr unnamed_type_id_t unnamed_type_id()
    {
        return detail::unnamed_type_id<T>();
    }

    //assert commented out, GCC 5.2.0 ICE here.
    //static_assert(type_id<void>() == type_id<void>(), "ctti::type_id_t instances must be constant expressions");
}

namespace std
{
    template<>
    struct hash<ctti::type_id_t>
    {
        constexpr std::size_t operator()(const ctti::type_id_t& id) const
        {
            // quiet warning about possible loss of data
            return std::size_t(id.hash());
        }
    };
}

#endif //CTTI_HASH_HPP
