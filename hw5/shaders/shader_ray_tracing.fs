#version 330
out vec4 FragColor;

in vec2 TexCoords;


// You can change the code whatever you want

const int MAX_DEPTH = 4; // maximum bounce
uniform samplerCube environmentMap;


struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    // phong shading coefficients
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;

    // reflect / refract
    vec3 R0; // Schlick approximation
    float ior; // index of refration

    // for refractive material
    vec3 extinction_constant;
    vec3 shadow_attenuation_constant;

    // 0 : phong
    // 1 : refractive dielectric
    // 2 : ground
    // 3 : box
    // 4 : gold 
    // 5 : mirror
    // 6 : lambert
    // 7 : mesh
    // add more
    int material_type;
};

const int material_type_phong = 0;
const int material_type_refractive = 1;

// Just consider point light
struct Light{
    vec3 position;
    vec3 color;
    bool castShadow;
};
uniform vec3 ambientLightColor;

// hit information
struct HitRecord{
    float t;        // distance to hit point
    vec3 p;         // hit point
    vec3 normal;    // hit point normal
    Material mat;   // hit point material
};

// Geometry
struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};

struct Plane {
    vec3 normal;
    vec3 p0;
    Material mat;
};

struct Box {
    vec3 box_min;
    vec3 box_max;
    Material mat;
};

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    // we didn't add material to triangle because it requires so many uniform memory when we use mesh...
};


const int mat_phong = 0;
const int mat_refractive = 1;

uniform Material material_ground;
uniform Material material_box;
uniform Material material_gold;
uniform Material material_dielectric_glass;
uniform Material material_mirror;
uniform Material material_lambert;
uniform Material material_mesh;


Sphere spheres[] = Sphere[](
  Sphere(vec3(1,0.5,-1), 0.5, material_gold),
  Sphere(vec3(-1,0.5,-1), 0.5, material_gold),
  Sphere(vec3(0,0.5,1), 0.5, material_lambert),
  Sphere(vec3(1,0.5,0), 0.5, material_lambert)
);

Box boxes[] = Box[](
  //Box(vec3(0,0,0), vec3(0.5,1,0.5), dielectric),
  Box(vec3(2,0,-3), vec3(3,1,-2), material_box)
);

Plane groundPlane = Plane(vec3(0,1,0), vec3(0,0,0), material_ground);
Triangle mirrorTriangle = Triangle( vec3(-3,0,0), vec3(0,0,-4), vec3(-1, 4, -2));
// mirrorTriangle.mat = material_mirror;

Light lights[] = Light[](
    Light(vec3(3,5,3), vec3(1,1,1), true),
    Light(vec3(-3,5,3), vec3(0.5,0,0), false),
    Light(vec3(-3,5,-3), vec3(0,0.5,0), false),
    Light(vec3(3,5,-3), vec3(0,0,0.5), false)
);

// use this for mesh
/*
layout (std140) uniform mesh_vertices_ubo
{
    vec3 mesh_vertices[500];
};

layout (std140) uniform mesh_tri_indices_ubo
{
    ivec3 mesh_tri_indices[500];
};

uniform int meshTriangleNumber;*/

// Math functions
/* returns a varying number between 0 and 1 */
float rand(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float max3 (vec3 v) {
  return max (max (v.x, v.y), v.z);
}

float min3 (vec3 v) {
  return min (min (v.x, v.y), v.z);
}


uniform vec3 cameraPosition;
uniform mat3 cameraToWorldRotMatrix;
uniform float fovY; //set to 45
uniform float H;
uniform float W;

Ray getRay(vec2 uv){
    vec3 rayOrigin = cameraPosition;

    float Px = (TexCoords.x - 0.5) * 2;
    float Py = (TexCoords.y - 0.5) * 2;
    vec3 rayDirection = vec3(Px, Py, -1);
    rayDirection = cameraToWorldRotMatrix * rayDirection;
    rayDirection = normalize(rayDirection);

    Ray newRay;
    newRay.origin = rayOrigin;
    newRay.direction = rayDirection;
    return newRay;
}


const float bias = 0.0001; // to prevent point too close to surface.

vec3 hitPoint(Ray r, float t){
    return r.origin + t * r.direction;
}

bool sphereIntersect(Sphere sp, Ray r, inout HitRecord hit){
    vec3 oc = r.origin - sp.center;
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - sp.radius * sp.radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) /a;
        if (temp > bias && temp < hit.t) {
            hit.t = temp;
            hit.p = hitPoint(r, hit.t);
            hit.normal = (hit.p - sp.center) / sp.radius;
            hit.mat = sp.mat;
            return true;
        }

        temp = (-b + sqrt(b*b-a*c)) /a;
        if (temp > bias && temp < hit.t) {
            hit.t = temp;
            hit.p = hitPoint(r, hit.t);
            hit.normal = (hit.p - sp.center) / sp.radius;
            hit.mat = sp.mat;
            return true;
        }
    }
    return false;

}

