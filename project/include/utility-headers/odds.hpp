#pragma once
// odds.hpp - C++23 "1 in N" odds / probability utilities
//
// Features:
// - Runtime: one_in(N)
// - Compile-time: one_in<100>()
// - Presets: ayejay::odds::p100(), p50(), p25(), p10(), p5(), p2(), etc.
// - Fast, deterministic PRNG (xoshiro256**), seedable.
// - Unbiased bounded uniform generation (no modulo bias).
//
// Notes:
// - Header-only.
// - Thread-safe by default via thread_local RNG (no locks).
// - For deterministic replay/testing, call ayejay::odds::seed_thread(...) once per thread.

#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

#if defined(_MSC_VER) && defined(_M_X64)
  #include <intrin.h>
#endif

namespace ayejay::odds
{
    // ----------------------------
    // Small utilities
    // ----------------------------
    template <class T>
    concept unsigned_int =
        std::is_unsigned_v<T> && std::is_integral_v<T>;

    [[nodiscard]] constexpr std::uint64_t rotl64(std::uint64_t x, int k) noexcept
    {
        return (x << k) | (x >> (64 - k));
    }

    // ----------------------------
    // splitmix64 - seeding generator
    // ----------------------------
    struct splitmix64 final
    {
        std::uint64_t state{};

        constexpr explicit splitmix64(std::uint64_t seed) noexcept : state(seed) {}

        [[nodiscard]] constexpr std::uint64_t next_u64() noexcept
        {
            std::uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
            z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
            z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
            return z ^ (z >> 31);
        }
    };

    // ----------------------------
    // xoshiro256** - fast PRNG
    // ----------------------------
    struct xoshiro256ss final
    {
        // State must not be all-zero.
        std::array<std::uint64_t, 4> s{
            { 0x123456789ABCDEF0ULL, 0xCAFEBABEDEADC0DEULL, 0x0F1E2D3C4B5A6978ULL, 0x1122334455667788ULL }
        };

        constexpr xoshiro256ss() noexcept = default;

        constexpr explicit xoshiro256ss(std::uint64_t seed) noexcept
        {
            seed_with(seed);
        }

        constexpr void seed_with(std::uint64_t seed) noexcept
        {
            splitmix64 sm(seed);
            s[0] = sm.next_u64();
            s[1] = sm.next_u64();
            s[2] = sm.next_u64();
            s[3] = sm.next_u64();

            // Ensure not all zero.
            if ((s[0] | s[1] | s[2] | s[3]) == 0ULL)
            {
                s[0] = 0x9E3779B97F4A7C15ULL;
                s[1] = 0xBF58476D1CE4E5B9ULL;
                s[2] = 0x94D049BB133111EBULL;
                s[3] = 0xD1B54A32D192ED03ULL;
            }
        }

        [[nodiscard]] constexpr std::uint64_t next_u64() noexcept
        {
            // xoshiro256** reference implementation (Vigna/Blackman).
            const std::uint64_t result = rotl64(s[1] * 5ULL, 7) * 9ULL;
            const std::uint64_t t = s[1] << 17;

            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];

            s[2] ^= t;
            s[3] = rotl64(s[3], 45);

            return result;
        }

