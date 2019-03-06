
#include "image.h"
#include "kdtree.h"
#include "ray.h"
#include "raytracer.h"
#include "scene_types.h"
#include <stdio.h>

#include <glm/gtc/epsilon.hpp>
#include <iostream>

/// acne_eps is a small constant used to prevent acne when computing
/// intersection
//  or boucing (add this amount to the position before casting a new ray !
const float acne_eps = 1e-4;
//Get the direction from p2 to p1, substracting them.

/* Texturing */
color3 applyImgTexObject(const Intersection &intersection){
    if (!intersection.mat->hasImgTexture){
        return intersection.mat->diffuseColor;
    }

    switch(intersection.obj->geom.type){
        case Etype::SPHERE:
            return applyImgTexSphere(intersection);
        default:
            return intersection.mat->diffuseColor;
    }
}

void findUVSphere(const Intersection &intersection, float &u, float &v, float &phiR, float &thetaR){
    vec3 Vp(0.f,-1.f,0.f), Ve(0.f, 0.f, 1.f);
    float U = 0.f, V = 0.f, phi = 0.f, theta = 0.f;
    vec3 N = intersection.baseNormal;

    phi = acosf(dot(-N, Vp));
    V = phi * (1.f/glm::pi<float>());

    if (V != 0.0f && V != 1.0f) {
        theta = acosf(dot(Ve, N) / sinf(phi)) / (2.f * glm::pi<float>());
        (dot(cross(Vp, Ve), N) > 0.0f) ? U = theta : U = 1.f - theta;
    }

    phiR = phi;
    thetaR = theta;
    u = U;
    v = V;
}

void findUVSphere(const Intersection &intersection, float &u, float &v){
    vec3 Vp(0.f,-1.f,0.f), Ve(0.f, 0.f, 1.f);
    float U = 0.f, V = 0.f, phi = 0.f, theta = 0.f;
    vec3 N = intersection.baseNormal;

    phi = acosf(dot(-N, Vp));
    V = phi * (1.f/glm::pi<float>());

    if (V != 0.0f && V != 1.0f) {
        theta = acosf(dot(Ve, N) / sinf(phi)) / (2.f * glm::pi<float>());
        (dot(cross(Vp, Ve), N) > 0.0f) ? U = theta : U = 1.f - theta;
    }

    u = U;
    v = V;
}

void applyBumpTexSphere(Intersection *intersection){
    if (!intersection->mat->hasBumpTexture){
        intersection->normal = intersection->baseNormal;
        return;
    }
    float U = 0.f, V = 0.f, phi = 0.f, theta = 0.f;

    findUVSphere(*intersection, U, V, phi, theta);

    auto x = (size_t) (U*float(intersection->mat->bump_texture->width));
    auto y = (size_t) (V*float(intersection->mat->bump_texture->height));

    phi += GLM_CONFIG_PRECISION_FLOAT;
    theta += GLM_CONFIG_PRECISION_FLOAT;

    //Applying map n
    float r = intersection->obj->geom.sphere.radius;
    float phi90 = glm::pi<float>()/2.f;
    mat3 rot = mat3(
            cosf(phi90), -sinf(phi90), 0.f,
            sinf(phi90), cosf(phi90), 0.f,
            0.f,0.f,1.f);
    vec3 p(r*cosf(theta)*cosf(phi),r*sinf(theta),r*cosf(theta)*sinf(phi));
    vec3 t(normalize(rot*intersection->baseNormal));
    //vec3 t(normalize(cross(vec3(0.f, -1.f, 0.f), (p - intersection->obj->geom.sphere.center))));
    vec3 b(normalize(cross(intersection->baseNormal, t)));
    vec3 map_n(normalize(*getPixelPtr(intersection->mat->bump_texture, x, y)));
    map_n = normalize((map_n*2.f) - vec3(1.f, 1.f, 1.f));
    mat3 tbn(t, b, intersection->baseNormal);
    intersection->normal = normalize(tbn * map_n);
}

