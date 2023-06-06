#ifndef catalyst_v3_h
#define catalyst_v3_h

#include "typesdef.h"

struct vector_3
{
	union
	{
		struct
		{
			r32 X;
			r32 Y;
			r32 Z;
		};

		struct
		{
			r32 R;
			r32 G;
			r32 B;
		};

		r32 Elements[3];
	};
};

inline vector_3 V3(r32 x, r32 y, r32 z)
{
	vector_3 result = {};

	result.X = x;
	result.Y = y;
	result.Z = z;

	return result;
}

inline vector_3 operator-(vector_3 a)
{
	vector_3 result = {};

	result.X = -a.X;
	result.Y = -a.Y;
	result.Z = -a.Z;

	return result;
}

inline vector_3 operator+(vector_3 a, vector_3 b)
{
	vector_3 result = {};

	result.X = a.X + b.X;
	result.Y = a.Y + b.Y;
	result.Z = a.Z + b.Z;

	return result;
}

inline vector_3 operator-(vector_3 a, vector_3 b)
{
	vector_3 result = {};

	result.X = a.X - b.X;
	result.Y = a.Y - b.Y;
	result.Z = a.Z - b.Z;

	return result;
}

inline vector_3 operator*(vector_3 a, r32 b)
{
	vector_3 result = {};

	result.X = b * a.X;
	result.Y = b * a.Y;
	result.Z = b * a.Z;

	return result;
}

inline vector_3 operator*(r32 a, vector_3 b)
{
	return b * a;
}

inline vector_3 &operator+=(vector_3 &a, vector_3 b)
{
	a = a + b;
	return a;
}

inline vector_3 &operator*=(vector_3 &a, r32 b)
{
	a = a * b;
	return a;
}


#endif