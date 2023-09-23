#pragma once

#include <bit>

template<class T>
class BitField
{
public:
    T bit;

    template<class U>
    U getValueAs() const
    {
        return std::bit_cast<U>(bit);
    }

    template<class U>
    void setValue(const U& value)
    {
        bit = std::bit_cast<T>(value);
    }
};