color3 applyImgTexSphere(const Intersection &intersection){
	float U = 0.f, V = 0.f;

	findUVSphere(intersection, U, V);

	auto x = (size_t) (U*float(intersection.mat->image_texture->width));
	auto y = (size_t) (V*float(intersection.mat->image_texture->height));

	return *getPixelPtr(intersection.mat->image_texture, x, y);
}

bool intersectPlane(Ray *ray, Intersection *intersection, Object *obj) {
	vec3 n = normalize(obj->geom.plane.normal);
	float dist = obj->geom.plane.dist;
	float coef = dot(ray->dir, n);
	if (coef == 0.0f) return false;
	float t = -(float(dot(ray->orig, n))+dist)/coef;
	if (t <= ray->tmin || t > ray->tmax) return false;
	ray->tmax = t;
	intersection->position = ray->orig + t*ray->dir;
	intersection->baseNormal = n;
	intersection->normal = n;
	intersection->mat = &obj->mat;
    intersection->obj = obj;
	return true;
}

bool intersectSphere(Ray *ray, Intersection *intersection, Object *obj) {
  vec3 dist = obj->geom.sphere.center-ray->orig;
  float b = dot(ray->dir, dist);
  float del = b*b - dot(dist, dist) + obj->geom.sphere.radius * obj->geom.sphere.radius;
  if (del > 0.0f){
	  float t = (b - sqrtf(del));
	  if (t >= ray->tmax) return false;
	  vec3 pos = ray->orig + t*ray->dir;
	  vec3 n = normalize(pos - obj->geom.sphere.center);
	  if (t <= ray->tmin){
		  t = (b + sqrtf(del));
		  if (t <= ray->tmin || t > ray->tmax) return false;
		  pos = ray->orig + t*ray->dir;
		  n = normalize(obj->geom.sphere.center - pos);
	  }
	  ray->tmax = t;
	  intersection->position = pos;
	  intersection->baseNormal = normalize(n);
	  intersection->mat = &obj->mat;
      intersection->obj = obj;
      applyBumpTexSphere(intersection); //edits normal
	  return true;
  }
  return false;
}

bool intersectScene(const Scene *scene, Ray *ray, Intersection *intersection) {
	bool hasIntersection = false;
	size_t objectCount = scene->objects.size();
	for (size_t i = 0 ; i < objectCount ; ++i){
		switch(scene->objects[i]->geom.type){
            case PLANE:
                if (intersectPlane(ray, intersection, scene->objects[i]))
                    hasIntersection = true;
                break;
            case SPHERE:
                if (intersectSphere(ray, intersection, scene->objects[i]))
                    hasIntersection = true;
                break;
		}
	}
	return hasIntersection;
}

/* ---------------------------------------------------------------------------
 */
/*
 *	The following functions are coded from Cook-Torrance bsdf model
 *description and are suitable only
 *  for rough dielectrics material (RDM. Code has been validated with Mitsuba
 *renderer)
 */

// Shadowing and masking function. Linked with the NDF. Here, Smith function,
// suitable for Beckmann NDF
float RDM_chiplus(float c) { return (c > 0.f) ? 1.f : 0.f; }

/** Normal Distribution Function : Beckmann
 * NdotH : Norm . Half
 */
float RDM_Beckmann(float NdotH, float alpha) {
    float cosS = NdotH * NdotH;
    float tanS = (1.0f-cosS) * 1.0f/(cosS);
    float alphaS = alpha * alpha;
    float D = RDM_chiplus(NdotH) * (expf(-tanS/alphaS) * (1.0f/(glm::pi<float>() * alphaS * (cosS * cosS))));
    return D;
}

