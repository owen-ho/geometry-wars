#include "Vec2.h"
#include <math.h>
#include <iostream>

// Default constructor
Vec2::Vec2()
{

}

// Constructor
Vec2::Vec2(float xin, float yin) :x(xin), y(yin)
{

}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	//TODO
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	//TODO
	return Vec2(x / val, y / val);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	//TODO
	return !(x == rhs.x && y == rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
	//TODO
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	//TODO
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	//TODO
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	//TODO
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	//TODO
	float dx = rhs.x-x;
	float dy = rhs.y - y;
	return sqrtf((dx * dx) + (dy * dy));
}

float Vec2::length() const
{
	return 0;
}

float Vec2::magnitude() const
{
	return sqrtf((x * x) + (y * y));
}

const Vec2& Vec2::normalize()
{
	float mag = this->magnitude();
	if (mag > 0)
	{
		x /= mag;
		y /= mag;
	}
	return Vec2(x,y);
}