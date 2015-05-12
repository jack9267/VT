/*
 * Copyright 2002 - Florian Schulze <crow@icculus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This file is part of vt.
 *
 */

#include <math.h>
#include "glmath.h"

#define RCSID "$Id: glmath.cpp,v 1.2 2002/09/12 09:19:19 crow Exp $"

const vec_t vec3_t::length() const {
	return (vec_t)sqrt((double)this->dot(*this));
};

const bool vec3_t::nearlyEquals(const vec3_t &v, const vec_t e) const {
	return fabs(x - v.x) < e && fabs(y - v.y) < e && fabs(z - v.z) < e;
};

static void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}

const vec3_t ProjectPointOnPlane(vec3_t &p, vec3_t &normal)
{
	vec_t inv_denom = 1.0F / normal.dot(normal);

	vec_t d = normal.dot(p) * inv_denom;

	vec3_t n = normal * inv_denom;

	return p - (n * d);
}

/*
** assumes "src" is normalized
*/
const vec3_t Perpendicular(vec3_t &src)
{
	int pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for ( pos = 0, i = 0; i < 3; i++ )
	{
		if (fabs(src[i]) < minelem)
		{
			pos = i;
			minelem = (float)fabs(src[i]);
		}
	}
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	tempvec = ProjectPointOnPlane(tempvec, src);

	/*
	** normalize the result
	*/
	return tempvec.unit();
}

#ifdef _MSC_VER
#pragma optimize("", off)
#endif

const vec3_t RotatePointAroundVector(vec3_t &dir, vec3_t &point, float degrees)
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	float	angle;
	vec3_t	vr, vup, vf;
	vec3_t	dst;

	vf = dir;

	vr = Perpendicular(dir);
	vup = vr.cross(vf);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	im[0][0] = m[0][0];
	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][1] = m[1][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];
	im[2][2] = m[2][2];

	zrot[0][1] = zrot[0][2] = 0;
	zrot[1][0] = zrot[1][2] = 0;
	zrot[2][0] = zrot[2][1] = 0;
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	angle = degrees * (F_PI / 180.0f);

	zrot[0][0] = (float)cos(angle);
	zrot[0][1] = (float)sin(angle);
	zrot[1][0] = (float)-sin(angle);
	zrot[1][1] = (float)cos(angle);

	R_ConcatRotations( m, zrot, tmpmat );
	R_ConcatRotations( tmpmat, im, rot );

	dst.x = rot[0][0] * point.x + rot[0][1] * point.y + rot[0][2] * point.z;
	dst.y = rot[1][0] * point.x + rot[1][1] * point.y + rot[1][2] * point.z;
	dst.z = rot[2][0] * point.x + rot[2][1] * point.y + rot[2][2] * point.z;

	return dst;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

void AngleVectors (ang3_t angles, vec3_t *forward, vec3_t *right, vec3_t *up)
{
	float	angle;
	float	sr, sp, sy, cr, cp, cy; // might need to be static to help MS compiler fp bugs

	angle = angles.yaw * (F_PI / 180.0f);
	sy = (float)sin(angle);
	cy = (float)cos(angle);
	angle = angles.pitch * (F_PI / 180.0f);
	sp = (float)sin(angle);
	cp = (float)cos(angle);
	angle = angles.roll * (F_PI / 180.0f);
	sr = (float)sin(angle);
	cr = (float)cos(angle);

	if (forward) {
		forward->x = -cp*sy;
		forward->y = sp;
		forward->z = -cp*cy;
	}
	if (right) {
		right->x = cr*cy+sp*sr*sy;
		right->y = cp*sr;
		right->z = -sy*cr+cy*sp*sr;
	}
	if (up) {
		up->x = -sr*cy+cr*sp*sy;
		up->y = cr*cp;
		up->z = sr*sy+cr*cy*sp;
	}
}
