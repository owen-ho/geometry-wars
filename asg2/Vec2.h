#pragma once

// Header file is defining the variables and functions, cpp file is where implementation is done
class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const float val) const; // Don't need to use pointers for primitive types
	Vec2 operator * (const float val) const;

	void operator += (const Vec2& rhs); // No const at the end as function will update value of x or y
	void operator -= (const Vec2& rhs);
	void operator *= (const float rhs);
	void operator /= (const float rhs);

	float dist(const Vec2& rhs) const;
	float length() const;
	float magnitude() const;
	const Vec2& normalize();
};