// Fresnel term computation. Implantation of the exact computation. we can use
// the Schlick approximation
// LdotH : Light . Half
float RDM_Fresnel(float LdotH, float extIOR, float intIOR) {
    float sinSt = ((extIOR*extIOR)/(intIOR*intIOR)) * (1.0f - LdotH*LdotH);
    if (sinSt > 1.0f)   return 1.0f;
    float cosT = sqrtf(1.0f-sinSt);
    float s1 = (extIOR * LdotH - intIOR * cosT);
    float s2 = (extIOR * LdotH + intIOR * cosT);
    float p1 = (extIOR * cosT - intIOR * LdotH);
    float p2 = (extIOR * cosT + intIOR * LdotH);
    float Rs = (s1 * s1) / (s2 * s2);
    float Rp = (p1 * p1) / (p2 * p2);
    return 0.5f * (Rs + Rp);
}

// DdotH : Dir . Half
// HdotN : Half . Norm
float RDM_G1(float DdotH, float DdotN, float alpha) {
    float tanTheta = sqrtf(1.0f-(DdotN*DdotN))*(1.0f/DdotN);
    float b = 1.0f/(alpha * tanTheta);
    float k = DdotH * (1.0f/DdotN);
    float G1 = RDM_chiplus(k);
    if (b < 1.6f){
        G1 *= (3.535f*b + 2.181f*b*b)/(1.0f+2.276f*b+2.577f*b*b);
    }
    return G1;

}

// LdotH : Light . Half
// LdotN : Light . Norm
// VdotH : View . Half
// VdotN : View . Norm
float RDM_Smith(float LdotH, float LdotN, float VdotH, float VdotN,
                float alpha) {
  return RDM_G1(LdotH, LdotN, alpha) * RDM_G1(VdotH, VdotN, alpha);

}

// Specular term of the Cook-torrance bsdf
// LdotH : Light . Half
// NdotH : Norm . Half
// VdotH : View . Half
// LdotN : Light . Norm
// VdotN : View . Norm
color3 RDM_bsdf_s(float LdotH, float NdotH, float VdotH, float LdotN,
                  float VdotN, Material *m) {

  float D = RDM_Beckmann(NdotH, m->roughness);
  float F = RDM_Fresnel(LdotH, 1.0f, m->IOR);
  float G = RDM_Smith(LdotH, LdotN, VdotH, VdotN, m->roughness);

  return m->specularColor * ((D*F*G)/(4.0f*LdotN*VdotN));

}
// diffuse term of the cook torrance bsdf
color3 RDM_bsdf_d(Intersection *i) {
    return applyImgTexObject(*i) * (1.0f/glm::pi<float>());
}

// The full evaluation of bsdf(wi, wo) * cos (thetai)
// LdotH : Light . Half
// NdotH : Norm . Half
// VdotH : View . Half
// LdotN : Light . Norm
// VdtoN : View . Norm
// compute bsdf * cos(Oi)
color3 RDM_bsdf(float LdotH, float NdotH, float VdotH, float LdotN, float VdotN,
                Intersection *i) {
    return RDM_bsdf_s(LdotH, NdotH, VdotH, LdotN, VdotN, i->mat) + RDM_bsdf_d(i);

}



color3 shade(vec3 n, vec3 v, vec3 l, color3 lc, Intersection *inter) {
    vec3 h = normalize((v+l)/length(v+l));
    float LdotH = dot(l, h);
    float NdotH = dot(n, h);
    float VdotH = dot(v, h);
    float LdotN = dot(l, n);
    float VdotN = dot(v, n);

    return lc * RDM_bsdf(LdotH, NdotH, VdotH, LdotN, VdotN, inter) * LdotN;
}

//! if tree is not null, use intersectKdTree to compute the intersection instead
//! of intersect scene

