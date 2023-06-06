#ifndef catalystv4_h
#define catalystv4_h

#include "typesdef.h"

struct vector_4
{
	union
	{
		struct
		{
			r32 X;
			r32 Y;
			r32 Z;
			r32 W;
		};

		struct
		{
			r32 R;
			r32 G;
			r32 B;
			r32 A;
		};

		r32 Elements[4];
	};
};

inline vector_4 V4(r32 x, r32 y, r32 z, r32 w)
{
	vector_4 result = {};

	result.X = x;
	result.Y = y;
	result.Z = z;
	result.W = w;

	return result;
}

inline vector_4 operator-(vector_4 a)
{
	vector_4 result = {};

	result.X = -a.X;
	result.Y = -a.Y;
	result.Z = -a.Z;
	result.W = -a.W;

	return result;
}

inline vector_4 operator+(vector_4 a, vector_4 b)
{
	vector_4 result = {};

	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;
	result.Z = a.Z + b.Z;
	result.W = a.W + b.W;

	return result;
}

inline vector_4 operator-(vector_4 a, vector_4 b)
{
	vector_4 result = {};

	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;
	result.Z = a.Z - b.Z;
	result.W = a.W - b.W;

	return result;
}

inline vector_4 operator*(vector_4 a, r32 b)
{
	vector_4 result = {};

	result.X = b * a.X;
	result.Y = b * a.Y;
	result.Z = b * a.Z;
	result.W = b * a.W;

	return result;
}

inline vector_4 operator*(r32 a, vector_4 b)
{
	return b * a;
}

inline vector_4 &operator+=(vector_4 &a, vector_4 b)
{
	a = a + b;
	return a;
}

inline vector_4 &operator*=(vector_4 &a, r32 b)
{
	a = a * b;
	return a;
}

#endif