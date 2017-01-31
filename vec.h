#ifndef __VEC_H__
#define __VEC_H__

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

	bool operator==(const vec2& B) const {
		return (this->X == B.X && this->Y == B.Y);
	}
	bool operator!=(const vec2& B) const {
		return (this->X != B.X || this->Y != B.Y);
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

	vec2 operator*(T s) const {
		return vec2{X * s, Y * s};
	}
	vec2 operator/(T s) const {
		return vec2{X / s, Y / s};
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

	static T dot(vec2 v1, vec2 v2) {
		return v1.X * v2.X + v1.Y * v2.Y;
	}
};

typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

template <class T>
struct vec3 {
	T X, Y, Z;

	vec3 operator-() const {
		return vec3{-X, -Y, -Z};
	}

	vec3 operator+(const vec3& v) const {
		return vec3{X + v.X, Y + v.Y, Z + v.Z};
	}
	vec3 operator-(const vec3& v) const {
		return vec3{X - v.X, Y - v.Y, Z - v.Z};
	}

	bool operator==(const vec3& B) const {
		return (this->X == B.X && this->Y == B.Y && this->Z == B.Z);
	}
	bool operator!=(const vec3& B) const {
		return (this->X != B.X || this->Y != B.Y || this->Z != B.Z);
	}

	vec3& operator+=(const vec3& v) {
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}
	vec3& operator+=(const vec3&& v) {
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}
	vec3& operator-=(const vec3& v) {
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	vec3 operator*(T s) const {
		return vec3{X * s, Y * s, Z * s};
	}
	vec3 operator/(T s) const {
		return vec3{X / s, Y / s, Z / s};
	}
	vec3& operator*=(T s) {
		X *= s;
		Y *= s;
		Z *= s;
		return *this;
	}
	vec3& operator/=(T s) {
		X /= s;
		Y /= s;
		Z /= s;
		return *this;
	}

	static T dot(vec3 v1, vec3 v2) {
		return v1.X * v2.X + v1.Y * v2.Y, + v1.Z * v2.Z;
	}
};

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;
#endif //__VEC_H__
