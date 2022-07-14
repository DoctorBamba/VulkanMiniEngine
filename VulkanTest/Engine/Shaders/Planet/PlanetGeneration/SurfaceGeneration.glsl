#pragma once
//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//
/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;
float snoise(vec3 p)
{
	vec3 s = floor(p + dot(p, vec3(F3)));
	vec3 x = p - s + dot(s, vec3(G3));
	 
	vec3 e = step(vec3(0.0), x - x.yzx);
	vec3 i1 = e*(1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	vec3 x1 = x - i1 + G3;
	vec3 x2 = x - i2 + 2.0 * G3;
	vec3 x3 = x - 1.0 + 3.0 * G3;
	 
	vec4 w, d;
	 
	w.x = dot(x, x);
	w.y = dot(x1, x1);
	w.z = dot(x2, x2);
	w.w = dot(x3, x3);
	 
	w = max(0.6 - w, 0.0);
	w *= w;
	w *= w;

	vec3 grad0  = random3(s);
	vec3 grad1 = random3(s + i1);
	vec3 grad2 = random3(s + i2);
	vec3 grad3 = random3(s + 1.0);

	d.x = dot(grad0, x);
	d.y = dot(grad1, x1);
	d.z = dot(grad2, x2);
	d.w = dot(grad3, x3);

	return 52.0 * dot(w, d);
}

float smooth_clamp(float t)
{
	if(t > 1.0)
		return 1.0;

	if(t < 0.0)
		return 0.0;

	return t*t*(3-t-t);
}

float PlanetMap(in vec3 point)
{
    float map = 0.0;

	const float bieom_size = 200.0;
	const float bieom_rate = 100.0 / bieom_size;

	float biome_hige		= snoise(point * bieom_rate) + snoise(point * 2 * bieom_rate);
	float biome_temperature = snoise((point + vec3(-1.3, 1.5, 7.9)) * bieom_rate);


	map += 4.0  * snoise((point + vec3(-1.3, 1.5, 7.9)) * 8) + 4;
	map += 4.0  * snoise((point + vec3(-14.3, 1.5, 7.9)) * 16);
	map += 0.2  * snoise((point + vec3(-1.3, 1.5, 7.9)) * 96);

	map = smooth_clamp(biome_hige + 0.4) * map - 1.0;

    return map;
}