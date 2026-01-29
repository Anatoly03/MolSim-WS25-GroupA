
/**
 * @brief Tests whether the provided arguments are in ascending order.
 */
// TODO rewrite to macro or template vararg
template <typename A, typename B, typename C>
constexpr bool ascending(A a, B b, C c) {
    return a <= b && b <= c;
}

/**
 * @brief Rounds the number to decimal.
 */
// TODO rewrite to macro or template vararg
constexpr double round(double i, double f) {
    return std::round(i / f) * f;
}
