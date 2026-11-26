#ifndef DYNAMIC_BITSET_HPP
#define DYNAMIC_BITSET_HPP

#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>

namespace sjtu {

struct dynamic_bitset {
private:
    using word = std::uint64_t;
    std::vector<word> words_;
    std::size_t size_ = 0;

    static std::size_t word_count(std::size_t n) {
        return n / 64 + (n % 64 != 0);
    }
    static word low_mask(std::size_t n) {
        return (word(1) << n) - 1; // Only called with n < 64.
    }
    void trim() {
        if (size_ % 64) words_.back() &= low_mask(size_ % 64);
    }

public:
    dynamic_bitset() = default;
    ~dynamic_bitset() = default;
    dynamic_bitset(const dynamic_bitset &) = default;
    dynamic_bitset &operator=(const dynamic_bitset &) = default;
    dynamic_bitset(std::size_t n) : words_(word_count(n), 0), size_(n) {}
    dynamic_bitset(const std::string &str) : dynamic_bitset(str.size()) {
        for (std::size_t i = 0; i < size_; ++i)
            words_[i / 64] |= word(str[i] == '1') << (i % 64);
    }

    bool operator[](std::size_t n) const {
        return (words_[n / 64] >> (n % 64)) & 1;
    }
    dynamic_bitset &set(std::size_t n, bool val = true) {
        const word mask = word(1) << (n % 64);
        if (val) words_[n / 64] |= mask;
        else words_[n / 64] &= ~mask;
        return *this;
    }
    dynamic_bitset &push_back(bool val) {
        if (size_ % 64 == 0) words_.push_back(0);
        if (val) words_.back() |= word(1) << (size_ % 64);
        ++size_;
        return *this;
    }
    std::size_t size() const { return size_; }
    bool none() const {
        for (word w : words_) if (w) return false;
        return true;
    }
    bool all() const {
        for (std::size_t i = 0; i < size_ / 64; ++i)
            if (words_[i] != ~word(0)) return false;
        return size_ % 64 == 0 || words_.back() == low_mask(size_ % 64);
    }

    dynamic_bitset &operator|=(const dynamic_bitset &other) {
        const std::size_t n = std::min(size_, other.size_);
        for (std::size_t i = 0; i < n / 64; ++i) words_[i] |= other.words_[i];
        if (n % 64) words_[n / 64] |= other.words_[n / 64] & low_mask(n % 64);
        return *this;
    }
    dynamic_bitset &operator&=(const dynamic_bitset &other) {
        const std::size_t n = std::min(size_, other.size_);
        for (std::size_t i = 0; i < n / 64; ++i) words_[i] &= other.words_[i];
        // Ones outside the overlap preserve the remaining bits of this word.
        if (n % 64) words_[n / 64] &= other.words_[n / 64] | ~low_mask(n % 64);
        return *this;
    }
    dynamic_bitset &operator^=(const dynamic_bitset &other) {
        const std::size_t n = std::min(size_, other.size_);
        for (std::size_t i = 0; i < n / 64; ++i) words_[i] ^= other.words_[i];
        if (n % 64) words_[n / 64] ^= other.words_[n / 64] & low_mask(n % 64);
        return *this;
    }

    dynamic_bitset &operator<<=(std::size_t n) {
        if (!n) return *this;
        const std::size_t old_words = words_.size();
        const std::size_t whole = n / 64, part = n % 64;
        const std::size_t new_size = size_ + n;
        words_.resize(word_count(new_size), 0);
        // Descending destinations keep all source words intact until read.
        for (std::size_t i = words_.size(); i-- > 0;) {
            word value = 0;
            if (i >= whole) {
                const std::size_t src = i - whole;
                if (src < old_words) value = words_[src] << part;
                if (part && src && src - 1 < old_words)
                    value |= words_[src - 1] >> (64 - part);
            }
            words_[i] = value;
        }
        size_ = new_size;
        trim();
        return *this;
    }
    dynamic_bitset &operator>>=(std::size_t n) {
        if (n >= size_) {
            words_.clear();
            size_ = 0;
            return *this;
        }
        if (!n) return *this;
        const std::size_t whole = n / 64, part = n % 64;
        const std::size_t new_size = size_ - n;
        for (std::size_t i = 0; i < word_count(new_size); ++i) {
            const std::size_t src = i + whole;
            word value = words_[src] >> part;
            if (part && src + 1 < words_.size())
                value |= words_[src + 1] << (64 - part);
            words_[i] = value;
        }
        size_ = new_size;
        words_.resize(word_count(size_));
        trim();
        return *this;
    }
    dynamic_bitset &set() {
        std::fill(words_.begin(), words_.end(), ~word(0));
        trim();
        return *this;
    }
    dynamic_bitset &flip() {
        for (word &w : words_) w = ~w;
        trim();
        return *this;
    }
    dynamic_bitset &reset() {
        std::fill(words_.begin(), words_.end(), word(0));
        return *this;
    }
};

} // namespace sjtu

using sjtu::dynamic_bitset;

#endif
