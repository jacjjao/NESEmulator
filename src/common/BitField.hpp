#pragma once

template<class T>
class BitField
{
public:
    T bit;

    template<class U>
    U getValueAs() const
    {
        return bit_cast<U>(bit);
    }

    template<class U>
    void setValue(const U& value)
    {
        bit = bit_cast<T>(value);
    }

private:
    template<typename T, typename U>
    static T bit_cast(U& bit) 
    {
        static_assert(sizeof(T) == sizeof(U));
        static_assert(std::is_pod_v<T>);
        static_assert(std::is_pod_v<U>);
        T val{};
        std::memcpy(std::addressof(val), std::addressof(bit), sizeof(T));
        return val;
    }
};