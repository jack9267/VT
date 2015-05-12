#ifndef _GLMATH_H_
#define _GLMATH_H_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef F_PI
#define F_PI 3.14159265358979323846f
#endif

typedef float ang_t;

class ang3_t {
      public:
	ang_t pitch;	// up / down
	ang_t yaw;	// left / right
	ang_t roll;	// fall over

	ang3_t () {
		pitch = 0; yaw = 0; roll = 0;
	};

	ang3_t (ang_t a, ang_t b, ang_t c) {
		pitch = a; yaw = b; roll = c;
	};

	ang_t &operator [] (const long i) {
		return *((&pitch) + i);
	};
};

typedef float vec_t;

class vec3_t {
      public:
	vec_t x, y, z;

	vec3_t () {
		x = 0; y = 0; z = 0;
	};

	vec3_t (vec_t a, vec_t b, vec_t c) {
		x = a; y = b; z = c;
	};

	vec_t &operator [] (const long i) {
		return *((&x) + i);
	};

	const bool operator ==(const vec3_t &v) const {
		return (v.x == x && v.y == y && v.z == z);
	};

	const bool operator !=(const vec3_t &v) const {
		return !(v == *this);
	};

	const vec3_t operator -() const {
		return vec3_t (-x, -y, -z);
	};

	const vec3_t &operator =(const vec3_t &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	};

	const vec3_t &operator +=(const vec3_t &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	};

	const vec3_t &operator -=(const vec3_t &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	};

	const vec3_t &operator *=(const vec_t &s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	};

	const vec3_t &operator /=(const vec_t &s) {
		const vec_t r = 1 / s;

		x *= r;
		y *= r;
		z *= r;
		return *this;
	};

	const vec3_t operator +(const vec3_t &v) const {
		return vec3_t (x + v.x, y + v.y, z + v.z);
	};

	const vec3_t operator -(const vec3_t &v) const {
		return vec3_t (x - v.x, y - v.y, z - v.z);
	};

	const vec3_t operator *(const vec_t &s) const {
		return vec3_t (x * s, y * s, z * s);
	};

	friend inline const vec3_t operator *(const vec_t &s, const vec3_t &v) {
		return v * s;
	};

	const vec3_t operator /(vec_t s) const {
		s = 1 / s;
		return vec3_t (s * x, s * y, s * z);
	};

	const vec3_t cross(const vec3_t &v) const {
		return vec3_t (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	};

	const vec_t dot(const vec3_t &v) const {
		return x * v.x + y * v.y + z * v.z;
	};

	const vec3_t unit() const {
		return (*this) / length();
	};

	void normalize() {
		(*this) /= length();
	};

	const vec_t length() const;

	const bool nearlyEquals(const vec3_t &v, const vec_t e) const;
};

const vec3_t vec3_origin(0, 0, 0);

const vec3_t ProjectPointOnPlane(vec3_t &p, vec3_t &normal);
const vec3_t RotatePointAroundVector(vec3_t &dir, vec3_t &point, float degrees);

void AngleVectors (ang3_t angles, vec3_t *forward, vec3_t *right, vec3_t *up);

#endif // _GLMATH_H_