        [[nodiscard]] constexpr std::uint32_t next_u32() noexcept
        {
            return static_cast<std::uint32_t>(next_u64() >> 32);
        }
    };

    // ----------------------------
    // Thread-local default RNG
    // ----------------------------
    namespace detail
    {
        [[nodiscard]] inline std::uint64_t entropy_seed() noexcept
        {
            // random_device quality varies by platform; we mix multiple pulls.
            std::random_device rd;
            std::uint64_t a = (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
            std::uint64_t b = (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
            std::uint64_t c = (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
            return a ^ rotl64(b, 21) ^ rotl64(c, 43) ^ 0xD6E8FEB86659FD93ULL;
        }

        inline thread_local xoshiro256ss tl_rng{ entropy_seed() };
    } // namespace detail

    [[nodiscard]] inline xoshiro256ss& thread_rng() noexcept
    {
        return detail::tl_rng;
    }

    inline void seed_thread(std::uint64_t seed) noexcept
    {
        detail::tl_rng.seed_with(seed);
    }

    // ----------------------------
    // Unbiased bounded uniform: [0, bound-1]
    // Using Lemire-style multiplication + rejection
    // ----------------------------
    template <unsigned_int UInt, class Rng>
    [[nodiscard]] inline UInt uniform_bounded(Rng& rng, UInt bound) noexcept
    {
        // Precondition: bound != 0.
        if (bound == 0) return 0;

        // Fast path for power-of-two bounds.
        if ((bound & (bound - 1)) == 0)
        {
            return static_cast<UInt>(rng.next_u64() & (static_cast<std::uint64_t>(bound) - 1ULL));
        }

        const std::uint64_t b = static_cast<std::uint64_t>(bound);
        const std::uint64_t threshold = (std::uint64_t{0} - b) % b;

        for (;;)
        {
            const std::uint64_t x = rng.next_u64();

#if defined(__SIZEOF_INT128__)
            const __uint128_t m = static_cast<__uint128_t>(x) * static_cast<__uint128_t>(b);
            const std::uint64_t l = static_cast<std::uint64_t>(m);
            if (l >= threshold)
                return static_cast<UInt>(m >> 64);

#elif defined(_MSC_VER) && defined(_M_X64)
            // MSVC x64: use _umul128
            std::uint64_t hi = 0;
            const std::uint64_t lo = _umul128(x, b, &hi);
            if (lo >= threshold)
                return static_cast<UInt>(hi);

#else
            // Portable unbiased rejection (slower than mul-high):
            // Accept x only if it falls under the largest multiple of b.
            const std::uint64_t limit = (std::numeric_limits<std::uint64_t>::max() / b) * b;
            if (x < limit)
                return static_cast<UInt>(x % b);
#endif
        }
    }

    // ----------------------------
    // Runtime odds: "true with probability 1/bound"
    // ----------------------------
    template <unsigned_int UInt = std::uint32_t, class Rng = xoshiro256ss>
    [[nodiscard]] inline bool one_in(Rng& rng, UInt bound) noexcept
    {
        if (bound <= 1) return true;
        return uniform_bounded<UInt>(rng, bound) == 0;
    }

    template <unsigned_int UInt = std::uint32_t>
    [[nodiscard]] inline bool one_in(UInt bound) noexcept
    {
        return one_in<UInt>(thread_rng(), bound);
    }

    // ----------------------------
    // Compile-time/cached odds: one_in<N>()
    // ----------------------------
    template <std::uint32_t N>
    struct one_in_t final
    {
        static_assert(N >= 1, "one_in_t<N>: N must be >= 1");

        [[nodiscard]] inline bool operator()(xoshiro256ss& rng) const noexcept
        {
            if constexpr (N == 1) return true;
            return uniform_bounded<std::uint32_t>(rng, static_cast<std::uint32_t>(N)) == 0;
        }

        [[nodiscard]] inline bool operator()() const noexcept
        {
            return (*this)(thread_rng());
        }
    };

    template <std::uint32_t N>
    inline constexpr one_in_t<N> one_in_v{};

    // ----------------------------
    // Presets / common denominators
    // ----------------------------
    // Requested:
    // 2, 5, 10, 25, 50, 100
    //
    // Additional common gameplay denominators:
    // 3, 4, 6, 8, 12, 16, 20, 30, 60, 128, 256
    inline constexpr one_in_t<2>   p2{};
    inline constexpr one_in_t<3>   p3{};
    inline constexpr one_in_t<4>   p4{};
    inline constexpr one_in_t<5>   p5{};
    inline constexpr one_in_t<6>   p6{};
    inline constexpr one_in_t<8>   p8{};
    inline constexpr one_in_t<10>  p10{};
    inline constexpr one_in_t<12>  p12{};
    inline constexpr one_in_t<16>  p16{};
    inline constexpr one_in_t<20>  p20{};
    inline constexpr one_in_t<25>  p25{};
    inline constexpr one_in_t<30>  p30{};
    inline constexpr one_in_t<50>  p50{};
    inline constexpr one_in_t<60>  p60{};
    inline constexpr one_in_t<100> p100{};
    inline constexpr one_in_t<128> p128{};
    inline constexpr one_in_t<256> p256{};

    // Convenience named wrappers.
    [[nodiscard]] inline bool one_in_2()   noexcept { return p2(); }
    [[nodiscard]] inline bool one_in_5()   noexcept { return p5(); }
    [[nodiscard]] inline bool one_in_10()  noexcept { return p10(); }
    [[nodiscard]] inline bool one_in_25()  noexcept { return p25(); }
    [[nodiscard]] inline bool one_in_50()  noexcept { return p50(); }
    [[nodiscard]] inline bool one_in_100() noexcept { return p100(); }

} // namespace ayejay::odds
