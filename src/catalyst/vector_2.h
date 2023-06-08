#ifndef catalystv2_h
#define catalystv2_h

#include "typesdef.h"

struct Vector2
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

inline Vector2 V2(r32 x, r32 y)
{
	Vector2 result = {};

	result.X = x;
	result.Y = y;

	return result;
}

inline Vector2 operator-(Vector2 a)
{
	Vector2 result = {};

	result.X = -a.X;
	result.Y = -a.Y;

	return result;
}

inline Vector2 operator+(Vector2 a, Vector2 b)
{
	Vector2 result = {};

	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;

	return result;
}

inline Vector2 operator-(Vector2 a, Vector2 b)
{
	Vector2 result = {};

	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;

	return result;
}

inline Vector2 operator*(Vector2 a, r32 b)
{
	Vector2 result = {};

	result.X = b * a.X;
	result.Y = b * a.Y;

	return result;
}

inline Vector2 operator*(r32 a, Vector2 b)
{
	return b * a;
}

inline Vector2 &operator+=(Vector2 &a, Vector2 b)
{
	a = a + b;
	return a;
}

inline Vector2 &operator*=(Vector2 &a, r32 b)
{
	a = a * b;

	return a;
}

#endif