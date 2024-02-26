// Structs
typedef struct color_t
{
    float r;
    float g;
    float b;
} color_t;

typedef struct vec3_t
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct camera_t
{
    vec3_t lookFrom;
    vec3_t lookAt;
    vec3_t up;
    vec3_t u, v, w; // Camera orthonormal base
    vec3_t step_u, step_v;
    vec3_t viewportUpperLeft;
    vec3_t defocus_disk_u, defocus_disk_v;
    float fov;
    float defocusAngle;
    float focusDistance;
} camera_t;

typedef struct sphere_t
{
    vec3_t position;
    color_t albedo;
    float radius;
    union { float roughness, fuzziness, refractionIndex; };
    enum { LAMBERTIAN, METAL, DIELECTRIC, NUMBER_OF_MATERIAL } material;     
} sphere_t;

// Types
typedef uchar 	uint8_t;
typedef ushort 	uint16_t;
typedef short 	int16_t;
typedef uint 	uint32_t;

// Prototypes
// Utils.h
float randomFloatInUnitInterval(uint32_t* seed);
float randomFloat(uint32_t* seed, float min, float max);
vec3_t randomInUnitDisk(uint32_t* seed);
vec3_t randomUnitVector(uint32_t* seed);
vec3_t randomInHemisphere(uint32_t* seed, const vec3_t* normal);

float shlickReflectance(float cos_theta, float refractionRation);

vec3_t randomDefocusedRayPosition(uint32_t* seed, const vec3_t* center, const vec3_t* defocus_disk_u, const vec3_t* defocus_disk_v);