bool planeIntersect(Plane p, Ray r, inout HitRecord hit){
    float t = (p.p0.y - r.origin.y) / r.direction.y;
    if (t > bias && t < hit.t){
        hit.t = t;
        hit.p = hitPoint(r, t);
        hit.mat = p.mat;
        hit.normal = p.normal;
        return true;
    }
    
    return false;
}

bool boxIntersect(Box b, Ray r, inout HitRecord hit){
    float txmin = (b.box_min.x - r.origin.x) / r.direction.x;
    float txmax = (b.box_max.x - r.origin.x) / r.direction.x;

    if(txmin > txmax){
        float temp = txmin;
        txmin = txmax;
        txmax = temp;
    }
    
    float tmin = txmin;
    float tmax = txmax;

    float tymin = (b.box_min.y - r.origin.y) / r.direction.y;
    float tymax = (b.box_max.y - r.origin.y) / r.direction.y;

    if(tymin > tymax){
        float temp = tymin;
        tymin = tymax;
        tymax = temp;
    }

    if((tmin > tymax) || (tymin > tmax)) return false;

    if(tymin > tmin) tmin = tymin;
    if(tymax < tmax) tmax = tymax;

    float tzmin = (b.box_min.z - r.origin.z) / r.direction.z;
    float tzmax = (b.box_max.z - r.origin.z) / r.direction.z;

    if(tzmin > tzmax){
        float temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    }

    if((tmin > tzmax) || (tzmin > tmax)) return false;

    if(tzmin > tmin) tmin = tzmin;
    if(tzmax < tmax) tmax = tzmax;

    if (tmin > bias && tmin < hit.t){
        hit.t = tmin;
        hit.p = hitPoint(r, hit.t);
        hit.mat = b.mat;
        if(hit.p.x < b.box_min.x + bias && hit.p.x > b.box_min.x - bias) hit.normal = vec3(b.box_min.x - b.box_max.x, 0.0, 0.0);
        else if(hit.p.x < b.box_max.x + bias && hit.p.x > b.box_max.x - bias) hit.normal = vec3(b.box_max.x - b.box_min.x, 0.0, 0.0);
        else if(hit.p.y < b.box_min.y + bias && hit.p.y > b.box_min.y - bias) hit.normal = vec3(0.0, b.box_min.y - b.box_max.y, 0.0);
        else if(hit.p.y < b.box_max.y + bias && hit.p.y > b.box_max.y - bias) hit.normal = vec3(0.0, b.box_min.y - b.box_max.y, 0.0);
        else if(hit.p.z < b.box_min.z + bias && hit.p.z > b.box_min.z - bias) hit.normal = vec3(0.0, 0.0, b.box_min.z - b.box_max.z);
        else if(hit.p.z < b.box_max.z + bias && hit.p.z > b.box_max.z - bias) hit.normal = vec3(0.0, 0.0, b.box_max.z - b.box_min.z);

        return true;
    }

    return false;
}

bool triangleIntersect(Triangle tri, Ray r, inout HitRecord hit){
    // TODO:
    vec3 v0v1 = tri.v1 - tri.v0; 
    vec3 v0v2 = tri.v2 - tri.v0; 
    vec3 N = cross(v0v1, v0v2); 
    float NdotRayDirection = dot(N, r.direction); 
    if (NdotRayDirection == 0) return false; 
    float d = dot(-N, tri.v0); 
 
    float temp = -(dot(N, r.origin) + d) / NdotRayDirection; 
 
    if (temp < 0) return false;
 
    vec3 P = r.origin + temp * r.direction; 

    vec3 edge0 = tri.v1 - tri.v0; 
    vec3 vp0 = P - tri.v0; 
    vec3 C = cross(edge0, vp0); 
    if (dot(N, C) < 0) return false;
 
    vec3 edge1 = tri.v2 - tri.v1; 
    vec3 vp1 = P - tri.v1; 
    C = cross(edge1, vp1); 
    if (dot(N, C) < 0)  return false;
 
    vec3 edge2 = tri.v0 - tri.v2; 
    vec3 vp2 = P - tri.v2; 
    C = cross(edge2, vp2); 
    if (dot(N, C) < 0) return false; 

    if(temp > bias && temp < hit.t){
        hit.t = temp;
        hit.p = hitPoint(r, hit.t);
        hit.mat = material_mirror;
        hit.normal = normalize(N);
    }
 
    return true;
}

