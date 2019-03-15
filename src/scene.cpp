#include "scene.h"
#include "scene_types.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


/* UTILS */
std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

Object *initSphere(point3 center, float radius, Material mat) {
    Object *ret;
    ret = (Object *)malloc(sizeof(Object));
    ret->geom.type = SPHERE;
    ret->geom.sphere.center = center;
    ret->geom.sphere.radius = radius;
    ret->geom.sphere.dir = vec3(0.f,0.f,-1.f);
    memcpy(&(ret->mat), &mat, sizeof(Material));
    return ret;
}

Object *initPlane(vec3 normal, float d, Material mat) {
    Object *ret;
    ret = (Object *)malloc(sizeof(Object));
    ret->geom.type = PLANE;
    ret->geom.plane.normal = normalize(normal);
    ret->geom.plane.dist = d;
    memcpy(&(ret->mat), &mat, sizeof(Material));
    return ret;
}

Object *initTriangle(vec3 v0, vec3 v1, vec3 v2, Material mat){
    Object *ret;
    ret = (Object *)malloc(sizeof(Object));
    ret->geom.type = TRIANGLE;
    ret->geom.triangle.v0 = v0;
    ret->geom.triangle.v1 = v1;
    ret->geom.triangle.v2 = v2;
    memcpy(&(ret->mat), &mat, sizeof(Material));
    return ret;
}

void initComplex(Scene *scene, const std::string &filename, Material mat, float scale, vec3 pos, float angle){

    std::string line;
    std::ifstream objFile(filename);
    if (objFile.is_open()){
        std::vector<vec3> vertexes;
        vertexes.emplace_back(vec3(0.f));
        while (getline(objFile, line)){
            std::vector<std::string> splittedLine = split(line, " ");
            std::string specifier;
            if (splittedLine.empty()){
                specifier = " ";
            }else{
                specifier = splittedLine[0];
            }
            if (specifier == "v"){
                //the line defines a vertex
                float x = std::stof(splittedLine[1]);
                float y = std::stof(splittedLine[2]);
                float z = std::stof(splittedLine[3]);
                vertexes.emplace_back(pos+(rotate(vec3(x,y,z)*scale, angle, vec3(0.f,1.f,0.f))));
            }else if (specifier == "f"){
                //the line defines a triangle
                std::vector<std::string> strA = split(splittedLine[1], "/");
                vec3 a = vertexes[size_t(std::stoi(strA[0]))];
                std::vector<std::string> strB = split(splittedLine[2], "/");
                vec3 b = vertexes[size_t(std::stoi(strB[0]))];
                std::vector<std::string> strC = split(splittedLine[3], "/");
                vec3 c = vertexes[size_t(std::stoi(strC[0]))];
                addObject(scene, initTriangle(b, a, c, mat));
            }
        }
    }

    objFile.close();
}

void freeObject(Object *obj) {
    free(obj);
}

Light *initLight(point3 position, color3 color) {
    Light *light = (Light*)malloc(sizeof(Light));
    light->position = position;
    light->color = color;
    return light;
}

void freeLight(Light *light) {
    free(light);
}

Scene * initScene() {
    return new Scene;
}

void freeScene(Scene *scene) {
    std::for_each(scene->objects.begin(), scene->objects.end(), freeObject);
    std::for_each(scene->lights.begin(), scene->lights.end(), freeLight);
    delete scene;
}

void setCamera(Scene *scene, point3 position, point3 at, vec3 up, float fov, float aspect) {
    scene->cam.fov = fov;
    scene->cam.aspect = aspect;
    scene->cam.position = position;
    scene->cam.zdir = normalize(at-position);
    scene->cam.xdir = normalize(cross(up, scene->cam.zdir));
    scene->cam.ydir = normalize(cross(scene->cam.zdir, scene->cam.xdir));
    scene->cam.center = 1.f / tanf ((scene->cam.fov * glm::pi<float>() / 180.f) * 0.5f) * scene->cam.zdir;
}

void addObject(Scene *scene, Object *obj) {
    scene->objects.push_back(obj);
}

void addLight(Scene *scene, Light *light) {
    scene->lights.push_back(light);
}

void setSkyColor(Scene *scene, color3 c) {
    scene->skyColor = c;
}
