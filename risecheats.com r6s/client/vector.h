#pragma once 
#ifndef VECTOR_H
#define VECTOR_H
#include <cmath>
#include <math.h>

#define M_PI 3.14159265358979323846264338327950288419716939937510

class Vector2
{
public:
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
};

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float x, float y, float z);

	Vector3 operator+(const Vector3& vector) const;
	Vector3 operator-(const Vector3& vector) const;
	Vector3 operator-() const;
	Vector3 operator*(float number) const;
	Vector3 operator/(float number) const;

	Vector3& operator+=(const Vector3& vector);
	Vector3& operator-=(const Vector3& vector);
	Vector3& operator*=(float number);
	Vector3& operator/=(float number);

	bool operator==(const Vector3& vector) const;
	bool operator!=(const Vector3& vector) const;

	inline float Dot(const Vector3& vector)
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}

	inline float Distance(const Vector3& vector)
	{
		return sqrt(
			(x - vector.x) * (x - vector.x) +
			(y - vector.y) * (y - vector.y) +
			(z - vector.z) * (z - vector.z));
	}

	inline float WorldDistance(const Vector3& vector)
	{
		return float(sqrtf(powf(vector.x - x, 2.0) + powf(vector.y - y, 2.0) + powf(vector.z - z, 2.0)));

	}
};

inline bool Vector3::operator==(const Vector3& vector) const
{
	return x == vector.x && y == vector.y && z == vector.z;
}

inline bool Vector3::operator!=(const Vector3& vector) const
{
	return x != vector.x || y != vector.y || z != vector.z;
}

inline Vector3 Vector3::operator+(const Vector3& vector) const
{
	return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

inline Vector3 Vector3::operator-(const Vector3& vector) const
{
	return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

inline Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator*(float number) const
{
	return Vector3(x * number, y * number, z * number);
}

inline Vector3 Vector3::operator/(float number) const
{
	return Vector3(x / number, y / number, z / number);
}

// Vector4
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4();
	Vector4(float x, float y, float z, float w);

	Vector4 operator+(const Vector4& vector) const;
	Vector4 operator-(const Vector4& vector) const;
	Vector4 operator-() const;
	Vector4 operator*(float number) const;
	Vector4 operator/(float number) const;

	Vector4& operator+=(const Vector4& vector);
	Vector4& operator-=(const Vector4& vector);
	Vector4& operator*=(float number);
	Vector4& operator/=(float number);

	bool operator==(const Vector4& vector) const;
	bool operator!=(const Vector4& vector) const;

	inline float Dot(const Vector4& vector)
	{
		return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
	}

	inline float Distance(const Vector4& vector)
	{
		return sqrt(
			(x - vector.x) * (x - vector.x) +
			(y - vector.y) * (y - vector.y) +
			(z - vector.z) * (z - vector.z) +
			(w - vector.w) * (w - vector.w));
	}
};

inline bool Vector4::operator==(const Vector4& vector) const
{
	return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
}

inline bool Vector4::operator!=(const Vector4& vector) const
{
	return x != vector.x || y != vector.y || z != vector.z || w != vector.w;
}

inline Vector4 Vector4::operator+(const Vector4& vector) const
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
}

inline Vector4 Vector4::operator-(const Vector4& vector) const
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
}

inline Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator*(float number) const
{
	return Vector4(x * number, y * number, z * number, w * number);
}

inline Vector4 Vector4::operator/(float number) const
{
	return Vector4(x / number, y / number, z / number, w / number);
}


#endif