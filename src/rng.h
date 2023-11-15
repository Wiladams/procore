// https://www.corsix.org/

double rand_between_zero_and_one() {
    double d;
    uint64_t x = rand_u64();
    uint64_t e = __builtin_ctzll(x) - 11ull;
    if ((int64_t)e >= 0) e = __builtin_ctzll(rand_u64());
    x = (((x >> 11) + 1) >> 1) - ((e - 1011ull) << 52);
    memcpy(&d, &x, sizeof(d));
    return d;
}
