#ifndef catalystv2_h
#define catalystv2_h

#include "typesdef.h"

struct vector_2
{
	union
	{
		struct
		{
			r32 X;
			r32 Y;
		};

		r32 Elements[2];
	};
};

inline vector_2 V2(r32 x, r32 y)
{
	vector_2 result = {};

	result.X = x;
	result.Y = y;

	return result;
}

inline vector_2 operator-(vector_2 a)
{
	vector_2 result = {};

	result.X = -a.X;
	result.Y = -a.Y;

	return result;
}

inline vector_2 operator+(vector_2 a, vector_2 b)
{
	vector_2 result = {};

	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;

	return result;
}

inline vector_2 operator-(vector_2 a, vector_2 b)
{
	vector_2 result = {};

	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;

	return result;
}

inline vector_2 operator*(vector_2 a, r32 b)
{
	vector_2 result = {};

	result.X = b * a.X;
	result.Y = b * a.Y;

	return result;
}

inline vector_2 operator*(r32 a, vector_2 b)
{
	return b * a;
}

inline vector_2 &operator+=(vector_2 &a, vector_2 b)
{
	a = a + b;
	return a;
}

inline vector_2 &operator*=(vector_2 &a, r32 b)
{
	a = a * b;

	return a;
}

#endif