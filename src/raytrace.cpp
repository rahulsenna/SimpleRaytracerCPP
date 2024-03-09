#pragma once
struct MATERIAL {
    r32 refractive_index = 1;
    v4 albedo = {1,0,0,0};
    v3 diffuse_color;
    r32 specular_exponent;
};
struct SPHERE
{
  v3  C;
  r32 r;
  MATERIAL material;
};

#define RED    {1,0,0}
#define BLUE   {0,0,1}
#define GREEN  {0,1,0}
#define YELLOW {1,1,0}

#include <vector>
#include <iostream>
// #include <stdio.h>

#include <unordered_map>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <ranges>

using namespace std;
using namespace ranges;

typedef struct
{
	u32 v0, v1, v2;
} TRIANGLE;

constexpr r32 kEpsilon = 1e-8;

struct MODEL
{
    std::vector<TRIANGLE> Triangles;
    std::vector<v3> Vertices;

    MODEL(const std::string_view filename)
    {
        ifstream file( filename.data() );
        string line;
        
        r32 v; char c;
        while (getline(file, line))
        {
            stringstream ss(line);
            ss >> c;
            if (c == 'v')
            {
                v3 vert;
                ss >> vert.x >> vert.y >> vert.z;
                Vertices.emplace_back(vert);
            }
            if (c == 'f')
            {
                TRIANGLE T;
                ss >> T.v0 >> T.v1 >> T.v2;
                T.v0--;
                T.v1--;
                T.v2--;
                Triangles.emplace_back(T);
            }
        }
    }
    bool Intersect(const v3 &O, const v3 &D, const v3 &v0, const v3 &v1, const v3 &v2, float &t)
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html
#if 0
 // compute plane's normal
    v3 v0v1 = v1 - v0;
    v3 v0v2 = v2 - v0;
    // no need to normalize
    v3 N = Cross(v0v1, v0v2); // N
    float denom = DotProduct(N, N);
    
    // Step 1: finding P
    
    // check if ray and plane are parallel ?
    float NdotRayDirection = DotProduct(N, D);

    if (fabs(NdotRayDirection) < kEpsilon) // almost 0
        return false; // they are parallel so they don't intersect ! 

    // compute d parameter using equation 2
    float d = -DotProduct(N, v0);
    
    // compute t (equation 3)
    t = -(DotProduct(N, O) + d) / NdotRayDirection;
    
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind
 
    // compute the intersection point using equation 1
    v3 P = O + t * D;
 
    // Step 2: inside-outside test
    v3 C; // vector perpendicular to triangle's plane
 
    // edge 0
    v3 edge0 = v1 - v0; 
    v3 vp0 = P - v0;
    C = Cross(edge0, vp0);
    if (DotProduct(N, C) < 0) return false; // P is on the right side
 
    // edge 1
    v3 edge1 = v2 - v1; 
    v3 vp1 = P - v1;
    C = Cross(edge1, vp1);
    if ((/* u =  */DotProduct(N, C)) < 0)  return false; // P is on the right side
 
    // edge 2
    v3 edge2 = v0 - v2; 
    v3 vp2 = P - v2;
    C = Cross(edge2, vp2);
    if ((/* v =  */DotProduct(N, C)) < 0) return false; // P is on the right side;

    // u /= denom;
    // v /= denom;

    return true; // this ray hits the triangle
#else

    v3 v0v1 = v1 - v0;
    v3 v0v2 = v2 - v0;
    v3 pvec = Cross(D, v0v2);
    r32 det = DotProduct(v0v1, pvec);

    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false;

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;


    v3 tvec = O - v0;
    r32 u = DotProduct(tvec, pvec);
    if (u < 0 || u > det) return false;

    v3 qvec = Cross(tvec,v0v1);
    r32 v = DotProduct(D, qvec);
    if (v < 0 || u + v > det) return false;
    
    t = DotProduct(v0v2, qvec) * (1./det);
    return t>kEpsilon;

#endif
    }
};


enum LightType
{
    AMBIENT_LIGHT = 0x0,
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
};

