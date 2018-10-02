#if !defined(UTIL_CPP)
#define UTIL_CPP

internal inline int32
MinInt32(int32 a, int32 b){
	if(a < b){
		return a;
	}
	return b;
}

internal inline int32
MaxInt32(int32 a, int32 b){
	if(a > b){
		return a;
	}
	return b;
}
internal inline real32
MinReal32(real32 a, real32 b){
	if(a < b){
		return a;
	}
	return b;
}

internal inline real32
MaxReal32(real32 a, real32 b){
	if(a > b){
		return a;
	}
	return b;
}

internal uint32
CapInt32(int32 a, int32 t, int32 b){
	if(t < a){
		return a;
	}else if (t > b){
		return b; }
	return t;
}

internal inline real32
ClampReal32(real32 t, real32 a, real32 b){
	if(t < a){
		return a;
	}
	if(b < t){
		return b;
	}
	return t;
}

real32 Min3Real32(real32 A, real32 B, real32 C){
	if (A < B){
		if(C < A){
			return C;
		}
		return A;
	}
	//B < A
	if(C < B){
		return C;
	}
	return B;
}
real32 Max3Real32(real32 A, real32 B, real32 C){
	if (A > B){
		if(C > A){
			return C;
		}
		return A;
	}
	//B > A
	if(C > B){
		return C;
	}
	return B;
}

internal inline bool32
IsInBounds(int32 i, int32 j, int32 MaxI, int32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxJ){
		return false;
	}	
	return true;
}

internal inline bool32
IsPInBounds(vec2i P, int32 MaxI, int32 MaxJ){
	return IsInBounds(P.X, P.Y, MaxI, MaxJ);
}

internal inline bool32
IsInBounds(real32 i, real32 j, real32 MaxI, real32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxJ){
		return false;
	}	
	return true;
}

internal inline bool32
IsPInBounds(vec2f P, real32 MaxI, real32 MaxJ){
	return IsInBounds(P.X, P.Y, MaxI, MaxJ);
}

internal bool
IsInsideRect(real32 X, real32 Y, rectangle Rect){
	if(X >= Rect.MinX && X < Rect.MaxX && Y >= Rect.MinY && Y < Rect.MaxY){
		return true;
	}
	return false;
}

internal bool
IsPInsideRect(vec2f P, rectangle Rect){
	return IsInsideRect(P.X, P.Y, Rect);
}

#endif //UTIL_CPP
