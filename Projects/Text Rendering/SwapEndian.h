#ifndef SWAP_ENDIAN_H
#define SWAP_ENDIAN_H

#include <cstdint>

template <typename T>
inline T swapEndian(T val);

template <>
inline uint16_t swapEndian(uint16_t val) {
    return (val >> 8) | (val << 8);
}

template <>
inline uint32_t swapEndian(uint32_t val) {
    return ((val >> 24) & 0x000000FF) |
        ((val >> 8) & 0x0000FF00) |
        ((val << 8) & 0x00FF0000) |
        ((val << 24) & 0xFF000000);
}

template <>
inline uint64_t swapEndian(uint64_t val) {
    return ((val >> 56) & 0x00000000000000FFULL) |
        ((val >> 40) & 0x000000000000FF00ULL) |
        ((val >> 24) & 0x0000000000FF0000ULL) |
        ((val >> 8) & 0x00000000FF000000ULL) |
        ((val << 8) & 0x000000FF00000000ULL) |
        ((val << 24) & 0x0000FF0000000000ULL) |
        ((val << 40) & 0x00FF000000000000ULL) |
        ((val << 56) & 0xFF00000000000000ULL);
}

template <>
inline int16_t swapEndian(int16_t val) {
    return (val >> 8) | (val << 8);
}

template <>
inline char swapEndian(char val) {
    return val; // No actual endian swap needed for a single byte
}

#endif