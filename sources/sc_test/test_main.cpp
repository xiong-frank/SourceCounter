
#include <tuple>
#include <iostream>

int main()
{
    using tuple_t = std::tuple<int, int>;
    tuple_t t{ 1, 2 }, t2{7, 11};

    auto x = t + t2;

    auto& [a, b] = t;

    ++a;
    b += 2;

    std::cout << "a: " << std::get<0>(t) << ", b: " << std::get<1>(t) << std::endl;

    return 0;
}