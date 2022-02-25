#include "wrapping_integers.hh"

#include <cstdint>

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

using namespace std;

constexpr uint64_t u64_max = 4294967296UL;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    auto isn_64 = static_cast<uint64_t>(isn.raw_value());
    auto res = static_cast<uint32_t>(n % u64_max + isn_64);
    return WrappingInt32{res};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint32_t diff = n - wrap(checkpoint, isn);
    uint64_t res = static_cast<uint64_t>(diff) + checkpoint;
    // change res to be the closest number to checkpoint
    if (diff >= (1U << 31) && res >= (1UL << 32))
        res -= 1UL << 32;
    return res;
}
