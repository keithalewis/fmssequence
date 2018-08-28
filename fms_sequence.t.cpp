// fms_sequence.t.cpp - test sequences
#include <cassert>
#include <cmath>
#include <chrono>
#include "fms_sequence.h"

using namespace fms;

inline auto time(const std::function<void(void)>& f, size_t n = 1)
{
    std::chrono::time_point<std::chrono::steady_clock> tp;

    tp = std::chrono::high_resolution_clock::now();
    while (n--)
        f();
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - tp;

    return duration;
}

template<class T>
void test_array()
{
    {
        T t[] = { 1,2,3 };
        auto s = sequence::array(3, t);

        assert(s);
        assert(*s == 1);
        ++s;
        assert(s);
        assert(*s == 2);
        ++s;
        assert(s);
        assert(*s == 3);
        ++s;
        assert(!s);
        ++s;
        assert(!s); 
    }
    {
        T t[] = { 1,2,3 };
        auto s = sequence::array(t);

        assert(s);
        assert(*s == 1);
        ++s;
        assert(s);
        assert(*s == 2);
        ++s;
        assert(s);
        assert(*s == 3);
        ++s;
        assert(!s);
        ++s;
        assert(!s);
    }
    {
        std::array<T,3> t = { 1,2,3 };
        auto s = sequence::array(t);

        assert(s);
        assert(*s == 1);
        ++s;
        assert(s);
        assert(*s == 2);
        ++s;
        assert(s);
        assert(*s == 3);
        ++s;
        assert(!s);
        ++s;
        assert(!s);
    }
}

template<class T>
void test_null()
{
    T t[] = { 1,2,3,0 };
    sequence::null<T> s(t);

    sequence::null<T> s2(s);
    assert(s2 == s);
    assert(s == s2);
    assert(!(s2 != s));

    s = s2;
    assert(s2 == s);
    assert(s == s2);
    assert(!(s2 != s));

    assert(s);
    assert(*s == 1);
    ++s;
    assert(s);
    assert(*s == 2);
    ++s;
    assert(s);
    assert(*s == 3);
    ++s;
    assert(!s);
}

void test_constant()
{
    sequence::constant<int> five(5);
    sequence::constant<int> five_(five);
    assert(five == five_);
    five = five_;
    assert(five == five_);
    assert(five);
    assert(*five == 5);
    ++five;
    assert(five);
    assert(*five == 5);
}

void test_drop()
{
    int t[] = { 1,2,3 };
    auto s = sequence::array(t);

    {
        assert(3 == length(s));
        assert(2 == length(drop(1, s)));
        auto s1 = drop(1, s);
        assert(s1);
        assert(*s1 == 2);
        ++s1;
        assert(s1);
        assert(*s1 == 3);
        ++s1;
        assert(!s1);
        ++s1;
        assert(!s1);
        assert(0 == length(drop(10, s)));
    }
}

void test_iota()
{
    {
        sequence::generate<double, std::plus<double>> s(0);
        assert(*s == 0);
        ++s;
        assert(*s == 1);
        ++s;
        assert(*s == 2);
        ++s;
        assert(*s == 3);
    }
    {
        sequence::linear<double> s(0);
        assert(*s == 0);
        ++s;
        assert(*s == 1);
        ++s;
        assert(*s == 2);
        ++s;
        assert(*s == 3);
    }
    {
        sequence::generate<double> s(0);
        sequence::generate<double> s2(s);
        assert(s == s2);
        s = s2;
        assert(s == s2);
        assert(*s == 0);
        ++s;
        assert(*s == 1);
        ++s;
        assert(*s == 2);
        ++s;
        assert(*s == 3);
    }
    {
        sequence::generate<double, std::multiplies<double>> s(1,2);
        assert(*s == 1);
        ++s;
        assert(*s == 2);
        ++s;
        assert(*s == 4);
        ++s;
        assert(*s == 8);
    }
    {
        sequence::geometric<double> s(1, 2);
        assert(*s == 1);
        ++s;
        assert(*s == 2);
        ++s;
        assert(*s == 4);
        ++s;
        assert(*s == 8);
    }
    {
        sequence::generate<double, std::multiplies<double>> s(3);
        assert(*s == 3);
        ++s;
        assert(*s == 3);
        ++s;
        assert(*s == 3);
        ++s;
        assert(*s == 3);
    }
}

void test_length()
{
    {
        auto s = sequence::geometric<double>(1, .5);
        auto se = sequence::epsilon<sequence::geometric<double>>(s);
        assert(53 == length(se));
    }
    {
        auto s = sequence::geometric<double>(1., .5);
        auto se = sequence::epsilon(s);
        assert(53 == length(se));
    }
    {
        assert(53 == length(sequence::epsilon(sequence::geometric<double>(1., .5))));
    }
    {
        int t[] = { 1,2,3 };
        assert(3 == length(sequence::array(t)));
        assert(2 == length(sequence::take(2, sequence::pointer(t))));
    }
}

