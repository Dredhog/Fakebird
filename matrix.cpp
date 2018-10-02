#if !defined(MATRIX_CPP)
#define MATRIX_CPP
#include <math.h>

internal inline mat3
MulMat3(mat3 A, mat3 B){
	mat3 R;
	int i;
	int Row;
	int Column;

	for(i = 0; i < 9; i++){
		Row    = i / 3;
		Column = i % 3;
		R.e[i] = A.e[ Row*3 + 0 ]*B.e[ 0*3 + Column ] +
			   A.e[ Row*3 + 1 ]*B.e[ 1*3 + Column ] +
			   A.e[ Row*3 + 2 ]*B.e[ 2*3 + Column ];
	}
	return R;
}

internal inline mat4
MulMat4(mat4 A, mat4 B){
	mat4 R;
	int i;
	int Row;
	int Column;

	for(i = 0; i < 16; i++){
		Row    = i / 4;
		Column = i % 4;
		R.e[i] = 	A.e[ Row*4 + 0 ]*B.e[ 0*4 + Column ] +
			   		A.e[ Row*4 + 1 ]*B.e[ 1*4 + Column ] +
			   		A.e[ Row*4 + 2 ]*B.e[ 2*4 + Column ] +
			   		A.e[ Row*4 + 3 ]*B.e[ 3*4 + Column ];
	}
	return R;
}

internal vec3
MulMat3Vec3(mat3 Mat, vec3 Vec){
	vec3 Result;
	Result.X = Mat.e[0]*Vec.X + Mat.e[1]*Vec.Y + Mat.e[2]*Vec.Z;
	Result.X = Mat.e[3]*Vec.X + Mat.e[4]*Vec.Y + Mat.e[5]*Vec.Z;
	Result.X = Mat.e[6]*Vec.X + Mat.e[7]*Vec.Y + Mat.e[8]*Vec.Z;
	return Result;
}

internal vec4
MulMat4Vec4(mat4 Mat, vec4 Vec){
	vec4 Result;
	Result.X = Mat.e[ 0]*Vec.X + Mat.e[ 1]*Vec.Y + Mat.e[ 2]*Vec.Z + Mat.e[ 3]*Vec.W;
	Result.Y = Mat.e[ 4]*Vec.X + Mat.e[ 5]*Vec.Y + Mat.e[ 6]*Vec.Z + Mat.e[ 7]*Vec.W;
	Result.Z = Mat.e[ 8]*Vec.X + Mat.e[ 9]*Vec.Y + Mat.e[10]*Vec.Z + Mat.e[11]*Vec.W;
	Result.W = Mat.e[12]*Vec.X + Mat.e[13]*Vec.Y + Mat.e[14]*Vec.Z + Mat.e[15]*Vec.W;
	return Result;

}

internal void
PrintMat3(mat3 Mat){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf("%3.2f ", (double)Mat.e[i*3 + j]);
		}
		printf("\n");
	}
}

internal void
PrintMat4(mat4 Mat){
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			printf("%3.2f ", (double)Mat.e[i*4 + j]);
		}
		printf("\n");
	}
}

internal mat3
Mat3Ident(){
	mat3 Result = {};
	Result._11 = 1;
	Result._22 = 1;
	Result._33 = 1;
	return Result;
}

internal mat4
Mat4Ident(){
	mat4 Result = {};
	Result._11 = 1;
	Result._22 = 1;
	Result._33 = 1;
	Result._44 = 1;
	return Result;
}

inline mat4
Mat4Translate(float Tx, float Ty, float Tz){
	return mat4{1, 0, 0, Tx,
				0, 1, 0, Ty,
				0, 0, 1, Tz,
				0, 0, 0, 1};
}

inline mat4
Mat4Translate(vec3 T){
	return Mat4Translate(T.X, T.Y, T.Z);
}

inline mat4 
Mat4RotateY(float Angle) { 
	Angle = (Angle * 3.14159f) / 180.0f;
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);

	return mat4{Cos, 0, Sin, 0,
				  0, 1,   0, 0,
			   -Sin, 0, Cos, 0,
			   	  0, 0,   0, 1};
}
inline mat4 
Mat4RotateZ(float Angle) { 
	Angle = (Angle * 3.14159f) / 180.0f;
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);

	return mat4{Cos, -Sin, 0, 0,
			    Sin,  Cos, 0, 0,
			      0,    0, 1, 0,
			   	  0,    0, 0, 1};
}
inline mat4 
Mat4RotateX(float Angle) { 
	Angle = (Angle * 3.14159f) / 180.0f;
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);

	return mat4{  1,   0,    0, 0,
			      0, Cos, -Sin, 0,
			      0, Sin,  Cos, 0,
			   	  0,   0,    0, 1};
}

inline mat4
Mat4Camera(vec3 P, vec3 Dir){
	Dir.Y = 0;
	vec3 NewZ = -Normalized(Dir);
	vec3 NewX = {NewZ.Z, 0, -NewZ.X};
	//printf("NewX: {%f, %f, %f}\n", (double)NewX.X, (double)NewX.Y, (double)NewX.Z);
	//printf("NewZ: {%f, %f, %f}\n\n", (double)NewZ.X, (double)NewZ.Y, (double)NewZ.Z);
	
	//if the back and right vectors are unit and perpendicular, this is the inverse
	mat4 OrientCamera = mat4{	NewX.X, NewX.Y, NewX.Z, 0,
					     		     0,      1,      0, 0,
								NewZ.X, NewZ.Y, NewZ.Z, 0,
								     0, 0,      0, 1};
	return MulMat4(OrientCamera, Mat4Translate(-P));
}

#endif //MATRIX_CPP
