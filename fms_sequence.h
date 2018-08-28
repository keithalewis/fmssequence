// fms_sequence.h - forward iterators that can be dereferenced when operator bool() const is true
#pragma once
#include <array>
#include <functional>
#include <tuple>
#include <typeinfo>
#include <type_traits>

namespace fms::sequence {

    // unsafe sequence
    template <class T>
    class pointer {
        T* t;
    public:
	    using value_type = T;
        pointer(T* t = nullptr) noexcept
            : t{ t }
        { }
        // same pointer
        bool operator==(const pointer& s) const
        {
            return t == s.t;
        }
        bool operator!=(const pointer& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return true;
        }
        pointer& operator++()
        {
            ++t;

            return *this;
        }
        value_type operator*() const
        {
            return *t;
        }
    };

    // limit sequence to size n
    template<class S>
    class take {
        size_t n;
        S s;
    public:
        using value_type = typename S::value_type;
        take(size_t n, S s)
            : n{ n }, s{ s }
        { }
        // same sequence pointer and size
        bool operator==(const take& s) const
        {
            return this->n == s.n && this->s == s.s;
        }
        bool operator!=(const take& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return n != 0;
        }
        take& operator++()
        {
            if (*this) {
                --n;
                ++s;
            }

            return *this;
        }
        value_type operator*() const
        {
            return *s;
        }
    };

    template<class T>
    inline auto array(size_t n, T* t)
    {
        return take(n, pointer<T>(t));
    }
    template<size_t N, class T>
    inline auto array(T(&t)[N])
    {
        return take(N, pointer<T>(&t[0]));
    }
    template<size_t N, class T>
    inline auto array(std::array<T, N>& t)
    {
        return take(N, pointer<T>(&t[0]));
    }

    template <class T = double>
    class constant {
        T t;
    public:
        using value_type = T;
        constant(T t = 0) noexcept
            : t(t)
        {
        }
        bool operator==(const constant& s) const
        {
            return t == s.t;
        }
        bool operator!=(const constant& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return true;
        }
        constant& operator++()
        {
            return *this;
        }
        value_type operator*() const
        {
            return t;
        }
    };

    // epsilon terminated array
    template <class S = double>
    class epsilon {
        S s;
    public:
        using value_type = typename S::value_type;
        epsilon(S s)
            : s{ s }
        { }
        // same pointer
        bool operator==(const epsilon& s) const
        {
            return s == s.s;
        }
        bool operator!=(const epsilon& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return *s + 1 != 1;
        }
        epsilon& operator++()
        {
            if (*this) {
                ++s;
            }

            return *this;
        }
        value_type operator*() const
        {
            return *s;
        }
    };

    template<class T = double>
    class factorial {
        T n_, n;
    public:
        using value_type = T;
        factorial() noexcept
            : n_{ 1 }, n{ 0 }
        { }
        bool operator==(const factorial& s) const
        {
            return n_ == s.n_;
        }
        bool operator!=(const factorial& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return true;
        }
        factorial& operator++()
        {
            n_ *= ++n;

            return *this;
        }
        value_type operator*() const
        {
            return n_;
        }
    };

    // t0, op(t0, dt), op(op(t0, dt), dt), ...
    template <class T, class Op = std::plus<T>>
    class generate {
        T t0, dt;
        Op op;
    public:
        using value_type = T;
        generate(T t0, T dt = 1) noexcept
            : t0(t0), dt(dt), op(Op{})
        { }
        bool operator==(const generate& s)
        {
            return t0 == s.t0 && dt == s.dt && typeid(op) == typeid(s.op);
        }
        bool operator!=(const generate& s) const
        {
            return !operator==(s);
        }
        operator bool() const 
        { 
            return false;
        }
        generate& operator++()
        {
            t0 = op(t0, dt);

            return *this;
        }
        value_type operator*() const 
        { 
            return t0; 
        }
    };
    template<class T>
    using linear = generate<T, std::plus<T>>;
    template<class T>
    using geometric = generate<T, std::multiplies<T>>;

    // null terminated array
    template <class T>
    class null {
        T* t;
    public:
        typedef T value_type;
        null(T* t = nullptr) noexcept
            : t{ t }
        { }
        // same pointer
        bool operator==(const null& s) const
        {
            return t == s.t;
        }
        bool operator!=(const null& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return *t != 0;
        }
        null& operator++()
        {
            if (*this) {
                ++t;
            }

            return *this;
        }
        value_type operator*() const
        {
            return *t;
        }
    };

