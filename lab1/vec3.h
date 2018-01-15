#ifndef __GWU_VEC__
#define __GWU_VEC__

//================================
// vec3
//================================
class vec3 {
public:
	float x, y, z;

public:
	vec3();
	vec3(float x, float y, float z);

	vec3 &			set(float x, float y, float z);

	vec3 &			zero(void);

	vec3			operator- (void) const;
	vec3			operator+ (void) const;

	vec3			operator+ (const vec3 &v) const;
	vec3			operator- (const vec3 &v) const;
	vec3			operator* (float scalar) const;
	vec3			operator/ (float scalar) const;
	//vec3		    operator* (const vec3 &v) const;

	vec3 &			operator= (const vec3 &v);
	vec3 &			operator+=(const vec3 &v);
	vec3 &			operator-=(const vec3 &v);
	vec3 &			operator*=(float scalar);
	vec3 &			operator/=(float scalar);

	float &			operator[](int index);
	const float &	operator[](int index) const;

	float			dot(const vec3 &v) const;

	float			magnitude(void) const;
	vec3 &			normalize(void);
	vec3 &			cross(const vec3 &v) const;
	vec3 &			PerspectiveMatrix(float d, float f, float h) const;
	vec3 &			ViewMatrix(const vec3 &u, const vec3 &v, const vec3 &n, const vec3 &c)  const;
	vec3 &			ModelMatrix(const vec3 &m) const;
	vec3 &			multi(const vec3 &v) const;

	float *			ptr(void);
	const float *	ptr(void) const;


};

vec3::vec3() {
	x = y = z = 0;
}

vec3::vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3 &vec3::set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;

	return *this;
}

vec3 &vec3::zero(void) {
	x = y = z = 0;

	return *this;
}

vec3 vec3::operator-(void) const {
	return vec3(-x, -y, -z);
}

vec3 vec3::operator+(void) const {
	return vec3(x, y, z);
}

vec3 vec3::operator+(const vec3 &v) const {
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3 &v) const {
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(float scalar) const {
	return vec3(x * scalar, y * scalar, z * scalar);
}

vec3 vec3::operator/(float scalar) const {
	float inv = 1.0f / scalar;
	return vec3(x * inv, y * inv, z * inv);
}

vec3 &vec3::operator=(const vec3 &v) {
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

vec3 &vec3::operator+=(const vec3 &v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vec3 &vec3::operator-=(const vec3 &v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3 &vec3::operator*=(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

vec3 &vec3::operator/=(float scalar) {
	float inv = 1.0f / scalar;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}

float &vec3::operator[](int index) {
	assert(index >= 0 && index < 3);
	return (&x)[index];
}

const float &vec3::operator[](int index) const {
	assert(index >= 0 && index < 3);
	return (&x)[index];
}

float vec3::dot(const vec3 &v) const {
	return x * v.x + y * v.y + z * v.z;
}

float vec3::magnitude(void) const {
	return sqrtf(x * x + y * y + z * z);
}

vec3 &vec3::normalize(void) {
	float mag = sqrtf(x * x + y * y + z * z);

	if (mag < 1e-6f) {
		x = y = z = 0;
	}
	else {
		float inv = 1.0f / mag;

		x *= inv;
		y *= inv;
		z *= inv;
	}

	return *this;
}

float *vec3::ptr(void) {
	return &x;
}

const float *vec3::ptr(void) const {
	return &x;
}

//vector1 X vector2
vec3 &vec3::cross(const vec3 &v) const {
	return vec3( (y * (v.z) - z * (v.y)), (z * (v.x) - x * (v.z)), (x * (v.y) - y * (v.x)) );
}

/*
[d/h 0 0 0]               [x]
[0 d/h 0 0]               [y]
[0 0 f/(f-d) -df/(f-d)] * [z] 
[0 0 1 0]                 [1]

because z co-ordinate is in the opposite direction in OpenGL, d and f are opposite
*/
vec3 &vec3::PerspectiveMatrix(float d, float f, float h) const{
	vec3 tempVerts;
	tempVerts.x = (x * d) / (h * z);
	tempVerts.y = (y * d) / (h * z);
	tempVerts.z = ((z - d) * f) / ((f - d) * z);
	return tempVerts;
}

/*
[ux uy uz 0]  [1 0 0 -cx]   [ux uy uz -c * u]
[vx vy vz 0]  [0 1 0 -cy] = [vx vy vz -c * v]
[nx ny nz 0]  [0 0 1 -cz]   [nx ny nz -c * n]
[0  0  0  1]  [0 0 0  1 ]   [0  0  0     1  ]

*/

vec3 &vec3::ViewMatrix(const vec3 &u, const vec3 &v, const vec3 &n, const vec3 &c) const{
	vec3 tempVerts;

	tempVerts.x = x * u.x + y * u.y + z * u.z - c.x * u.x - c.y * u.y - c.z * u.z;
	tempVerts.y = x * v.x + y * v.y + z * v.z - c.x * v.x - c.y * v.y - c.z * v.z;
	tempVerts.z = x * n.x + y * n.y + z * n.z - c.x * n.x - c.y * n.y - c.z * n.z;

	return tempVerts;
}

vec3 &vec3::ModelMatrix(const vec3 &m) const {
	vec3 tempVerts;

	tempVerts.x = x + m.x;
	tempVerts.y = y + m.y;
	tempVerts.z = z + m.z;

	return tempVerts;
}

vec3 &vec3::multi(const vec3 &v) const {
	vec3 tempVerts;

	tempVerts.x = x * v.x;
	tempVerts.y = y * v.y;
	tempVerts.z = z * v.z;

	return tempVerts;
}



#endif // __GWU_VEC__