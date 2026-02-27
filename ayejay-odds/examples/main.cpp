#include <iostream>

#include <ayejay/odds.hpp>
// If you enable modules and your toolchain supports them, you can instead:
// import ayejay.odds;

int main()
{
    using namespace ayejay::odds;

    seed_thread(1337);

    std::size_t hits = 0;
    constexpr std::size_t trials = 1'000'000;

    for (std::size_t i = 0; i < trials; ++i)
    {
        if (p100()) // 1%
            ++hits;
    }

    std::cout << "1 in 100 hits: " << hits << "\n";

    if (one_in(37u))
        std::cout << "Lucky 37 triggered.\n";

    return 0;
}