    // 1 = t^0, t = t^1, t^2, ...
    template<class T = double>
    class power {
        T tn, t;
    public:
        typedef T value_type;
        power(T t) noexcept
            : tn{ 1 }, t{ t }
        { }
        bool operator==(const power& s) const
        {
            return  t == t && tn == s.tn;
        }
        bool operator!=(const power& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return true;
        }
        power& operator++()
        {
            tn *= t;

            return *this;
        }
        value_type operator*() const
        {
            return tn;
        }
    };

    template<class Op, class S0, class S1>
    class binop {
        Op op;
        S0 s0;
        S1 s1;
    public:
        typedef std::common_type_t<typename S0::value_type, typename S1::value_type> arg_type;
        typedef typename std::invoke_result_t<Op, arg_type, arg_type> value_type;
        binop(Op op, S0 s0, S1 s1)
            : op(op), s0(s0), s1(s1)
        { }
        bool operator==(const binop& s) const
        {
            return  typeid(op) == typeid(s.op) && s0 == s.s0 && s1 == s.s1;
        }
        bool operator!=(const binop& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return s0 && s1;
        }
        binop& operator++()
        {
            ++s0;
            ++s1;

            return *this;
        }
        value_type operator*() const
        {
            return op(*s0, *s1);
        }
    };

    template<class ...S>
    class concatenate {
        std::tuple<S...> s;
        size_t i; // index of active sequence
    public:
        typedef std::common_type_t<typename S::value_type...> value_type;
        concatenate(S... s)
            : s(s), i(0)
        { }
        bool operator==(const concatenate& s) const
        {
            if (this->s.size() != s.size() || this->i != s.i) {
                return false;
            }

            for (size_t j = 0; j < this->s.size(); ++i) {
                if (std::get<j>(this->s) != std::get<j>(s.s)) {
                    return false;
                }
            }

            return true;
        }
        bool operator!=(const concatenate& s) const
        {
            return !operator==(s);
        }
        operator bool() const
        {
            return i != s.size();
        }
        concatenate& operator++()
        {
            ++std::get<i>(s);
            if (!std::get<i>(s)) {
                ++i;
            }

            return *this;
        }
        value_type operator*() const
        {
            return 0;// *(std::get<i>(s));
        }
    };

    //
    // Functions
    //

    template<class S0, class S1>
    inline auto operator+(S0 s0, S1 s1)
    {
        return binop(std::plus<std::common_type_t<typename S0::value_type, typename S1::value_type>>{}, s0, s1);
    }
    template<class S0, class S1>
    inline auto operator-(S0 s0, S1 s1)
    {
        return binop(std::minus<std::common_type_t<typename S0::value_type, typename S1::value_type>>{}, s0, s1);
    }
    template<class S0, class S1>
    inline auto operator*(S0 s0, S1 s1)
    {
        return binop(std::multiplies<std::common_type_t<typename S0::value_type, typename S1::value_type>>{}, s0, s1);
    }
    template<class S0, class S1>
    inline auto operator/(S0 s0, S1 s1)
    {
        return binop(std::divides<std::common_type_t<typename S0::value_type, typename S1::value_type>>{}, s0, s1);
    }
    template<class S0, class S1>
    inline auto operator%(S0 s0, S1 s1)
    {
        return binop(std::modulus<std::common_type_t<typename S0::value_type, typename S1::value_type>>{}, s0, s1);
    }

    // s[0] + x*(s[1] + x*(...))
    template<class S>
    inline typename S::value_type horner(S s, typename S::value_type x)
    {
        return !s ? 0 : *s + x * horner(++s, x);
    }

    template <class S>
    inline S last(S s)
    {
        S s_ = s;

        while (*++s) {
            ++s_;
        }

        return s_;
    }
    template <class S>
    inline auto back(S s)
    {
        return *last(s);
    }

    template <class S>
    inline S drop(size_t n, S s)
    {
        while (s && n--)
            ++s;

        return s;
    }

    template <class S>
    inline size_t length(S s)
    {
        size_t n = 0;

        while (s) {
            ++n;
            ++s;
        }

        return n;
    }

    // same values and length
    template <class U, class V>
    inline bool same(U u, V v)
    {
        while (u && v) {
            if (*u != *v)
                return false;
            ++u;
            ++v;
        }

        return !u && !v;
    }

    template <class S>
    inline typename S::value_type sum(S s)
    {
        if (!s)
            return 0;

        typename S::value_type t = *s;

        while (++s) {
            t += *s;
        }

        return t;
    }

    template <class S>
    inline typename S::value_type product(S s)
    {
        if (!s)
            return 1;

        typename S::value_type t = *s;

        while (++s) {
            t *= *s;
        }

        return t;
    }

} // namespace fms::sequence
