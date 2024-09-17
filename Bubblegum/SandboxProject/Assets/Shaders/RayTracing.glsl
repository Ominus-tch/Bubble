#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;  // Position of the vertices of the full-screen quad
layout(location = 1) in vec2 a_UV;        // UV coordinates for screen-space mapping

layout(location = 0) out vec2 fragUV;  // Pass UV to the fragment shader

void main()
{
    // Pass through the UV coordinates
    fragUV = a_UV;

    // Output the vertex position in clip space
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) in vec2 fragUV;                // UV coordinates from the vertex shader
layout(location = 0) out vec4 fragColor;            // Final fragment color output

layout(std140, binding = 0) uniform Camera {
    vec4 u_CameraPosition;
    vec4 u_ViewParams;
    mat4 u_CameraToWorldMatrix;
};

layout(std140, binding = 1) uniform Environment {
    vec4 GroundColour;
    vec4 SkyColourHorizon;
    vec4 SkyColourZenith;
    int EnvironmentEnabled;
    float SunFocus;
    float SunIntensity;
};

// Ray structure for ray tracing
struct Ray {
    vec3 origin;
    vec3 dir;
};

struct RayTracingMaterial
{
    vec4 colour;
    vec4 emissionColour;
    vec4 specularColour;
    float emissionStrength;
    float smoothness;
    float specularProbability;
    int flag;
};

struct Sphere
{
    vec3 position;
    float radius;
    RayTracingMaterial material;
};

struct HitInfo {
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
    RayTracingMaterial material;
};

layout (std430, binding = 0) buffer SpheresBuffer {
    Sphere Spheres[];
};

layout(std140, binding = 2) uniform WorldData {
	int MaxBounceCount;
	int NumRaysPerPixel;
	int Frame;
	int NumSpheres;
	ivec2 PixelCount;
};

const float PI = 3.1415926535;

vec3 GetEnvironmentLight(Ray ray) {
    if (EnvironmentEnabled == 0) {
        return vec3(0);
    }

    float skyGradientT = pow(smoothstep(0.0, 0.4, ray.dir.y), 0.35);
    float groundToSkyT = smoothstep(-0.01, 0.0, ray.dir.y);
    vec3 skyGradient = mix(SkyColourHorizon.rgb, SkyColourZenith.rgb, skyGradientT);
    float sun = pow(max(0.0, dot(ray.dir, vec3(0.0, 1.0, 0.0))), SunFocus) * SunIntensity;
    vec3 composite = mix(GroundColour.rgb, skyGradient, groundToSkyT) + sun * (groundToSkyT >= 1.0 ? 1.0 : 0.0);
    return composite;
}

mat3 RotationMatrix(vec3 rotation) {
    float pitch = rotation.x;  // Pitch (rotation around the x-axis)
    float yaw = rotation.y;    // Yaw (rotation around the y-axis)
    float roll = rotation.z;   // Roll (rotation around the z-axis)

    // Rotation around the X axis (pitch)
    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(pitch), -sin(pitch),
        0.0, sin(pitch), cos(pitch)
    );

    // Rotation around the Y axis (yaw)
    mat3 rotY = mat3(
        cos(yaw), 0.0, sin(yaw),
        0.0, 1.0, 0.0,
        -sin(yaw), 0.0, cos(yaw)
    );

    // Rotation around the Z axis (roll)
    mat3 rotZ = mat3(
        cos(roll), -sin(roll), 0.0,
        sin(roll), cos(roll), 0.0,
        0.0, 0.0, 1.0
    );

    // Final rotation matrix (yaw * pitch * roll)
    return rotZ * rotY * rotX;
}

// Ray-Sphere intersection function
HitInfo RaySphere(Ray ray, vec3 sphereCentre, float sphereRadius) {
    HitInfo hitInfo;
    vec3 oc = ray.origin - sphereCentre;

    float a = dot(ray.dir, ray.dir);
    float b = 2.0 * dot(oc, ray.dir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant >= 0.0) {
        float dst = (-b - sqrt(discriminant)) / (2.0 * a);

        if (dst >= 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphereCentre);
        }
    }


    return hitInfo;
}

uint NextRandom(inout uint state) {
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return result;
}

float RandomValue(inout uint state) {
    return NextRandom(state) / 4294967295.0;
}

