
/**
 * @brief Tests whether the provided arguments are in ascending order.
 */
// TODO rewrite to macro or template vararg
constexpr bool ascending(int a, int b, int c) {
    return a <= b && b <= c;
}

/**
 * @brief Rounds the number to decimal.
 */
// TODO rewrite to macro or template vararg
constexpr double round(double i, double f) {
    return std::round(i / f) * f;
}
