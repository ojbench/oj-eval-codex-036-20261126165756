#include "../dynamic_bitset.hpp"

static std::uint64_t state = 123456789;
static std::size_t random_number() {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}
static bool check(const dynamic_bitset &bits, const std::string &model) {
    if (bits.size() != model.size()) return false;
    for (std::size_t i = 0; i < model.size(); ++i)
        if (bits[i] != (model[i] == '1')) return false;
    return bits.none() == (model.find('1') == std::string::npos)
        && bits.all() == (model.find('0') == std::string::npos);
}
static std::string random_string(std::size_t n) {
    std::string s(n, '0');
    for (char &c : s) c += random_number() % 2;
    return s;
}
int main() {
    if (!check(sjtu::dynamic_bitset(), "")) return 7;
    for (std::size_t n = 0; n <= 260; ++n) {
        const std::string s = random_string(n);
        dynamic_bitset source(s);
        dynamic_bitset moved(std::move(source));
        if (!check(moved, s) || !check(source, "")) return 8;
        source = std::move(moved);
        if (!check(source, s) || !check(moved, "")) return 9;
        source = std::move(source);
        if (!check(source, s)) return 10;
        if (!check(dynamic_bitset(n), std::string(n, '0'))) return 1;
        for (std::size_t shift = 0; shift <= 260; ++shift) {
            dynamic_bitset left(s), right(s);
            left <<= shift;
            right >>= shift;
            if (!check(left, std::string(shift, '0') + s)) return 2;
            if (!check(right, s.substr(std::min(shift, n)))) return 3;
        }
        for (std::size_t m = 0; m <= 260; ++m) {
            const std::string t = random_string(m);
            for (int op = 0; op < 3; ++op) {
                dynamic_bitset a(s), b(t);
                std::string expected = s;
                for (std::size_t i = 0; i < std::min(n, m); ++i) {
                    bool x = s[i] == '1', y = t[i] == '1';
                    expected[i] = '0' + (op == 0 ? x | y : op == 1 ? x & y : x ^ y);
                }
                if (op == 0) a |= b;
                if (op == 1) a &= b;
                if (op == 2) a ^= b;
                if (!check(a, expected) || !check(b, t)) return 4;
            }
        }
    }
    for (int trial = 0; trial < 200; ++trial) {
        dynamic_bitset a;
        std::string s;
        for (int step = 0; step < 2000; ++step) {
            std::size_t n = random_number() % 130;
            switch (random_number() % 10) {
            case 0: a <<= n; s.insert(0, n, '0'); break;
            case 1: a >>= n; s.erase(0, std::min(n, s.size())); break;
            case 2: a.push_back(n % 2); s += char('0' + n % 2); break;
            case 3: a.set(); std::fill(s.begin(), s.end(), '1'); break;
            case 4: a.reset(); std::fill(s.begin(), s.end(), '0'); break;
            case 5: a.flip(); for (char &c : s) c = c == '0' ? '1' : '0'; break;
            case 6:
                if (!s.empty()) { n %= s.size(); a.set(n, false); s[n] = '0'; }
                break;
            case 7: a ^= a; std::fill(s.begin(), s.end(), '0'); break;
            case 8: a |= a; a &= a; break;
            case 9: {
                dynamic_bitset copy(a), assigned;
                assigned = a;
                a.flip();
                if (!check(copy, s) || !check(assigned, s)) return 5;
                a = assigned;
                break;
            }
            }
            if (!check(a, s)) return 6;
        }
    }
    std::cout << "All differential tests passed\n";
}
