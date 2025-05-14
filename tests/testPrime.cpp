#include <random>
#include <catch2/catch_test_macros.hpp>
#include "catch2/benchmark/catch_benchmark.hpp"

#include "../src/Prime.cpp"

TEST_CASE("Test the primality of some small numbers", "[prime]") {
    REQUIRE(is_prime(2) == true);
    REQUIRE(is_prime(13) == true);
    REQUIRE(is_prime(167) == true);
    REQUIRE(is_prime(347) == true);
    REQUIRE(is_prime(1039) == true);
    REQUIRE(is_prime(1609) == true);
    REQUIRE(is_prime(2819) == true);
    REQUIRE(is_prime(3847) == true);
    REQUIRE(is_prime(5011) == true);
    REQUIRE(is_prime(7919) == true);

    REQUIRE(is_prime(14) == false);
    REQUIRE(is_prime(345) == false);
    REQUIRE(is_prime(1737) == false);
    REQUIRE(is_prime(2391) == false);
    REQUIRE(is_prime(3333) == false);
    REQUIRE(is_prime(7410) == false);
}

TEST_CASE("Edge-Cases: Numbers <=1 will be defined as non-primes", "[prime]") {
    REQUIRE(is_prime(1) == false);
    REQUIRE(is_prime(0) == false);
    REQUIRE(is_prime(-1) == false);
    REQUIRE(is_prime(-30232123) == false);
}

TEST_CASE("Benchmarks on Prime-Number Detection", "[prime]") {
    SECTION( "Hard-Coded Benchmarks" ) {
        BENCHMARK("Constant-Checks"){
            return is_prime(2);
        };
        BENCHMARK("Smaller Primes"){
            return is_prime(73381507);
        };
        BENCHMARK("Larger Primes"){
            return is_prime(73381507);
        };
    }

    SECTION( "Randomized Benchmarks" ) {
        std::mt19937 gen = std::mt19937(std::random_device{}());
        auto smaller = std::uniform_int_distribution(-10, 50000);
        auto larger = std::uniform_int_distribution(50000000, 1000000000);
        auto mixed = std::uniform_int_distribution(-1000, 100000000);

        BENCHMARK("Smaller Numbers"){
            return is_prime(smaller(gen));
        };
        BENCHMARK("Larger Numbers"){
            return is_prime(larger(gen));
        };
        BENCHMARK("Mixed Numbers"){
            return is_prime(mixed(gen));
        };
    }

}