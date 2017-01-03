#ifndef __VEC2_H__
#define __VEC2_H__

#include <cmath>

template <class T>
struct vec2 {
	T X, Y;

	vec2() :X(0), Y(0) {}
	vec2(T X, T Y) : X(X), Y(Y) {}
	vec2(const vec2& v) : X(v.X), Y(v.Y) {}

	vec2& operator=(const vec2& v) {
		X = v.X;
		Y = v.Y;
		return *this;
	}
	vec2& operator=(vec2&& v) {
		X = v.X;
		Y = v.Y;
		return *this;
	}
	vec2 operator-() const {
		return vec2(-X, -Y);
	}


	vec2 operator+(const vec2& v) const noexcept {
		return vec2(X + v.X, Y + v.Y);
	}
	vec2 operator-(const vec2& v) const noexcept {
		return vec2(X - v.X, Y - v.Y);
	}

	vec2& operator+=(const vec2& v) {
		X += v.X;
		Y += v.Y;
		return *this;
	}
	vec2& operator+=(const vec2&& v) {
		X += v.X;
		Y += v.Y;
		return *this;
	}
	vec2& operator-=(const vec2& v) {
		X -= v.X;
		Y -= v.Y;
		return *this;
	}

	vec2 operator+(T s) const {
		return vec2(X + s, Y + s);
	}
	vec2 operator-(T s) const {
		return vec2(X - s, Y - s);
	}
	vec2 operator*(T s) const {
		return vec2(X * s, Y * s);
	}
	vec2 operator/(T s) const {
		return vec2(X / s, Y / s);
	}

	vec2& operator+=(T s) {
		X += s;
		Y += s;
		return *this;
	}
	vec2& operator-=(T s) {
		X -= s;
		Y -= s;
		return *this;
	}
	vec2& operator*=(T s) {
		X *= s;
		Y *= s;
		return *this;
	}
	vec2& operator/=(T s) {
		X /= s;
		Y /= s;
		return *this;
	}

	void set(T X, T Y) {
		this->X = X;
		this->Y = Y;
	}

	static T Length(vec2 V) {
		return std::sqrt(V.X * V.X + V.Y * V.Y);
	}


	void rotate(T deg) {
		T theta = deg / 180.0 * M_PI;
		T c = cos(theta);
		T s = sin(theta);
		T tX = X * c - Y * s;
		T tY = X * s + Y * c;
		X = tX;
		Y = tY;
	}
	vec2 rotated(T deg) const {
		T theta = deg / 180.0 * M_PI;
		T c = cos(theta);
		T s = sin(theta);
		T tX = X * c - Y * s;
		T tY = X * s + Y * c;
		return vec2(tX, tY);
	}

	vec2& Normalize() {
		if (Length(*this) == 0) return *this;
		*this /= Length(*this);
		return *this;
	}
	vec2 Normalized() const {
		return vec2{ *this /  Length(*this) };
	}
	vec2 multiple(float s) const {
		return vec2{ X*s, Y*s };
	}

	float dist(vec2 v) const {
		vec2 d(v.X - X, v.Y - Y);
		return d.length();
	}

	void truncate(T length) {
		T angle = atan2f(Y, X);
		X = length * cos(angle);
		Y = length * sin(angle);
	}

	vec2 ortho() const {
		return vec2(Y, -X);
	}

	static float dot(vec2 v1, vec2 v2) {
		return v1.X * v2.X + v1.Y * v2.Y;
	}
	static float cross(vec2 v1, vec2 v2) {
		return (v1.X * v2.Y) - (v1.Y * v2.X);
	}
};

typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

//Function for finding where two lines intersect
//p1 and p2 are the first lines coordinates
//p3 and p4 are the second lines coordinates
inline vec2f GetIntersection(const vec2f& p1, const vec2f& p2, const vec2f& p3, const vec2f& p4)
{
	//Find the lines' slopes
	//k = Y/X
	float k1 = (p1.Y - p2.Y) / (p1.X - p2.X);
	float k2 = (p3.Y - p4.Y) / (p3.X - p4.X);

	//Find the vertical displacement
	//b = Y - k*X
	float b1 = p1.Y - k1*p1.X;
	float b2 = p3.Y - k2*p3.X;

	vec2f intersect;

	//Find the X intersection
	//X = (b2-b1)/(k1-k2)
	intersect.X = (b2 - b1) / (k1 - k2);
	//Reuse the X intersect to get the Y
	intersect.Y = k1*intersect.X + b1;

	return intersect;
}

//Function for checking whether a point is above a line or below
//p1 and p2 are the points of the line
inline bool IsPointAboveLine(const vec2f& P1, const vec2f& P2, const vec2f& P)
{
	//The slope of the line
	float k = (P2.Y - P1.Y) / (P2.X - P1.X);

	//The vertical displacement of the line
	//b = Y-k*X
	float b = P1.Y - k*P1.X;

	//If the point's Y value is larger than the lines' at the point's X value, the point is above the line
	if (P.Y < P.X*k + b)
	{
		return true;
	}
	return false;
}

inline vec2f GetAverageP(const vec2f Verts[], int n)
{
	vec2f AverageP = vec2f{ 0, 0 };
	for (int i = 0; i < n; i++)
	{
		AverageP += Verts[i];
	}
	AverageP /= (float)n;
	return AverageP;
}

#endif
