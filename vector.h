#ifndef __VEC_H__
#define __VEC_H__
#include <math.h>

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
	static T Length(vec2 V) {
		return sqrtf(V.X*V.X + V.Y*V.Y);
	}
	static T Normalized(vec2 V) {
		return V/Length(V);
	}
		
};
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

vec2i Vec2i(vec2f v){
	return vec2i{(int)v.X, (int)v.Y};
}
vec2f Vec2f(vec2i v){
	return vec2f{(float)v.X, (float)v.Y};
}

/////////VEC3/////////////
union vec3 {
	struct{
		float X, Y, Z;
	};
	vec2f XY;
};

vec3 operator-(const vec3& V) {
	return vec3{-V.X, -V.Y, -V.Z};
}

vec3 operator+(const vec3& A, const vec3& B) {
	return vec3{A.X + B.X, A.Y + B.Y, A.Z + B.Z};
}
vec3 operator-(const vec3& A, const vec3& B) {
	return vec3{A.X - B.X, A.Y - B.Y, A.Z - B.Z};
}

bool operator==(const vec3& A, const vec3& B) {
	return (A.X == B.X && A.Y == B.Y && A.Z == B.Z);
}
bool operator!=(const vec3& A, const vec3& B) {
	return (A.X != B.X || A.Y != B.Y || A.Z != B.Z);
}

vec3& operator+=(vec3& A, const vec3& B) {
	A.X += B.X;
	A.Y += B.Y;
	A.Z += B.Z;
	return A;
}

vec3& operator-=(vec3& A, const vec3& B) {
	A.X -= B.X;
	A.Y -= B.Y;
	A.Z -= B.Z;
	return A;
}

vec3 operator*(const vec3& A, float s) {
	return vec3{A.X * s, A.Y * s, A.Z * s};
}
vec3 operator*(float s, const vec3& A) {
	return A*s;
}

vec3 operator/(const vec3 A, float s) {
	return vec3{A.X / s, A.Y / s, A.Z / s};
}
vec3 operator/(float s, const vec3 A) {
	return A/s;
}

vec3& operator*=(vec3& A, float s) {
	A.X *= s;
	A.Y *= s;
	A.Z *= s;
	return A;
}
vec3& operator/=(vec3& A, float s) {
	A.X /= s;
	A.Y /= s;
	A.Z /= s;
	return A;
}

static float dot(vec3 v1, vec3 v2) {
	return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
}

static float Length(vec3 V){
	return sqrtf(V.X*V.X + V.Y*V.Y + V.Z*V.Z);
}
static vec3 Normalized(vec3 V){
	return V/Length(V);
}
/////////VEC4/////////////
union vec4 {
	struct{
		float X, Y, Z, W;
	};
	struct{
		vec2f XY;
	};
	struct{
		vec3 XYZ;
	};
};

vec4 operator-(const vec4& V) {
	return vec4{-V.X, -V.Y, -V.Z, -V.W};
}

vec4 operator+(const vec4& A, const vec4& B) {
	return vec4{A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W};
}
vec4 operator-(const vec4& A, const vec4& B) {
	return vec4{A.X - B.X, A.Y - B.Y, A.Z - B.Z, A.W - B.W};
}

bool operator==(const vec4& A, const vec4& B) {
	return (A.X == B.X && A.Y == B.Y && A.Z == B.Z && A.W == B.W );
}
bool operator!=(const vec4& A, const vec4& B) {
	return (A.X != B.X || A.Y != B.Y || A.Z != B.Z || A.W != B.W);
}

vec4& operator+=(vec4& A, const vec4& B) {
	A.X += B.X;
	A.Y += B.Y;
	A.Z += B.Z;
	A.W += B.W;
	return A;
}

vec4& operator-=(vec4& A, const vec4& B) {
	A.X -= B.X;
	A.Y -= B.Y;
	A.Z -= B.Z;
	A.W -= B.W;
	return A;
}

vec4 operator*(const vec4& A, float s) {
	return vec4{A.X * s, A.Y * s, A.Z * s, A.W * s};
}
vec4 operator*(float s, const vec4& A) {
	return A*s;
}

vec4 operator/(const vec4& A, float s) {
	return vec4{A.X / s, A.Y / s, A.Z / s, A.W * s};
}
vec4 operator/(float s, const vec4& A) {
	return A/s;
}

vec4& operator*=(vec4& A, float s) {
	A.X *= s;
	A.Y *= s;
	A.Z *= s;
	A.W *= s;
	return A;
}
vec4& operator/=(vec4& A, float s) {
	A.X /= s;
	A.Y /= s;
	A.Z /= s;
	A.W /= s;
	return A;
}

static float dot(vec4 A, vec4 B) {
	return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}

inline vec4 Vec4(vec3 V3, float W){
	return {V3.X, V3.Y, V3.Z, W};
}
#endif //__VEC_H__
