#ifndef __VEC2_H__
#define __VEC2_H__

#include <cmath>

template <class T>
struct vec2 {
	T X, Y;

	vec2 operator-() const {
		return vec2{-X, -Y};
	}

	vec2 operator+(const vec2& v) const {
		return vec2{X + v.X, Y + v.Y};
	}
	vec2 operator-(const vec2& v) const {
		return vec2{X - v.X, Y - v.Y};
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
		return vec2{X + s, Y + s};
	}
	vec2 operator-(T s) const {
		return vec2{X - s, Y - s};
	}
	vec2 operator*(T s) const {
		return vec2{X * s, Y * s};
	}
	vec2 operator/(T s) const {
		return vec2{X / s, Y / s};
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
		return vec2{tX, tY};
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
		return vec2{Y, -X};
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

#endif
