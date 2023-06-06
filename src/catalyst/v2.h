#ifndef catalystv2_h
#define catalystv2_h

#include "typesdef.h"

struct v2
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

inline v2 V2(r32 x, r32 y)
{
	v2 result = {};

	result.X = x;
	result.Y = y;

	return result;
}

inline v2 operator-(v2 a)
{
	v2 result = {};

	result.X = -a.X;
	result.Y = -a.Y;

	return result;
}

inline v2 operator+(v2 a, v2 b)
{
	v2 result = {};

	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;

	return result;
}

inline v2 operator-(v2 a, v2 b)
{
	v2 result = {};

	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;

	return result;
}

inline v2 operator*(v2 a, r32 b)
{
	v2 result = {};

	result.X = b * a.X;
	result.Y = b * a.Y;

	return result;
}

inline v2 operator*(r32 a, v2 b)
{
	return b * a;
}

inline v2 &operator+=(v2 &a, v2 b)
{
	a = a + b;
	return a;
}

inline v2 &operator*=(v2 &a, r32 b)
{
	a = a * b;

	return a;
}

#endif