
/**
 * @brief Tests whether the provided arguments are in ascending order.
 */
// TODO rewrite to macro or template vararg
constexpr bool ascending(int a, int b, int c) {
    return a <= b && b <= c;
}
