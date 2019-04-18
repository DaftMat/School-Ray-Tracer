#ifndef __SCENE_H__
#define __SCENE_H__

#include "defines.h"
#include "image.h"
#include <string>
#include <vector>

// SCENE
typedef struct scene_s Scene;
typedef struct object_s Object;
typedef struct light_s Light;
typedef struct camera_s Camera;

typedef struct material_s {
	float IOR;	//! Index of refraction (for dielectric)
	float roughness; //! 0.001 - 0.01 : very smooth finish with slight imperfections. 0.1 : relatively rough. 0.3-0.7 extremely rough
	color3 specularColor;	//! Specular "albedo"
	color3 diffuseColor;	//! Base color
	float transparency;     //! 0 : not transparent, 1 : totally transparent
	Image *image_texture;   //! Texture that replace diffuse color
	Image *bump_texture;    //! Texture that change normals direction
	Image *spec_texture;    //! Texture that replace specular color
	Image *rough_texture;   //! Texture that set roughness
	bool hasImgTexture;
    bool hasBumpTexture;
	bool hasSpecTexture;
	bool hasRoughTexture;
} Material;

enum Etype {SPHERE=1, PLANE, TRIANGLE};

std::vector<std::string> split(const std::string& str, const std::string& delim);

//! create a new sphere structure
Object* initSphere(point3 center, float radius, Material mat);
Object* initPlane(vec3 normal, float d, Material mat);
Object* initTriangle(vec3 v0, vec3 v1, vec3 v2, Material mat);
void initTriFace(Scene *s, vec3 normal, int res, Material mat, bool sphere, float scale, vec3 centerPos);
void initCube(Scene *s, int res, Material mat, float scale, vec3 centerPos);
void initSphere(Scene *s, int res, Material mat, float scale, vec3 centerPos);
void initComplex(Scene *scene, const std::string &filename, Material mat, float scale, vec3 pos, float angle);

//! release memory for the object obj
void freeObject(Object *obj);

//! init a new light at position with a give color (no special unit here for the moment)
Light* initLight(point3 position, color3 color);

//! release memory for the light
void freeLight(Light *);

// allocate the momery for the scene
Scene *initScene();
void freeScene(Scene *scene);

void setCamera(Scene *scene, point3 position, vec3 at, vec3 up, float fov, float aspect);

//! take ownership of obj freeScene will free obj) ... typically use addObject(scene, initPlane()
void addObject(Scene *scene, Object *obj);

//! take ownership of light : freeScene will free light) ... typically use addObject(scene, initLight()
void addLight(Scene *scene, Light *light);

void setSkyColor(Scene *scene, color3 c);


#endif
