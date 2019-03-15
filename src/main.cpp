#include "defines.h"
#include "image.h"
#include "ray.h"
#include "raytracer.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600

Material mat_lib[] = {
    /* 0 nickel */
    {2.4449, 0.0681, {1.0, 0.882, 0.786}, {0.014, 0.012, 0.012}, 0.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 1 specular black phenolic */
    {1.072, 0.0588, {1.0, 0.824, 0.945}, {0.002, 0.002, 0.003}, 0.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 2 specular blue phenolic */
    {1.1051, 0.0568, {0.005, 0.013, 0.032}, {1.0, 0.748, 0.718}, 0.f,nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 3 specular green phenolic */
    {1.1051, 0.0567, {0.006, 0.026, 0.022}, {1.0, 0.739, 0.721}, 0.f,nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 4 specular white phenolic */
    {1.1022, 0.0579, {0.286, 0.235, 0.128}, {1.0, 0.766, 0.762}, 0.f,nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 5 marron plastic */
    {1.0893, 0.0604, {0.202, 0.035, 0.033}, {1.0, 0.857, 0.866}, 0.f,nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 6 purple paint */
    {1.1382, 0.0886, {0.301, 0.034, 0.039}, {1.0, 0.992, 0.98}, 0.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 7 red specular plastic */
    {1.0771, 0.0589, {0.26, 0.036, 0.014}, {1.0, 0.852, 1.172}, 0.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 8 green acrylic */
    {1.1481, 0.0625, {0.016, 0.073, 0.04}, {1.0, 1.056, 1.146}, 0.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 9 obsidian diffuse only (for tests) */
    {1.5, 0.05f, {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, 0.f
            , loadImageJPG(static_cast<char*>("resources/obsidianDiffuse.jpg"))
            , nullptr
            , nullptr
            , nullptr
            , true, false, false, false},

    /* 10 glass */
    {1.5, 0.056, {1.f, 0.9f, 0.86f}, {1.0, 1.056, 1.146}, 1.f, nullptr, nullptr, nullptr, nullptr, false, false, false, false},

    /* 11 water */
    {1.33, 0.06, {1.f, 0.9f, 0.86f}, {0.f, 0.f, 0.f}, 1.f, nullptr, loadImageJPG(static_cast<char*>("resources/waterNormal.jpg")), nullptr, nullptr, false, true, false, false},

    /* 12 obsidian */
    {1.5, 0.05f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 0.f
            , loadImageJPG(static_cast<char*>("resources/obsidianDiffuse.jpg"))
            , loadImageJPG(static_cast<char*>("resources/obsidianNormal.jpg"))
            , loadImageJPG(static_cast<char*>("resources/obsidianSpec.jpg"))
            , loadImageJPG(static_cast<char*>("resources/obsidianRough.jpg"))
            , true, true, true, true},

    /* 13 Pavement1 */
    {1.2, 0.3f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 0.f
            , loadImageJPG(static_cast<char*>("resources/pavement1Diffuse.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement1Normal.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement1Spec.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement1Rough.jpg"))
            , true, true, true, true},

    /* 14 Pavement2 */
    {1.1, 0.3f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 0.f
            , loadImageJPG(static_cast<char*>("resources/pavement2Diffuse.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement2Normal.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement2Spec.jpg"))
            , loadImageJPG(static_cast<char*>("resources/pavement2Rough.jpg"))
            , true, true, true, true},

    /* 15 earth */
    {1.25, 0.3f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 0.f
            , loadImageJPG(static_cast<char*>("resources/earthDiffuse.jpg"))
            , loadImageJPG(static_cast<char*>("resources/earthNormal.jpg"))
            , loadImageJPG(static_cast<char*>("resources/earthSpec.jpg"))
            , nullptr
            , true, true, true, false}};

Scene *initScene0() {
    Scene *scene = initScene();
    setCamera(scene, point3(3, 1, 0), vec3(0, 0.3, 0), vec3(0, 1, 0), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.1f, 0.3f, 0.5f));
    Material mat;
    mat.IOR = 1.3;
    mat.roughness = 0.1;
    mat.specularColor = color3(0.5f);
    mat.transparency = 0.f;
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    mat.diffuseColor = color3(.5f);
    addObject(scene, initSphere(point3(0, 0, 0), 0.25, mat));

    mat.diffuseColor = color3(0.5f, 0.f, 0.f);
    addObject(scene, initSphere(point3(1, 0, 0), .25, mat));
    addObject(scene, initTriangle(vec3(0,0,1), vec3(0,0,0), vec3(0,1,0), mat));

    mat.diffuseColor = color3(0.f, 0.5f, 0.5f);
    addObject(scene, initSphere(point3(0, 1, 0), .25, mat));

    mat.diffuseColor = color3(0.f, 0.f, 0.5f);
    addObject(scene, initSphere(point3(0, 0, 1), .25, mat));

    mat.diffuseColor = color3(0.6f);
    addObject(scene, initPlane(vec3(0, 1, 0), 0, mat));

    addLight(scene, initLight(point3(10, 10, 10), color3(1, 1, 1)));
    addLight(scene, initLight(point3(4, 10, -2), color3(1, 1, 1)));

    return scene;
}

Scene *initScene1() {

    Scene *scene = initScene();
    setCamera(scene, point3(3, 0, 0), vec3(0, 0.3, 0), vec3(0, 1, 0), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));

    Material mat;
    mat.IOR = 1.12;
    mat.roughness = 0.2;
    mat.specularColor = color3(0.4f);
    mat.diffuseColor = color3(0.6f);
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    for (int i = 0; i < 10; ++i) {
        mat.diffuseColor = color3(0.301, 0.034, 0.039);
        mat.specularColor = color3(1.0, 0.992, 0.98);
        mat.IOR = 1.1382;
        mat.roughness = 0.0886;
        mat.roughness = ((float)10 - i) / (10 * 9.f);
        addObject(scene, initSphere(point3(0, 0, -1.5 + i / 9.f * 3.f), .15, mat));
    }
    for (int i = 0; i < 10; ++i) {
        mat.diffuseColor = color3(0.012, 0.036, 0.106);
        mat.specularColor = color3(1.0, 0.965, 1.07);
        mat.IOR = 1.1153;
        mat.roughness = 0.068;
        mat.roughness = ((float)i + 1) / 10.f;
        addObject(scene, initSphere(point3(0, 1, -1.5 + i / 9.f * 3.f), .15, mat));
    }
    mat.diffuseColor = color3(0.014, 0.012, 0.012);
    mat.specularColor = color3(1.0, 0.882, 0.786);
    mat.IOR = 2.4449;
    mat.roughness = 0.0681;
    addObject(scene, initSphere(point3(-3.f, 1.f, 0.f), 2., mat));

    mat.diffuseColor = color3(0.016, 0.073, 0.04);
    mat.specularColor = color3(1.0, 1.056, 1.146);
    mat.IOR = 1.1481;
    mat.roughness = 0.0625;
    addObject(scene, initPlane(vec3(0, 1, 0), +1, mat));

    addLight(scene, initLight(point3(10, 10, 10), color3(10, 10, 10)));
    addLight(scene, initLight(point3(4, 10, -2), color3(5, 3, 10)));
    return scene;
}

Scene *initScene2() {
    Scene *scene = initScene();
    setCamera(scene, point3(0.5, 3, 1), vec3(0, 0, 0.6), vec3(0, 0, 1), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));
    Material mat;
    mat.diffuseColor = color3(0.014, 0.012, 0.012);
    mat.specularColor = color3(1.0, 0.882, 0.786);
    mat.IOR = 2.4449;
    mat.roughness = 0.0681;
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    mat.diffuseColor = color3(0.05, 0.05, 0.05);
    mat.specularColor = color3(0.95);
    mat.IOR = 1.1022;
    mat.roughness = 0.0579;

    addObject(scene, initPlane(vec3(0, 0, 1), 0, mat));

    mat.diffuseColor = color3(0.005, 0.013, 0.032);
    mat.specularColor = color3(1.0, 0.748, 0.718);
    for (int i = 0; i < 4; ++i) {
        mat.IOR = 1.1051 + (-0.1 + float(i) / 3.f * 0.4);
        for (int j = 0; j < 10; ++j) {
            mat.roughness = 0.0568 + (-0.1 + float(j) / 9.f * 0.3);
            addObject(scene, initSphere(point3(-1.5 + float(j) / 9.f * 3.f, 0,
                                               0.4 + float(i) * 0.4f),
                                        .15, mat));
        }
    }
    addLight(scene, initLight(point3(-20, 5, 10), color3(30, 30, 30)));
    addLight(scene, initLight(point3(10, 10, 10), color3(30, 30, 30)));
    addLight(scene, initLight(point3(50, -100, 10), color3(1, 0.7, 2)));
    return scene;
}

Scene *initScene3() {
    Scene *scene = initScene();
    setCamera(scene, point3(4.5, .8, 4.5), vec3(0, 0.3, 0), vec3(0, 1, 0), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));
    Material mat;
    mat.diffuseColor = color3(0.301, 0.034, 0.039);
    mat.specularColor = color3(1.0, 0.992, 0.98);
    mat.IOR = 1.1382;
    mat.roughness = 0.0886;
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    addLight(scene, initLight(point3(0, 1.7, 1), .5f * color3(3, 3, 3)));
    addLight(scene, initLight(point3(3, 2, 3), .5f * color3(4, 4, 4)));
    addLight(scene, initLight(point3(4, 3, -1), .5f * color3(5, 5, 5)));

    mat.diffuseColor = color3(0.014, 0.012, 0.012);
    mat.specularColor = color3(0.7, 0.882, 0.786);
    mat.IOR = 6;
    mat.roughness = 0.0181;
    addObject(scene, initSphere(point3(0, 0.1, 0), .3, mat));

    mat.diffuseColor = color3(0.26, 0.036, 0.014);
    mat.specularColor = color3(1.0, 0.852, 1.172);
    mat.IOR = 1.3771;
    mat.roughness = 0.01589;
    addObject(scene, initSphere(point3(1, -.05, 0), .15, mat));

    mat.diffuseColor = color3(0.014, 0.012, 0.012);
    mat.specularColor = color3(0.7, 0.882, 0.786);
    mat.IOR = 3;
    mat.roughness = 0.00181;
    addObject(scene, initSphere(point3(3, 0.05, 2), .25, mat));

    mat.diffuseColor = color3(0.46, 0.136, 0.114);
    mat.specularColor = color3(0.8, 0.852, 0.8172);
    mat.IOR = 1.5771;
    mat.roughness = 0.01589;
    addObject(scene, initSphere(point3(1.3, 0., 2.6), 0.215, mat));

    mat.diffuseColor = color3(0.06, 0.26, 0.22);
    mat.specularColor = color3(0.70, 0.739, 0.721);
    mat.IOR = 1.3051;
    mat.roughness = 0.567;
    addObject(scene, initSphere(point3(1.9, 0.05, 2.2), .25, mat));

    mat.diffuseColor = color3(0.012, 0.036, 0.406);
    mat.specularColor = color3(1.0, 0.965, 1.07);
    mat.IOR = 1.1153;
    mat.roughness = 0.068;
    mat.roughness = 0.18;
    addObject(scene, initSphere(point3(0, 0, 1), .20, mat));

    mat.diffuseColor = color3(.2, 0.4, .3);
    mat.specularColor = color3(.2, 0.2, .2);
    mat.IOR = 1.382;
    mat.roughness = 0.05886;
    addObject(scene, initPlane(vec3(0, 1, 0), 0.2, mat));

    mat.diffuseColor = color3(.5, 0.09, .07);
    mat.specularColor = color3(.2, .2, .1);
    mat.IOR = 1.8382;
    mat.roughness = 0.886;
    addObject(scene, initPlane(vec3(1, 0.0, -1.0), 2, mat));

    mat.diffuseColor = color3(0.1, 0.3, .05);
    mat.specularColor = color3(.5, .5, .5);
    mat.IOR = 1.9382;
    mat.roughness = 0.0886;
    addObject(scene, initPlane(vec3(0.3, -0.2, 1), 4, mat));
    return scene;
}

Scene *initScene4() {
    Scene *scene = initScene();
    setCamera(scene, point3(6, 4, 6), vec3(0, 1, 0), vec3(0, 1, 0), 90,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));
    Material mat;
    mat.diffuseColor = color3(0.301, 0.034, 0.039);
    mat.specularColor = color3(1.0, 0.992, 0.98);
    mat.IOR = 1.1382;
    mat.roughness = 0.0886;
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    addLight(scene, initLight(point3(10, 10.7, 1), .5f * color3(3, 3, 3)));
    addLight(scene, initLight(point3(8, 20, 3), .5f * color3(4, 4, 4)));
    addLight(scene, initLight(point3(4, 30, -1), .5f * color3(5, 5, 5)));

    mat.diffuseColor = color3(.2, 0.4, .3);
    mat.specularColor = color3(.2, 0.2, .2);
    mat.IOR = 2.382;
    mat.roughness = 0.005886;
    addObject(scene, initPlane(vec3(0, 1, 0), 0.2, mat));

    mat.diffuseColor = color3(.5, 0.09, .07);
    mat.specularColor = color3(.2, .2, .1);
    mat.IOR = 2.8382;
    mat.roughness = 0.00886;
    addObject(scene, initPlane(vec3(1, 0.0, 0.0), 2, mat));

    mat.diffuseColor = color3(0.1, 0.3, .05);
    mat.specularColor = color3(.5, .5, .5);
    mat.IOR = 2.9382;
    mat.roughness = 0.00886;
    addObject(scene, initPlane(vec3(0, 0, 1), 4, mat));

    for (int i = 0; i < 600; i++) {
        addObject(scene,
                  initSphere(point3(1 + rand() % 650 / 100.0, rand() % 650 / 100.0,
                                    1 + rand() % 650 / 100.0),
                             .05 + rand() % 200 / 1000.0, mat_lib[rand() % 10]));
    }
    return scene;
}

Scene *initScene1bis(float ior, float t) {

  Scene *scene = initScene();
  setCamera(scene, point3(0.5, 5, 0.5), vec3(-3, -0.3, -3), vec3(0, 1, 0), 60,
            (float)WIDTH / (float)HEIGHT);
  setSkyColor(scene, color3(0.2, 0.2, 0.7));

  Material mat;
  mat.IOR = 1.12;
  mat.roughness = 0.2;
  mat.specularColor = color3(0.4f);
  mat.diffuseColor = color3(0.6f);
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

//  for (int i = 0; i < 10; ++i) {
//    mat.diffuseColor = color3(0.301, 0.034, 0.039);
//    mat.specularColor = color3(1.0, 0.992, 0.98);
//    mat.IOR = 1.1382;
//	  mat.transparency = 0.f;
//    mat.roughness = 0.0886;
//    mat.roughness = ((float)10 - i) / (10 * 9.f);
//    addObject(scene, initSphere(point3(0, 0, -1.5 + i / 9.f * 3.f), .15, mat));
//  }
//  for (int i = 0; i < 10; ++i) {
//    mat.diffuseColor = color3(0.012, 0.036, 0.106);
//    mat.specularColor = color3(1.0, 0.965, 1.07);
//    mat.IOR = 1.1153;
//	  mat.transparency = 0.f;
//    mat.roughness = 0.068;
//    mat.roughness = ((float)i + 1) / 10.f;
//    addObject(scene, initSphere(point3(0, 1, -1.5 + i / 9.f * 3.f), .15, mat));
//  }

  mat.diffuseColor = color3(0.f, 0.5f, 0.f);
  mat.specularColor = color3(1.0, 0.882, 0.786);
  mat.IOR = ior;
  mat.roughness = 0.06f;
	mat.transparency = t;
  addObject(scene, initSphere(point3(-3.f, 1.f, -3.f), 1.4f, mat));

//	mat.diffuseColor = color3(0.014, 0.012, 0.012);
//	mat.specularColor = color3(1.0, 0.882, 0.786);
//	mat.IOR = 2.4449;
//	mat.roughness = 0.0681;
//	mat.transparency = 0.f;
//	addObject(scene, initSphere(point3(-7.f, .5f, ind), 1., mat));


    mat.hasImgTexture = true;
    mat.image_texture = loadImageJPG(static_cast<char*>("resources/chess2.jpg"));
  mat.diffuseColor = color3(0.8, 0.06, .014);
  mat.specularColor = color3(1., 1., 1.);
  mat.IOR = 1.001;
  mat.roughness = 0.0625;
  mat.transparency = 0.f;
  addObject(scene, initPlane(vec3(0, 1, 0), 1.f, mat));


  addLight(scene, initLight(point3(10, 10, 10), color3(10, 10, 10)));
  addLight(scene, initLight(point3(4, 10, -2), color3(5, 3, 10)));
  return scene;
}

Scene *initCustomScene(){
    //Bench of spheres with the mats in the matlib
    Scene *scene = initScene();
    setCamera(scene, point3(4.5, 2, 4.5), vec3(0, 0.3, 0), vec3(0, 1, 0), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));

    //objects
    addObject(scene, initPlane(vec3(0,1,0), 0.2f, mat_lib[14]));
    addObject(scene, initPlane(vec3(0,1,0), -0.8f, mat_lib[11]));
    addObject(scene, initPlane(vec3(1,0,0), 3.f, mat_lib[13]));
    addObject(scene, initPlane(vec3(0.5,0,0.5), 3.f, mat_lib[13]));

    addObject(scene, initSphere(point3(3,0.8,1), 1.f, mat_lib[10]));
    addObject(scene, initSphere(point3(3.5,0.8,2.5), .5f, mat_lib[0]));
    addObject(scene, initSphere(point3(1,1.5,-0.5), 0.25f, mat_lib[1]));
    addObject(scene, initSphere(point3(-2.5,1.8,-0.5), 1.f, mat_lib[15]));
    addObject(scene, initSphere(point3(1,0.3,3), .5f, mat_lib[13]));
    addObject(scene, initSphere(point3(0.5,1.5,0.5), .7f, mat_lib[12]));
    addObject(scene, initSphere(point3(1,1.6,-1.5), .75f, mat_lib[13]));
    addObject(scene, initSphere(point3(-1.5,1.6,1), .5f, mat_lib[14]));

    addLight(scene, initLight(point3(2.5,4,-2.5), color3(1,1,1)));
    addLight(scene, initLight(point3(-2.5,3,1.5), color3(1,1,1)));
    addLight(scene, initLight(point3(0,5,0), color3(1,1,1)));
    addLight(scene, initLight(point3(5,5,5), color3(1,1,1)));
    return scene;
}

Scene *comparingTexture(bool allTextures){
    //Bench of spheres with the mats in the matlib
    Scene *scene = initScene();
    setCamera(scene, point3(4.5, 2, 4.5), vec3(0, 0.5, 0), vec3(0, 1, 0), 40,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.2, 0.2, 0.7));

    //objects
    addObject(scene, initPlane(vec3(0,1,0), 0.f, mat_lib[14]));
    addObject(scene, initPlane(vec3(1,0,0), 3.f, mat_lib[13]));
    addObject(scene, initPlane(vec3(0.5,0,0.5), 3.f, mat_lib[13]));

    int index = allTextures ? 12 : 9;
    addObject(scene, initSphere(point3(0.25,1,0.25), 1.f, mat_lib[index]));

    addLight(scene, initLight(point3(2.5,4,-2.5), color3(1,1,1)));
    addLight(scene, initLight(point3(-2.5,3,1.5), color3(1,1,1)));
    addLight(scene, initLight(point3(0,5,0), color3(1,1,1)));
    addLight(scene, initLight(point3(5,5,5), color3(1,1,1)));
    return scene;
}

Scene *initWolfScene() {
    Scene *scene = initScene();
    setCamera(scene, point3(4, 2, 0), vec3(0, 0.4, 0), vec3(0, 1, 0), 60,
              (float)WIDTH / (float)HEIGHT);
    setSkyColor(scene, color3(0.1f, 0.3f, 0.5f));
    Material mat;
    mat.IOR = 1.3;
    mat.roughness = 0.1;
    mat.diffuseColor = color3(0.301, 0.034, 0.039);
    mat.specularColor  = color3(1.0, 0.992, 0.98);
    mat.transparency = 0.f;
    mat.hasImgTexture = false;
    mat.hasBumpTexture = false;
    mat.hasSpecTexture = false;
    mat.hasRoughTexture = false;

    initComplex(scene, "resources/wolf.obj", mat, 1.f/700.f, vec3(0.f,0.f,1.f), pi<float>()/4.f);
    mat.diffuseColor = color3(0.034f,0.301f,0.039f);
    initComplex(scene, "resources/Wolf.obj", mat, 1.f/200.f, vec3(0.f,0.f,-1.f), pi<float>()/4.f);
    mat.diffuseColor = color3(0.64f,0.640f,0.66f);
    mat.roughness = 0.06f;
    initComplex(scene, "resources/Deer.obj", mat, 1.f/250.f, vec3(-2.f,0.f,-.5f), pi<float>()/3.f);

    mat.hasImgTexture = ((mat.image_texture = loadImageJPG(static_cast<char*>("resources/chess2.jpg"))) == NULL);
    mat.diffuseColor = color3(0.6f);
    addObject(scene, initPlane(vec3(0, 1, 0), 0, mat));

    addLight(scene, initLight(point3(10, 10, 10), color3(1, 1, 1)));
    addLight(scene, initLight(point3(4, 10, -2), color3(1, 1, 1)));

    return scene;
}

void registerStudy(char *basename){
    Image *img = initImage(WIDTH, HEIGHT);
    Scene *scene = NULL;

    int count = 0;
    for (float ior = 1.0f ; ior < 2.1f ; ior += .1f) {
        for (float t = 0.f ; t < 1.1f ; t += 0.1f) {
            scene = initScene1bis(ior, t);

            printf("render scene %d.\n\tIOR : %2f\n\tTransparency : %2f\n", count, ior, t);
            renderImage(img, scene);

            std::string name(basename);
            name += std::to_string(count);
            char countedname[name.length()];
            strcpy(countedname, name.c_str());

            printf("save image to %s\n", countedname);
            saveImage(img, countedname);
            count+=1;
        }
    }
    freeImage(img);
    freeScene(scene);
}

int main(int argc, char *argv[]) {
  printf("Welcome to the L3 IGTAI RayTracer project\n");

  char basename[256];

  if (argc < 2 || argc > 3) {
    printf("usage : %s filename i\n", argv[0]);
    printf("        filename : where to save the result, whithout extention\n");
    printf("        i : scenen number, optional\n");
    exit(0);
  }

  strncpy(basename, argv[1], 255);

  int scene_id = 0;
  if (argc == 3) {
    scene_id = atoi(argv[2]);
  }

    Image *img = initImage(WIDTH, HEIGHT);
      Scene *scene = NULL;
      switch (scene_id) {
            case 0:
                scene = initScene0();
                break;
          case 1:
                scene = initScene1();
                break;
          case 2:
                scene = initScene2();
                break;
          case 3:
                scene = initScene3();
                break;
          case 4:
                scene = initScene4();
                break;
          case 5:
              registerStudy(basename);
              return 0;
          case 6:
              scene = initCustomScene();
              break;
          case 7:
              scene = comparingTexture(false);
              break;
          case 8:
              scene = comparingTexture(true);
              break;
          case 9:
              scene = initWolfScene();
              break;
          default:
              scene = initScene0();
              break;
  }

  printf("render scene %d\n", scene_id);

  renderImage(img, scene);
  freeScene(scene);
  scene = NULL;

  printf("save image to %s\n", basename);
  saveImage(img, basename);
  freeImage(img);
  img = NULL;
  printf("done. Goodbye\n");

  return 0;
}
