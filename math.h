#include <cstdint>

// adapted from https://github.com/joxeankoret/tahh/blob/master/comodo/defs.h

template <class T>
T __ROL__(T value, int count)
{
    uint32_t nbits = sizeof(T) * 8;

    if (count > 0)
    {
        count %= nbits;
        T high = value >> (nbits - count);
        if (T(-1) < 0) // signed value
            high &= ~((T(-1) << count));
        value <<= count;
        value |= high;
    }
    else
    {
        count = -count % nbits;
        T low = value << (nbits - count);
        value >>= count;
        value |= low;
    }
    return value;
}

inline uint64_t __ROR8__(uint64_t value, int count) { return __ROL__((uint64_t)value, -count); }