// vec3_color.h
float vec3_lengthSquared(const vec3_t* v);
float vec3_magnitude(const vec3_t* v);
float vec3_dot(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_cross(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_reflect(const vec3_t* v, const vec3_t* n);
vec3_t vec3_refract(const vec3_t* uv, const vec3_t* n, float refractionRatio, uint32_t* seed);
uint8_t vec3_isNearZero(const vec3_t* v);
void vec3_normalize(vec3_t* v);
void vec3_opposite(vec3_t* v);
vec3_t vec3_add(const vec3_t* v1, const vec3_t* v2);
vec3_t vec3_sub(const vec3_t* v1, const vec3_t* v2);
void vec3_scalarAdd(vec3_t* v, float s);
void vec3_scalarMul(vec3_t* v, float s);
vec3_t vec3_scalarMul_return(const vec3_t *v, float s);


float color_dot(const color_t* v1, const color_t* v2);
color_t color_add(const color_t* v1, const color_t* v2);
color_t color_mul(const color_t* v1, const color_t* v2);
void color_scalarMul(color_t* v, float s);
color_t color_scalarMul_return(const color_t *v, float s);

// Constant 
const color_t BLACK = { 0.0f, 0.0f, 0.0f };

// Kernel
__kernel void raytracing(	__global color_t* restrict image, 
						 	__global const sphere_t* restrict spheres, 
						 	__global const camera_t* restrict camera, 
							const uint16_t width, 
							const uint16_t height, 
							const uint16_t raysPerPixel, 
							const uint8_t raysDepth, 
							const uint16_t numberOfSpheres) 
{
	ulong gid = get_global_id(0);
	uint i = gid % width;
	uint j = gid / width;
	
	// Rays weight
	const float inv_raysPerPixel = 1.0f / raysPerPixel;

	// Pixel initialisation
	color_t pixelColor = (color_t){ 0.0f, 0.0f, 0.0f };
	uint32_t seed = gid;
	uint16_t k, rayIdx, depthIdx;
    
	for (rayIdx = 0; rayIdx < raysPerPixel; rayIdx++)
	{
		// Pixel position (Ray position in viewport plane)
		vec3_t pixelPosition_u = vec3_scalarMul_return(&camera->step_u, i + randomFloatInUnitInterval(&seed));
		vec3_t pixelPosition_v = vec3_scalarMul_return(&camera->step_v, j + randomFloatInUnitInterval(&seed));
		vec3_t pixelPosition = camera->viewportUpperLeft;
		pixelPosition = vec3_add(&pixelPosition, &pixelPosition_u);
		pixelPosition = vec3_add(&pixelPosition, &pixelPosition_v);

		// Ray Initialisation
		vec3_t rayPosition = (camera->defocusAngle <= 0.0f) ? camera->lookFrom : randomDefocusedRayPosition(&seed, &camera->lookFrom, &camera->defocus_disk_u, &camera->defocus_disk_v);
		vec3_t rayDirection = vec3_sub(&pixelPosition, &rayPosition);
		color_t rayColor = { 1.0f, 1.0f, 1.0f };

		// Bounce loop
		uint8_t isSkyHit = 0;
		for (depthIdx = 0; depthIdx < raysDepth && !isSkyHit; depthIdx++)
		{
			// Iterate through spheres to get the closest one
			float closestSphereDistance = INFINITY;
			int16_t closestSphereIndex = -1;
			for (k = 0; k < numberOfSpheres; k++)
			{   
				// Maths (line == sphere equation)
				vec3_t originToCenter = vec3_sub(&rayPosition, &spheres[k].position);
				float a = vec3_dot(&rayDirection, &rayDirection);
				float half_b = vec3_dot(&originToCenter, &rayDirection);
				float c = vec3_dot(&originToCenter, &originToCenter) - spheres[k].radius * spheres[k].radius;
				float delta = half_b*half_b - a*c;

				// 1 or 2 solutions => sphere hit
				if (delta >= 0)
				{
					float newDistance = (-half_b - sqrt(delta))/ a;

					// Ignore digital noise
					if (newDistance <=  0.001f)
						continue;

					// Update sphere
					if (newDistance < closestSphereDistance)
					{
						closestSphereDistance = newDistance;
						closestSphereIndex = k;
					}
				}
			}

			// If sphere hit, else sky hit
			if (closestSphereIndex != -1)
			{
				// Ray-sphere hit position
				vec3_t hitPosition = vec3_scalarMul_return(&rayDirection, closestSphereDistance);
				hitPosition = vec3_add(&rayPosition, &hitPosition);

				// Get sphere normal on hit position
				vec3_t sphereNormal = vec3_sub(&hitPosition, &spheres[closestSphereIndex].position);
				vec3_scalarMul(&sphereNormal, 1.0f / spheres[closestSphereIndex].radius);   

				// Which face hit ?
				uint8_t isFrontFace = vec3_dot(&rayDirection, &sphereNormal) > 0 ? 0 : 1;

				// The ray hit a sphere => Update ray position + direction & add color info
				rayPosition = hitPosition;
				switch (spheres[closestSphereIndex].material)
				{
					case LAMBERTIAN:
						// Bad hemisphere diffusion  
						// rayDirection = randomInHemisphere(&seed, &sphereNormal); 
						
						// True Lambertian diffusion
						rayDirection = randomUnitVector(&seed);
						vec3_scalarMul(&rayDirection, spheres[closestSphereIndex].roughness);
						rayDirection = vec3_add(&rayDirection, &sphereNormal);      
						if (vec3_isNearZero(&rayDirection))
							rayDirection = sphereNormal;
						break;
						
					case METAL:
					{
						vec3_t roughnessVector = randomUnitVector(&seed);
						vec3_scalarMul(&roughnessVector, spheres[closestSphereIndex].fuzziness);
						rayDirection = vec3_reflect(&rayDirection, &sphereNormal);
						rayDirection = vec3_add(&rayDirection, &roughnessVector);
						break;
					}

					case DIELECTRIC:
					{

						float refractionRatio = isFrontFace ? 1.0f / spheres[closestSphereIndex].refractionIndex : spheres[closestSphereIndex].refractionIndex;
						vec3_t uRayDirection = rayDirection;
						vec3_normalize(&uRayDirection);
						rayDirection = vec3_refract(&uRayDirection, &sphereNormal, refractionRatio, &seed);
						break;
					}
					
					default:
						printf("ERROR::BAD_SPHERE_MATERIAL: %d\n", spheres[closestSphereIndex].material);
						break;
				}
				rayColor = color_mul(&rayColor, &spheres[closestSphereIndex].albedo);
			}
			else
			{
				// The ray hit the sky, the loop must stop
				isSkyHit = 1;
				float skyGradiant = 0.5f * (rayDirection.y / vec3_magnitude(&rayDirection) + 1.0f);
				const color_t skyColor = (color_t){ (1.0f - skyGradiant)*1.0f + skyGradiant*0.5f, (1.0f - skyGradiant)*1.0f + skyGradiant*0.7f, (1.0f- skyGradiant)*1.0f + skyGradiant*1.0f };
				rayColor = color_mul(&rayColor, &skyColor);
			}
		}

		if (isSkyHit)
			pixelColor = color_add(&pixelColor, &rayColor);
		else 
			pixelColor = color_add(&pixelColor, &BLACK);
	}
	color_scalarMul(&pixelColor, inv_raysPerPixel);
	image[i + j * width] = pixelColor;

}

// Functions
// utils.c

// vec3_color.c
static uint32_t pcg_hash(uint32_t* seed)
{
    uint32_t state = *seed;
    *seed = *seed * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}


float randomFloatInUnitInterval(uint32_t* seed)
{
    return (float)pcg_hash(seed) * (1.0f / 0xffffffffu);
}

float randomFloat(uint32_t *seed, float min, float max)
{
    return min + (max - min) * randomFloatInUnitInterval(seed);
}

vec3_t randomInUnitDisk(uint32_t *seed)
{
    float theta = 2.0f * M_PI * randomFloatInUnitInterval(seed);
    float r = sqrt(randomFloatInUnitInterval(seed));
    return (vec3_t){ r * cos(theta), r * sin(theta), 0.0f };
}

vec3_t randomUnitVector(uint32_t *seed)
{
    float theta = 2.0f * M_PI * randomFloatInUnitInterval(seed);
    float phi = acos(1.0f - 2.0f * randomFloatInUnitInterval(seed));
    vec3_t v;
    v.x = cos(theta) * sin(phi);
    v.y = sin(theta) * sin(phi);
    v.z = cos(phi);
    return v;
}

vec3_t randomInHemisphere(uint32_t *seed, const vec3_t *normal)
{
    vec3_t v = randomUnitVector(seed);

    // The hemisphere is OUT of the sphere
    if (vec3_dot(&v, normal) < 0)
        vec3_opposite(&v);
    return v;
}

float shlickReflectance(float cos_theta, float refractionRatio)
{ 
    float r0 = (1.0f - refractionRatio) / (1.0f + refractionRatio);
    r0 *= r0;
    return r0 + (1.0f - r0) * pow((1.0f - cos_theta) ,5);
}

vec3_t randomDefocusedRayPosition(uint32_t *seed, const vec3_t *center, const vec3_t *defocus_disk_u, const vec3_t *defocus_disk_v)
{
    const vec3_t v = randomInUnitDisk(seed);
    const vec3_t randomDefocus_u = vec3_scalarMul_return(defocus_disk_u, v.x);
    const vec3_t randomDefocus_v = vec3_scalarMul_return(defocus_disk_v, v.y);
    const vec3_t defocusedRayPosition = vec3_add(&randomDefocus_u, &randomDefocus_v);
    return vec3_add(center, &defocusedRayPosition);
}

// vec3_color.c
float vec3_lengthSquared(const vec3_t *v)
{
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

float vec3_magnitude(const vec3_t *v)
{
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

float vec3_dot(const vec3_t* v1, const vec3_t* v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

vec3_t vec3_cross(const vec3_t *v1, const vec3_t *v2)
{
    return (vec3_t){ v1->y * v2->z - v1->z * v2->y,
                     v1->z * v2->x - v1->x * v2->z,
                     v1->x * v2->y - v1->y * v2->x };
}

vec3_t vec3_reflect(const vec3_t *v, const vec3_t *n)
{
    vec3_t temp = vec3_scalarMul_return(n, 2.0f * vec3_dot(v, n));
    return vec3_sub(v, &temp);
}

vec3_t vec3_refract(const vec3_t *uv, const vec3_t *n, float refractionRatio, uint32_t* seed)
{
    // Perpendicular component
    vec3_t refractPerpendicular = *uv;
    vec3_opposite(&refractPerpendicular);
    float cos_theta = min(vec3_dot(&refractPerpendicular, n), 1.0f);
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);
    if (sin_theta * refractionRatio > 1.0f || shlickReflectance(cos_theta, refractionRatio) > randomFloatInUnitInterval(seed))
        return vec3_reflect(uv, n);
    refractPerpendicular = vec3_scalarMul_return(n, cos_theta);
    refractPerpendicular = vec3_add(&refractPerpendicular, uv);
    vec3_scalarMul(&refractPerpendicular, refractionRatio);

    // Parallel component
    vec3_t refractParallel = vec3_scalarMul_return(n, -sqrt(fabs(1.0f - vec3_lengthSquared(&refractPerpendicular))));

    // Refraction vector
    return vec3_add(&refractPerpendicular, &refractParallel);
}

uint8_t vec3_isNearZero(const vec3_t *v)
{
    const float threshold = 1e-8f;
    return (v->x < threshold) && (v->y < threshold) && (v->z < threshold);
}

void vec3_normalize(vec3_t* v)
{
    float invMag = 1.0f / vec3_magnitude(v);
    v->x *= invMag;
    v->y *= invMag;
    v->z *= invMag;
}

void vec3_opposite(vec3_t *v)
{
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

vec3_t vec3_add(const vec3_t *v1, const vec3_t *v2)
{
    return (vec3_t){ v1->x + v2->x, v1->y + v2->y, v1->z + v2->z };
}

vec3_t vec3_sub(const vec3_t *v1, const vec3_t *v2)
{
    return (vec3_t){ v1->x - v2->x, v1->y - v2->y, v1->z - v2->z };
}

void vec3_scalarAdd(vec3_t *v, float s)
{
    v->x += s;
    v->y += s;
    v->z += s;
}

void vec3_scalarMul(vec3_t *v, float s)
{
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

vec3_t vec3_scalarMul_return(const vec3_t *v, float s)
{
    vec3_t result;
    result.x = v->x * s;
    result.y = v->y * s;
    result.z = v->z * s;
    return result;
}

float color_dot(const color_t* v1, const color_t* v2)
{
    return v1->r * v2->r + v1->g * v2->g + v1->b * v2->b;
}

color_t color_add(const color_t *v1, const color_t *v2)
{
    return (color_t){ v1->r + v2->r, v1->g + v2->g, v1->b + v2->b };
}

color_t color_mul(const color_t *v1, const color_t *v2)
{
    return (color_t){ v1->r * v2->r, v1->g * v2->g, v1->b * v2->b };
}

color_t color_scalarMul_return(const color_t *v, float s)
{
    color_t result;
    result.r = v->r * s;
    result.g = v->g * s;
    result.b = v->b * s;
    return result;
}

void color_scalarMul(color_t *v, float s)
{
    v->r *= s;
    v->g *= s;
    v->b *= s;
}