struct LIGHT
{
  LightType type;
  r32 I;
  v3 Vec;
};


#define CW BUFFER_WIDTH
#define CH BUFFER_HEIGHT

#define VW 1
#define VH 1
#define d  1
#define Vz 1

#define PI_32 3.1415926535897932f


// const float fov      =  PI_32/2.f;
// const float fov      = 0.92502450355699467577f; // 53 deg
// const float fov      = PI_32/1.01f;
const float fov      = PI_32/3.;




MODEL model("../hand.obj");


vector<LIGHT> Lights;
vector<SPHERE> Spheres;

MATERIAL      ivory{1.0f, {0.6f,  0.3f, 0.1f, 0.0f}, {0.4f, 0.4f, 0.3f},   50.f};
MATERIAL red_rubber{1.0f, {0.9f,  0.1f, 0.0f, 0.0f}, {0.3f, 0.1f, 0.1f},   10.f};
MATERIAL     mirror{1.0f, {0.0f, 10.0f, 0.8f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1425.f};
MATERIAL     glass {2.3f, {0.0f,  0.5f, 0.1f, 0.8f}, {0.6f, 0.7f, 0.8f},  125.f};

r32 Intersect(v3 &O, v3 &D, vector<SPHERE> &Spheres, MATERIAL &Material, v3& N, r32 min_dist = 1.f)
{
    r32 closest_t = INFINITY;

    for (auto &Sphere : Spheres)
    {
        v3 CO =  O - Sphere.C;

        // P_ray    = O + (Dt)
        // P_sphere = |C + P| = r^2

        r32 a = DotProduct(D,D);
        r32 b = 2*DotProduct(CO,D);
        r32 c = DotProduct(CO,CO) - Sphere.r*Sphere.r;

        r32 Discriminant = b*b - 4*a*c;

        if (Discriminant>0.f)
        {
            r32 discriminant_sqrt = SquareRoot(Discriminant);
            r32 t1 = (-b + discriminant_sqrt) / 2*a;
            r32 t2 = (-b - discriminant_sqrt) / 2*a;
            if (t2<t1)
                std::swap(t1,t2);

            if (t1 < closest_t && t1 > min_dist)
            {
                Material = Sphere.material;
                closest_t = t1;
                v3 P = O + closest_t*D;
                N = P - Sphere.C;
                Normalize(N);
            }   
        }       
    }

#if 1 
    float tnear = INFINITY;
    #pragma omp parallel for
    for (int fi = 0; fi < model.Triangles.size(); ++fi)
    {
        TRIANGLE T = model.Triangles[fi];
        v3 v0 = model.Vertices[T.v0];
        v3 v1 = model.Vertices[T.v1];
        v3 v2 = model.Vertices[T.v2];
        if (model.Intersect(O, D, v0,v1,v2, tnear) && tnear < closest_t)
        {
            Material  = ivory;
            closest_t = tnear;
            v3 v0v1 = v1 - v0;
            v3 v0v2 = v2 - v0;
            N = Cross(v0v1, v0v2);

            break;   
        }
    }
#endif



    r32 checkerboard_dist = INFINITY;
    if (fabs(D.y)>0.001)
    {

        /* 
        v3 N = {0,1,0};  // Plane Normal
        v3 A = {0,-4,0}; // Point on the Plane
        r32 D_dot_N = DotProduct(D,N);
        if (D_dot_N == 0) return(closest_t); // Make sure not dividing by zero - geometrically means ray is parallel to the plane
        r32 t = DotProduct((A-O),N) / D_dot_N;
        v3 P = O + D*t;
        */

        // Ax+By+Cz+t = 0;
        // 0x+1y+0z+t = 0 when N = <0,1,0>
        // y + t = 0
        // t = -y

        // P = O + D*t;
        // t = (P - O) / D;  // P = (0,4,0)

        r32 t = -(O.y+4)/D.y; // the checkerboard plane has equation y = -4
        v3 P = O + D*t;
        if (t>0.0001 && fabs(P.x)<10 && P.z<-10 && P.z>-30 && t<closest_t)
        {
            checkerboard_dist = t;

            N = {0,1,0};
            Material.diffuse_color = (int(.5*P.x+1000) + int(.5*P.z)) & 1 ? v3{1, 0.73, 0.47} : v3{0.07, 0.31, 0.5};
            // Material.diffuse_color = v3{1, 0.73, 0.47};
            Material.diffuse_color*=.3;
        }
    }
#if 0  // Wall
    if (fabs(D.x)> 0.001 && checkerboard_dist == INFINITY)
    {
        r32 t = -(O.x+10.f)/D.x; // 
        v3 P = O + D*t;
        if (t>0.0001  && t<closest_t && P.y>-4.f && P.y<8.f && P.z<-10.f && P.z>-30.f)
        {
            checkerboard_dist = t;

            N = {1,0,0};
            // Material.diffuse_color = (int(.5*P.y+1000) + int(.5*P.z)) & 1 ? v3{1, 0.73, 0.47} : v3{0.07, 0.31, 0.5};
            Material.diffuse_color = v3{0.2, 0.3, 0.5};
            Material.diffuse_color*=.3;
        }
    }
#endif

    return min(closest_t, checkerboard_dist);
}
v3 Gray = {.3,.3,.5};
v3 White = {1., 1., 1.};

v3 refract(v3 &I, v3 &N,  r32 refractive_index)
{ // Snell's law
    r32 cosi = - max(-1.f, min(1.f, DotProduct(I,N)));
    r32 etai = 1, etat = refractive_index;
    v3 n = N;
    if (cosi < 0) { // if the ray is inside the object, swap the indices and invert the normal to get the correct result
        cosi = -cosi;
        std::swap(etai, etat); n = -N;
    }
    r32 eta = etai / etat;
    r32 k = 1 - eta*eta*(1 - cosi*cosi);
    return k < 0 ? v3{} : I*eta + n*(eta * cosi - sqrtf(k));
}

s32 ImgW,ImgH,BytesPerPixel;    
vector<v3> Envmap_Color;


v3 CastRay(v3& O, v3& D, u32 depth=0)
{
    MATERIAL material;
    v3 N;

    r32 closest_t = Intersect(O, D, Spheres, material, N);

    if (closest_t == INFINITY)
    {
        u32 x =ImgW *(D.x *.5f+.5f);
        u32 y =ImgH *(D.y *.5f+.5f);
        return Envmap_Color[x+y*ImgW];
    }
                        
    v3 V = -D;
    v3 P = O + closest_t*D;


    // Reflection
    v3 ReflectColor = {};
    if (depth>0 && material.albedo.z > 0.f) // if material is not reflective at all, we'll skip the recursion
    {
        v3 ReflectDir = 2 * N * DotProduct(N, V) - V;
        Normalize(ReflectDir);
        ReflectColor = CastRay(P, ReflectDir, depth-1);
    }
    // Refraction
    v3 RefractColor = {};
    if (depth>0 && material.albedo.w > 0.f) // if material is not refractive at all, we'll skip the recursion
    {
        v3 RefractDir = refract(D, N, material.refractive_index);
        Normalize(RefractDir);
        RefractColor = CastRay(P, RefractDir, depth-1);
    }


    r32 diffuse_light_intensity = 0.f;
    r32 specular_light_intensity = 0.f;

    for (auto &Light: Lights)
    {
        if (Light.type == AMBIENT_LIGHT)
        {
            diffuse_light_intensity += Light.I;
            continue;
        }

        // Diffuse
        v3 L = {};
        if (Light.type == DIRECTIONAL_LIGHT)
        { 
            L = Light.Vec;
        }
        if (Light.type == POINT_LIGHT)
        {
            L = Light.Vec - P;
        }
        
        // Normalize(L);
        // Shadow
        MATERIAL shadow_material;
        v3 shadow_N;

        auto shadow_t = Intersect(P, L, Spheres, shadow_material,shadow_N, 0.001f);
        if (shadow_t != INFINITY)
            continue;
        

        r32 N_dot_L = DotProduct(N, L);
        if (N_dot_L > 0.f)
        {
            // cos(NPL) = (N_dot_L / (Length(L) * Length(N)));
            diffuse_light_intensity += Light.I* (N_dot_L / (Length(L) * Length(N)));
        }
        
        if (material.specular_exponent == -1) // No Specular
            continue;

        // Specular
        v3 Ln = N * DotProduct(N,L);
        v3 Lp = L - Ln;
        v3 R =  Ln - Lp;
        // v3 R = 2 * N * DotProduct(N, L) - L;
        
        r32 R_dot_V = DotProduct(R, V);
        if (R_dot_V > 0.f)
        { 
            r32 cosRPV = R_dot_V / (Length(R) * Length(V));
            specular_light_intensity += Light.I * (r32)pow(cosRPV, material.specular_exponent);
        }
    }

    v3 Color = material.diffuse_color * diffuse_light_intensity * material.albedo.x + 
               White * specular_light_intensity * material.albedo.y +
               ReflectColor * material.albedo.z +
               RefractColor * material.albedo.w;

    // Color = Closest_Sphere.material.diffuse_color;
    return (Color);
}

// inline v3 CanvasToViewportVec(s32 x, s32 y)
// {
//     return {(r32)x*VW/CW, (r32)y*VH/CH, -d}; // ScreenSpace = -0.5 to 0.5
// }
inline v3 CanvasToViewportVec(r32 x, r32 y)
{
    r32 w = (2*((x+.5f)/CW)) *tan(fov/2.f)*CW/(r32)CH; // NDC -1.0 to 1.0
    r32 h = (2*((y+.5f)/CH)) *tan(fov/2.f);
    v3 dir  {w, h, -1};
    return(dir);
}


void RayTracer()
{
    loaded_bitmap bitmap = LoadBMP("../OldCityCenter.bmp");
    ImgW = bitmap.Width;
    ImgH = bitmap.Height;
    BytesPerPixel = 4;

    Envmap_Color = vector<v3>(ImgW*ImgH);

    for (u32 i = 0; i < ImgH*ImgW; ++i)
    {
        u8 *pixel = (u8*)bitmap.Memory+i*BytesPerPixel;
        r32 r = *(pixel+2);
        r32 g = *(pixel+1);
        r32 b = *(pixel+0);

        v3 color = {r,g,b};

        color *= 1.f/255.0f;
        Envmap_Color[i] = color;
    }

    Spheres = {
        {{-3,    0,   -16}, 2, ivory},
        {{-1.0, -1.5, -12}, 2, glass},
        {{ 1.5, -0.5, -18}, 3, red_rubber},
        {{ 9,    0,   -18}, 4, mirror},


        {{  0.5, -3.2, -10}, .34, red_rubber},
        {{  3.5, -3.2, -13}, .53, ivory},
        {{  1., -3.2, -9},  .3, ivory},
        {{  1.4, -3.2, -10}, .39, glass},
        {{  1.5, -3.2, -11}, .37, red_rubber},
        {{  2.5, -3.2, -11}, .43, mirror},
        {{  2.5, -3.2, -14}, .36, glass},

        };

    
    Lights = {
        // {AMBIENT_LIGHT, .2f}, 
        // {DIRECTIONAL_LIGHT, .5f, {1, 4,  4}},
        
        {POINT_LIGHT, 1.5f, {-20, 20,  20}},
        {POINT_LIGHT, 1.8f, { 30, 50, -25}},
        {POINT_LIGHT, 1.7f, { 30, 20,  30}},

        };

    
    #pragma omp parallel for
    for (s32 y = -CH/2; y < CH/2; ++y)
    {
        for (s32 x = -CW/2; x < CW/2; ++x)
        {
            // v3 D = CanvasToViewportVec(x,y);
            v3 D = CanvasToViewportVec(x,y);
            Normalize(D);   // Dir Length = 1
            
            v3 Color = CastRay(O, D, 4);
            u32 C = Color2UINT(Color);
            PUT_PIXEL(x,y, C);

        }
    }
}