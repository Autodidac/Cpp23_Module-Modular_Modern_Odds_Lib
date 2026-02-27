module;

#include "ayejay/odds.hpp"

export module ayejay.odds;

export namespace ayejay::odds
{
    using ::ayejay::odds::splitmix64;
    using ::ayejay::odds::xoshiro256ss;
    using ::ayejay::odds::thread_rng;
    using ::ayejay::odds::seed_thread;
    using ::ayejay::odds::uniform_bounded;
    using ::ayejay::odds::one_in;
    using ::ayejay::odds::one_in_t;
    using ::ayejay::odds::one_in_v;

    using ::ayejay::odds::p2;
    using ::ayejay::odds::p3;
    using ::ayejay::odds::p4;
    using ::ayejay::odds::p5;
    using ::ayejay::odds::p6;
    using ::ayejay::odds::p8;
    using ::ayejay::odds::p10;
    using ::ayejay::odds::p12;
    using ::ayejay::odds::p16;
    using ::ayejay::odds::p20;
    using ::ayejay::odds::p25;
    using ::ayejay::odds::p30;
    using ::ayejay::odds::p50;
    using ::ayejay::odds::p60;
    using ::ayejay::odds::p100;
    using ::ayejay::odds::p128;
    using ::ayejay::odds::p256;

    using ::ayejay::odds::one_in_2;
    using ::ayejay::odds::one_in_5;
    using ::ayejay::odds::one_in_10;
    using ::ayejay::odds::one_in_25;
    using ::ayejay::odds::one_in_50;
    using ::ayejay::odds::one_in_100;
}
