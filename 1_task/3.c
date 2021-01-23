#include <stdint.h>

typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

typedef enum {
    MoveToFindSign = 63,
    MoveToFindExp = 52,
    ExpBitMask = 0x7FF,
    One = 1,
    MantissaBitMask = 0xFFFFFFFFFFFFF,
    FiftyFirstBit = 0x8000000000000 // Единица в 51 разряде
} checking_flags;


_Bool
is_positive(uint8_t* sign)
{
    return *sign == 0;
}

_Bool
full_exp(uint16_t* exp) {
    return *exp == ExpBitMask;
}

_Bool
zero_exp(uint16_t* exp) {
    return (*exp & ExpBitMask) == 0;
}

_Bool
full_mantissa(uint64_t* mantissa) {
    return *mantissa == MantissaBitMask;
}

_Bool
zero_mantissa(uint64_t* mantissa) {
    return (*mantissa & MantissaBitMask) == 0;
}

float_class_t
work_with_positive(uint16_t* exp, uint64_t* mantissa)
{
    if (zero_mantissa(mantissa) && zero_exp(exp)) {
        return PlusZero;
    }
    if (full_exp(exp)) // inf or NaN
    {
        if (zero_mantissa(mantissa)) {
            return PlusInf;
        }
        return (*mantissa & FiftyFirstBit) ? QuietNaN : SignalingNaN;
    }
    if (zero_exp(exp)) {
        return PlusDenormal;
    }
    return PlusRegular;
}


float_class_t
work_with_negative(uint16_t* exp, uint64_t* mantissa)
{
    if (zero_mantissa(mantissa) && zero_exp(exp)) {
        return MinusZero;
    }
    if (full_exp(exp)) // inf or NaN
    {
        if (zero_mantissa(mantissa)) {
            return MinusInf;
        }
            return (*mantissa & FiftyFirstBit) ? QuietNaN : SignalingNaN;
        }
        if (zero_exp(exp)) {
            return MinusDenormal;
    }
    return MinusRegular;
}

extern float_class_t
classify(double *value_ptr)
{
    // Приведение типов с помощью указателей для работы с битами
    void* ptr_to_void = value_ptr;
    uint64_t* uint_ptr = ptr_to_void;
    uint8_t sign = *uint_ptr >> MoveToFindSign; // знак
    uint16_t exp = *uint_ptr >> MoveToFindExp & ExpBitMask; // экспонента
    uint64_t mantissa = *uint_ptr & MantissaBitMask; // мантисса
    return is_positive(&sign) ? work_with_positive(&exp, &mantissa) : work_with_negative(&exp, &mantissa);
}
