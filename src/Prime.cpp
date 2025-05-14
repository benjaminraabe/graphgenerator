#include <cmath>


// Naive primality-Testing for n>2 in O(sqrt(n))
inline bool naive_prime(const int n) {
    if (n % 2 == 0)
        return false;

    int divisor = 3;
    const int target = std::ceil(std::sqrt(n));
    while (divisor <= target)
    {
        if (n % divisor == 0) {
            return false;
        }
        divisor = divisor + 2;
    }

    return true;
}


// Miller-Rabin Primality Test
// Stochastic-Test with Failure-Probability < 1/2 ^ Iterations
inline bool miller_rabin(int n, int iterations) {
    throw; // Not yet implemented.
    return true;
}


inline bool is_prime(const int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    return naive_prime(n);
}