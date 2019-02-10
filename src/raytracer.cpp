
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
const auto PI = 3.141592653589793238462643383279502884197169f;
//Get the direction from p2 to p1, substracting them.

bool intersectPlane(Ray *ray, Intersection *intersection, Object *obj) {
	vec3 n = normalize(obj->geom.plane.normal);
	float dist = obj->geom.plane.dist;
	float coef = dot(ray->dir, n);
	if (coef == 0.0f) return false;
	float t = -(float(dot(ray->orig, n))+dist)/coef;
	if (t <= ray->tmin || t > ray->tmax) return false;
	ray->tmax = t;
	intersection->position = ray->orig + t*ray->dir;
	intersection->normal = n;
	intersection->mat = &obj->mat;
	return true;
}

bool intersectSphere(Ray *ray, Intersection *intersection, Object *obj) {
  vec3 dist = obj->geom.sphere.center-ray->orig;
  float b = dot(ray->dir, dist);
  float del = b*b - dot(dist, dist) + obj->geom.sphere.radius * obj->geom.sphere.radius;
  if (del > 0.0f){
	  float t = (b - sqrtf(del));
	  if (t >= ray->tmax) return false;
	  if (t <= ray->tmin){
		  t = (b + sqrtf(del));
		  if (t <= ray->tmin || t > ray->tmax) return false;
	  }
	  ray->tmax = t;
	  intersection->position = ray->orig + t*ray->dir;
      intersection->normal = normalize(intersection->position-obj->geom.sphere.center);
	  intersection->mat = &obj->mat;
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
    float D = RDM_chiplus(NdotH) * (expf(-tanS/alphaS) * 1.0f/(PI * alphaS * (cosS * cosS)));
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
color3 RDM_bsdf_d(Material *m) {
  return m->diffuseColor * (1.0f/PI);
}

// The full evaluation of bsdf(wi, wo) * cos (thetai)
// LdotH : Light . Half
// NdotH : Norm . Half
// VdotH : View . Half
// LdotN : Light . Norm
// VdtoN : View . Norm
// compute bsdf * cos(Oi)
color3 RDM_bsdf(float LdotH, float NdotH, float VdotH, float LdotN, float VdotN,
                Material *m) {
    return RDM_bsdf_s(LdotH, NdotH, VdotH, LdotN, VdotN, m) + RDM_bsdf_d(m);

}



color3 shade(vec3 n, vec3 v, vec3 l, color3 lc, Material *mat) {
    vec3 h = normalize((v+l)/length(v+l));
    float LdotH = dot(l, h);
    float NdotH = dot(n, h);
    float VdotH = dot(v, h);
    float LdotN = dot(l, n);
    float VdotN = dot(v, n);

    return lc * RDM_bsdf(LdotH, NdotH, VdotH, LdotN, VdotN, mat) * LdotN;
}

//! if tree is not null, use intersectKdTree to compute the intersection instead
//! of intersect scene

color3 trace_ray(Scene *scene, Ray *ray, KdTree *tree) {

  color3 ret = color3(0.0f);
  if (ray->depth > 5)   return ret;
  Intersection intersection;

	if (intersectScene(scene, ray, &intersection)){
		color3 cd(0.0f);
		color3 cr(0.0f);
		color3 ct(0.0f);
	    for (auto &light : scene->lights) {
		    vec3 dist = (light->position - intersection.position);
		    float t = length(dist);
		    vec3 l = normalize(dist*(1.0f/t));
		    Ray shadow;
		    shadow.orig = intersection.position + acne_eps * l;
		    shadow.dir = l;
		    shadow.tmin = 0.0f;
		    shadow.tmax = t;
		    Intersection fake;
		    if (!intersectScene(scene, &shadow, &fake))
		        cd += shade(intersection.normal, -ray->dir, l, light->color, intersection.mat);
		}

		Ray reflectedRay;
	    reflectedRay.dir = normalize(reflect(ray->dir, intersection.normal));
        reflectedRay.orig = intersection.position + acne_eps * normalize(intersection.normal);
	    reflectedRay.tmin = 0.0f;
	    reflectedRay.tmax = 10000.f;
	    reflectedRay.depth = ray->depth+1;
	    float Fr = RDM_Fresnel(dot(reflectedRay.dir, intersection.normal), 1.0f, intersection.mat->IOR);
        cr = trace_ray(scene, &reflectedRay, tree);

//	    Ray transmittedRay;
//	    transmittedRay.dir = normalize(refract(ray->dir, intersection.normal, intersection.mat->IOR));
//	    transmittedRay.orig = intersection.position + acne_eps * normalize(-intersection.normal);
//	    transmittedRay.tmin = 0.0f;
//	    transmittedRay.tmax = 10000.f;
//        transmittedRay.depth = ray->depth+1;
//	    //float Ft = RDM_Fresnel(dot(transmittedRay.dir, intersection.normal), 1.0f, intersection.mat->IOR);
//	    ct = trace_ray(scene, &transmittedRay, tree);

	    ret = cd + (Fr*cr);// - ct*intersection.mat->transparency;
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

      //Anti-aliasing : cut each pixel in 15 parts, and get the average
      for (float aj = -1.f ; aj < 1.1f ; aj+=.5f){
          for (float ai = -1.f ; ai < 1.1f ; ai+=.5f){
              //I decided to adapt a little the sampling technique, getting parts of pixels closer to the center of this one.
              vec3 ray_dir = scene->cam.center + ray_delta_x + ray_delta_y + float(i) * dx + float(j) * dy + ai*delta_x*0.2f + aj*delta_y*0.2f;

              Ray rx;
              rayInit(&rx, scene->cam.position, normalize(ray_dir));
              *ptr += trace_ray(scene, &rx, tree);
          }
      }
      //average : 5*5 iterations.
      *ptr /= 25.f;

    }
  }
}