template<class T>
void test_factorial()
{
    sequence::factorial<T> s;

    assert(s);
    assert(*s == 1);
    ++s;
    assert(s);
    assert(*s == 1);
    ++s;
    assert(s);
    assert(*s == 2);
    ++s;
    assert(s);
    assert(*s == 6);
    ++s;
    assert(s);
    assert(*s == 24);
    ++s;
}

void test_power()
{
    sequence::power<int> s(2);
    assert(s);
    assert(*s == 1);
    ++s;
    assert(s);
    assert(*s == 2);
    ++s;
    assert(s);
    assert(*s == 4);
    ++s;
    assert(s);
    assert(*s == 8);
    ++s;
}

void test_sum()
{
    double t[] = { 1,2,3,0 };
    sequence::null<double> s(t);
    auto hi_ = sum(s);
    assert(hi_ == 6);
}

void test_product()
{
    double t[] = { 1,2,3,0 };
    sequence::null<double> s(t);
    auto hi_ = product(s);
    assert(hi_ == 6);
}

void test_binop()
{
    {
        int t0[] = { 1,2,3 };
        int t1[] = { 3,4,5 };
        auto s0 = sequence::array(t0);
        auto s1 = sequence::array(t1);
        auto s = s0 + s1;
        decltype(s) s2{ s };
        assert(s2 == s);
        assert(!(s2 != s));
        s = s2;
        assert(s);
        assert(*s == *s0 + *s1);
        ++s;
        assert(*s == 2 + 4);
        ++s;
        assert(*s == 8);
        ++s;
        assert(!s);
    }
    {
        int t0[] = { 1,2 };
        int t1[] = { 3,4,5 };
        auto s0 = sequence::array(t0);
        auto s1 = sequence::array(t1);
        auto s = s0 + s1;
        assert(s);
        assert(*s == *s0 + *s1);
        ++s;
        assert(*s == 2 + 4);
        ++s;
        assert(!s);
        ++s;
        assert(!s);
    }
    {
        int t0[] = { 1,2,3 };
        int t1[] = { 3,4,5 };
        auto s0 = sequence::array(t0);
        auto s1 = sequence::array(t1);
        auto s = s0 - s1;
        assert(s);
        assert(*s == *s0 - *s1);
        ++s;
        assert(*s == 2 - 4);
        ++s;
        assert(*s == -2);
        ++s;
        assert(!s);
    }
    {
        int t0[] = { 1,2,3 };
        int t1[] = { 3,4,5 };
        auto s0 = sequence::array(t0);
        auto s1 = sequence::array(t1);
        auto s = s0 * s1;
        assert(s);
        assert(*s == *s0 * *s1);
        ++s;
        assert(*s == 2 * 4);
        ++s;
        assert(*s == 15);
        ++s;
        assert(!s);
    }
    {
        int t0[] = { 1,2,3 };
        int t1[] = { 3,4,5 };
        auto s0 = sequence::array(t0);
        auto s1 = sequence::array(t1);
        auto s = s0 / s1;
        assert(s);
        assert(*s == *s0 / *s1);
        ++s;
        assert(*s == 2 / 4);
        ++s;
        assert(*s == 0);
        ++s;
        assert(!s);
    }
    {
        using sequence::sum;
        using sequence::power;
        using sequence::factorial;
        using sequence::epsilon;
        using sequence::length;
        using sequence::constant;

        double x = 1;
        auto s = epsilon(power(x) / factorial<>());
        assert (19 == length(s));
        assert(exp(x) - sum(s) == -2 * std::numeric_limits<double>::epsilon());
        assert(exp(x) == sequence::horner(epsilon(constant(1) / factorial<>()), x));

        auto duration = time([s]() { return sum(s); }, 10000);
        duration = duration;
        duration = time([x]() { return sequence::horner(epsilon(constant(1) / factorial<>()), x); }, 10000);
        duration = duration;
        auto h = epsilon(constant(1) / factorial<>());
        duration = time([h,x]() { return sequence::horner(h, x); }, 10000);
        duration = duration; 
        duration = time([x]() { return exp(x); }, 10000);
        duration = duration;
    }
}

template<class T>
void test_concatenate()
{
    {
        using sequence::concatenate;
        using sequence::array;

        T a[] = { 1,2 };
        T b[] = { 3,4,5 };
        auto c = concatenate(array(a), array(b));
        assert(c);
        assert(*c == 1);
        ++c;
        assert(c);
        assert(*c == 2);

    }
}

int main()
{
    test_array<int>();
    test_array<char>();
    test_array<float>();
    test_array<double>();

    test_array<const int>();
    test_array<const float>();
    test_array<const double>();

    test_constant();

    test_null<int>();
    test_null<char>();
    test_null<float>();
    test_null<double>();

    test_iota();

    test_drop();
    test_length();
    test_sum();
    test_product();
    test_factorial<int>();

    test_binop();

    test_concatenate<int>();

    return 0;
}