float RandomValueNormalDistribution(inout uint state) {
    float theta = 2.0 * PI * RandomValue(state);
    float rho = sqrt(-2.0 * log(RandomValue(state)));
    return rho * cos(theta);
}

vec3 RandomDirection(inout uint state) {
    float x = RandomValueNormalDistribution(state);
    float y = RandomValueNormalDistribution(state);
    float z = RandomValueNormalDistribution(state);
    return normalize(vec3(x, y, z));
}

vec2 RandomPointInCircle(inout uint rngState) {
    float angle = RandomValue(rngState) * 2.0 * PI;
    vec2 pointOnCircle = vec2(cos(angle), sin(angle));
    return pointOnCircle * sqrt(RandomValue(rngState));
}

HitInfo CalculateRayCollision(Ray ray) {
    HitInfo closestHit;
    closestHit.didHit = false;
    closestHit.dst = 1e9;

	for (int i = 0; i < NumSpheres; i++) {
        Sphere sphere = Spheres[i];
        HitInfo hitInfo = RaySphere(ray, sphere.position, sphere.radius);
        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
            closestHit = hitInfo;
            closestHit.material = sphere.material;
        }
    }

    return closestHit;
}

vec3 Trace(Ray ray, inout uint rngState)
{
    vec3 incomingLight = vec3(0);
    vec3 rayColour = vec3(1);

	for (int bounceIndex = 0; bounceIndex <= MaxBounceCount; bounceIndex++) {
		HitInfo hitInfo = CalculateRayCollision(ray);

		if (hitInfo.didHit) {
			RayTracingMaterial material = hitInfo.material;
			//incomingLight += vec3(1) * rayColour;

			bool isSpecularBounce = material.specularProbability >= RandomValue(rngState);

			ray.origin = hitInfo.hitPoint;
			vec3 diffuseDir = normalize(hitInfo.normal);
			vec3 specularDir = reflect(ray.dir, hitInfo.normal);
			ray.dir = normalize(mix(diffuseDir, specularDir, material.smoothness * float(isSpecularBounce)));

			vec3 emittedLight = material.emissionColour.rgb * material.emissionStrength;
			incomingLight += emittedLight * rayColour;
			rayColour *= mix(material.colour.rgb, material.specularColour.rgb, float(isSpecularBounce));

			//float p = max(rayColour.r, max(rayColour.g, rayColour.b));
			//if (RandomValue(rngState) >= p) {
            //    break;
            //}

			//rayColour *= 1.0 / p;
		} else {
			incomingLight += GetEnvironmentLight(ray) * rayColour;
			break;
		}
	}

    return incomingLight;
}

void main() {

    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    uint pixelIndex = uint(pixelCoord.y * PixelCount.x + pixelCoord.x);
    uint rngState = pixelIndex + uint(Frame) * 719393;

    vec3 viewPointLocal = vec3(fragUV * 2.0 - 1.0, 1.0) * u_ViewParams.xyz;
    vec3 viewPoint = (u_CameraToWorldMatrix * vec4(viewPointLocal, 1.0)).xyz;
	vec3 camRight = vec3(u_CameraToWorldMatrix[0][0], u_CameraToWorldMatrix[1][0], u_CameraToWorldMatrix[2][0]);
    vec3 camUp = vec3(u_CameraToWorldMatrix[0][1], u_CameraToWorldMatrix[1][1], u_CameraToWorldMatrix[2][1]);

    Ray ray;
	vec3 totalIncomingLight = vec3(0);

    for (int rayIndex = 0; rayIndex < NumRaysPerPixel; rayIndex++) {
        vec2 defocusJitter = RandomPointInCircle(rngState) * 0.01 / float(PixelCount.x);
        ray.origin = vec3(0.0) + camRight * defocusJitter.x + camUp * defocusJitter.y;

        vec2 jitter = RandomPointInCircle(rngState) * 0.01 / float(PixelCount.x);
        vec3 jitteredFocusPoint = viewPoint + camRight * jitter.x + camUp * jitter.y;
        ray.dir = normalize(jitteredFocusPoint - ray.origin);

        totalIncomingLight += Trace(ray, rngState);
    }

    // Check if the ray hits the sphere
    vec3 col = totalIncomingLight / float(NumRaysPerPixel);
    fragColor = vec4(col, 1.0);
}