color3 trace_ray(Scene *scene, Ray *ray, KdTree *tree, float reflCoef) {
    color3 ret = color3(0.0f);
    Intersection intersection;
    Ray shadow;
    Ray reflectedRay;
    Ray transmittedRay;

	if (intersectScene(scene, ray, &intersection)){
	    for (auto &light : scene->lights) {
		    vec3 dist = (light->position - intersection.position);
		    float t = length(dist);
		    vec3 l = normalize(dist*(1.0f/t));
		    vec3 shadowOrig = intersection.position + acne_eps * l;
		    rayInit(&shadow, shadowOrig, l, 0.f, t, ray->depth);
		    Intersection fake;
            if (!intersectScene(scene, &shadow, &fake)) {
                ret += shade(intersection.normal, -ray->dir, l, light->color, &intersection);
            }
		}

        if (ray->depth < 100 && reflCoef > 0.01f){
		    if (intersection.mat->transparency < 1.f) {
			    vec3 reflectDir = normalize(reflect(ray->dir, intersection.normal));
			    vec3 reflectOrig = intersection.position + acne_eps * reflectDir;
			    rayInit(&reflectedRay, reflectOrig, reflectDir, 0.f, 10000.f, ray->depth + 1);
			    float Fr = RDM_Fresnel(dot(reflectDir, intersection.normal), 1.0f, intersection.mat->IOR);
			    ret += (1.f - intersection.mat->transparency) * Fr * trace_ray(scene, &reflectedRay, tree, reflCoef*0.5f) * intersection.mat->specularColor;
		    }

	        if (intersection.mat->transparency > 0.0f){
			    vec3 refractDir = normalize(glm::refract(ray->dir, intersection.normal,1.f/intersection.mat->IOR));
			    vec3 refractOrig = intersection.position + acne_eps * refractDir;
			    rayInit(&transmittedRay, refractOrig, refractDir, 0.f, 10000.f, ray->depth+1);
			    ret += intersection.mat->transparency*trace_ray(scene, &transmittedRay, tree, reflCoef/2.f);//*intersection.mat->specularColor;
	        }

        }

	}else{
	    ret = scene->skyColor;
	}

  return ret;
}

void renderImage(Image *img, Scene *scene) {

  //! This function is already operational, you might modify it for antialiasing
  //! and kdtree initializaion
  float aspect = 1.f / scene->cam.aspect;

  KdTree *tree = NULL;


//! \todo initialize KdTree

  float delta_y = 1.f / (img->height * 0.5f);   //! one pixel size
  vec3 dy = delta_y * aspect * scene->cam.ydir; //! one pixel step
  vec3 ray_delta_y = (0.5f - img->height * 0.5f) / (img->height * 0.5f) * aspect * scene->cam.ydir;

  float delta_x = 1.f / (img->width * 0.5f);
  vec3 dx = delta_x * scene->cam.xdir;
  vec3 ray_delta_x = (0.5f - img->width * 0.5f) / (img->width * 0.5f) * scene->cam.xdir;


  for (size_t j = 0; j < img->height; j++) {
    if (j != 0)
      printf("\033[A\r");
    float progress = (float)j / img->height * 100.f;
    printf("progress\t[");
    int cpt = 0;
    for (cpt = 0; cpt < progress; cpt += 5)
      printf(".");
    for (; cpt < 100; cpt += 5)
      printf(" ");
    printf("]\n");

    #pragma omp parallel for
    for (size_t i = 0; i < img->width; i++) {
      color3 *ptr = getPixelPtr(img, i, j);
      vec3 center = scene->cam.center + ray_delta_x + ray_delta_y;
      int nb_rays = 5;

      //Anti-aliasing
      for (float aj = -0.5f+(1.f/(2.f*nb_rays)) ; aj < .5f ; aj+=1.f/nb_rays){
          for (float ai = -0.5f+(1.f/(2.f*nb_rays)) ; ai < .5f ; ai+=1.f/nb_rays){
              vec3 ray_dir = center + (float(i)+aj) * dx + (float(j)+aj) * dy;

              Ray rx;
              rayInit(&rx, scene->cam.position, normalize(ray_dir));
              *ptr += trace_ray(scene, &rx, tree, 1.0f);
          }
      }
      //average
      *ptr *= (1.f/(nb_rays*nb_rays));

    }
  }
}
