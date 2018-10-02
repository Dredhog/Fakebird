#if !defined(MATRIX_H)
#define MATRIX_H
union mat3{
	struct{
		float _11, _12, _13;
		float _21, _22, _23;
		float _31, _32, _33;
	};
	float e[9];
};

union mat4{
	struct{
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;
	};
	float e[16];
};

internal mat3 Mat3Ident();
internal mat4 Mat4Ident();
internal mat3 MulMat3(mat3 A, mat3 B);
internal mat4 MulMat4(mat4 A, mat4 B);
//internal vec3 MulMat3Vec3(mat3 Mat, vec3 Vec);
//internal vec4 MulMat4Vec4(mat4 Mat, vec4 Vec);
#endif //MATRIX_H
