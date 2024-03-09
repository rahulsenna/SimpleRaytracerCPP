#pragma once
#include <stdint.h>
#include <math.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int32_t b32;

typedef float r32;
typedef double r64;

inline r32
SquareRoot(r32 Real32)
{
    r32 Result = sqrtf(Real32);
    return (Result);
}

inline r32
SafeRatioN(r32 Numerator, r32 Divisor, r32 N)
{
    r32 Result = N;

    if (Divisor != 0.0f)
    {
        Result = Numerator / Divisor;
    }

    return (Result);
}

inline r32
SafeRatio0(r32 Numerator, r32 Divisor)
{
    r32 Result = SafeRatioN(Numerator, Divisor, 0.0f);
    return (Result);
}

inline r32
SafeRatio1(r32 Numerator, r32 Divisor)
{
    r32 Result = SafeRatioN(Numerator, Divisor, 1.0f);
    return (Result);
}

struct v2
{
    union
    {
        struct
        {
            r32 x, y;
        };
        r32 E[2];
    };
};

struct v3
{
    union
    {
        struct
        {
            r32 x, y, z;
        };

        struct
        {
            r32 r, g, b;
        };
        r32 E[3];
    };
};

struct v4
{
    union
    {
        struct
        {
            r32 x, y, z, w;
        };

        struct
        {
            r32 r, g, b, a;
        };
        struct
        {
            v3 xyz;
            r32 Ignore0_;
        };
        struct
        {
            v3 rgb;
            r32 Ignore1_;
        };
        r32 E[4];
    };
};

inline v3
operator+(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return (Result);
}

inline v3
operator*(r32 A, v3 B)
{
    v3 Result = {};
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;

    return (Result);
}

inline v3
operator*(v3 B, r32 A)
{
    v3 Result = A * B;
    return (Result);
}

inline v3
operator-(v3 A)
{
    v3 Result = {};
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    return (Result);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return (Result);
}

inline v3&
operator*=(v3& B, r32 A)
{
    B = A * B;
    return (B);
}

inline v3&
operator+=(v3& B, v3 A)
{
    B = A + B;
    return (B);
}
inline v3&
operator-=(v3& B, v3 A)
{
    B = B - A;
    return (B);
}


inline r32
DotProduct(v3 A, v3 B)
{
    r32 Result = A.x * B.x + A.y * B.y + A.z * B.z;
    return (Result);
}

inline v3
Cross(const v3& A, const v3& B)
{
    v3 Result = { A.y*B.z-A.z*B.y, A.z*B.x-A.x*B.z, A.x*B.y-A.y*B.x };
    //           (A.y*B.z-A.z*B.y, A.z*B.x-A.x*B.z, A.x*B.y-A.y*B.x);
    return (Result);
}

inline r32
LengthSq(v3 A)
{
    r32 Result = DotProduct(A, A);
    return (Result);
}

inline r32
Length(v3 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return (Result);
}

inline void
Normalize(v3& A)
{
    r32 Magnitude = Length(A);
    A *= 1.f/Magnitude;
}
inline r32
Clamp(r32 Min, r32 Value, r32 Max)
{
    r32 Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }
    return (Result);
}

inline r32
Clamp01(r32 Value)
{
    r32 Result = Clamp(0.0f, Value, 1.0f);
    return (Result);
}

#define RED_SPACE   16
#define GREEN_SPACE 8
#define BLUE_SPACE  0

inline u32 
Color2UINT(r32 r, r32 g, r32 b)
{
	r32 R = Clamp01(r);
	r32 G = Clamp01(g);
	r32 B = Clamp01(b);

	u32 Color32 = (0 |
		(u32(R * 255.0f) << RED_SPACE) |
		(u32(G * 255.0f) << GREEN_SPACE) |
		(u32(B * 255.0f) << BLUE_SPACE));

	return(Color32);
}
inline u32 
Color2UINT(v3 &C)
{
    return Color2UINT(C.r, C.g, C.b);
}