float schlick(float cosine, float r0) {
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 schlick(float cosine, vec3 r0) {
    return vec3(schlick(cosine, r0.x), schlick(cosine, r0.y), schlick(cosine, r0.z));
}


bool trace(Ray r, out HitRecord hit){
    // TODO: trace single ray.
    bool hit_anything = false;
    hit.t = 1.0/0.0;

    for(int i = 0; i < spheres.length(); i++) {
        if (sphereIntersect(spheres[i], r, hit)) hit_anything = true;
    }
    if(boxIntersect(boxes[0], r, hit)){
        hit_anything = true;
    }
    if(triangleIntersect(mirrorTriangle, r, hit)){
        hit_anything = true;
    }
    if(planeIntersect(groundPlane, r, hit)) {
        hit_anything = true;
    }

    return hit_anything;
}

bool shadowTrace(Light l, HitRecord hit){
    HitRecord shadowHit;
    shadowHit.t  = 1.0/0.0;
    vec3 rayOrigin = hit.p;
    vec3 rayDirection = l.position - hit.p;
    rayDirection = normalize(rayDirection);

    Ray shadowRay;
    shadowRay.origin = rayOrigin;
    shadowRay.direction = rayDirection;

    for(int i = 0; i < spheres.length(); i++) {
        if (sphereIntersect(spheres[i], shadowRay, shadowHit)) return true;
    }
    if(boxIntersect(boxes[0], shadowRay, shadowHit)) return true;
    if(triangleIntersect(mirrorTriangle, shadowRay, shadowHit)) return true;
    if(planeIntersect(groundPlane, shadowRay, shadowHit)) return true;

    return false;
}

vec3 castRay(Ray ray){
    HitRecord hit;
    vec3 col = vec3(0.0, 0.0, 0.0);
    vec3 prevR0 = vec3(1.0, 1.0, 1.0);
    
    for(int bounce = 0; bounce < MAX_DEPTH; bounce++){
        if(trace(ray, hit)){
            bool hitShadow = shadowTrace(lights[0], hit);
            vec3 shadowAttenuation = vec3(1.0f, 1.0f, 1.0f);
            if(hitShadow) shadowAttenuation = hit.mat.shadow_attenuation_constant;

            for(int i = 0; i < lights.length(); i++){
                vec3 Iambi = hit.mat.Ka * ambientLightColor;
                col += prevR0 * Iambi;

                vec3 Lvec = lights[i].position - hit.p;
                Lvec = normalize(Lvec);
                vec3 Nvec = normalize(hit.normal);
                float NL = dot(Nvec, Lvec);
                vec3 Idiff = hit.mat.Kd * lights[i].color;
                Idiff = shadowAttenuation * NL * Idiff;
                col += prevR0 * Idiff;

                vec3 Rvec = reflect(Lvec, Nvec);
                float spec = pow(dot(Rvec, Nvec), hit.mat.shininess);
                vec3 Ispec = shadowAttenuation * spec * hit.mat.Ks * lights[i].color;
                col += prevR0 * Ispec;
            
            }
        }
        else {
            // col += prevR0 * vec3(0.0, 0.5, 0.5);
            col += prevR0 * texture(environmentMap, ray.direction).rgb;
            break;
        }

        Ray newRay;
        newRay.origin = hit.p + 0.00001 * normalize(hit.normal);
        newRay.direction = normalize(reflect(ray.direction, hit.normal));
        ray = newRay;
        // prevR0 = prevR0 * hit.mat.R0;
        float cosine = dot(hit.normal, ray.direction)/(length(hit.normal)* length(ray.direction));
        prevR0 = prevR0 * schlick(cosine, hit.mat.R0);
    }

    return col;
}

void main()
{
    // TODO:

    const int nsamples = 1;
    vec3 color = vec3(0);
    Ray r = getRay(TexCoords);
    color += castRay(r);
    color /= nsamples;
    FragColor = vec4(color, 1.0);
}
