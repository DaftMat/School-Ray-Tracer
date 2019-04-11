# School-Ray-Tracer
A more advanced Ray Tracer with shaders, bsdf, anti-aliasing and much more !

## Introduction
  This Ray Tracer is the school project for image traitment program of 3rd bachelor year in computer science, there are some scenes you can try ! Or you can add yours setting the right lines in the main.cpp (do not break it !)
  
## What does this Ray Tracer include ?
  * Basic Ray-Casting from foved camera to a 3d scene represented with points, distances and vectors.
  * Basic shading and shades : realistic effects on the color of the objects in the scene.
  * BSDF : realistic effect of light scattered by the surfaces.
  * Reflexions and refractions
  * Anti-aliasing : using basic supersampling
  * Mapping : possibility to add a texture to any of these parameter (in the material of the object)
    * Diffuse color
    * Specular color
    * Roughness (for the BSDF)
    * Intersection normals
  
## Download
  To download this project you can clone it with the submodules so you can build it and edit it as you want using your favorite environment.
  - `git clone --recurse-submodules <gitlink>`
    - hint : you can get the git link clicking on the `clone` button on the top of the repository.
  
## Build
  - Go to src, make sure all the dependencies are in this `src` directory. Open a terminal and run the following commands
    - `mkdir build`
    - `cd mkdir`
    - `cmake ..`
    - `make`
  - Then you'll have two executable files in the `build` directory : `mrt` and `unit-test` (<= you don't need this one)

## Execution
  Command : `./mrt [out_file_name] [id_scene]`
  Width `id_scene` from 0 to 8.
  
## Sample result (scene 3)

![Here is my favorite scene gaven by the teachers](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/intermediary/f/3bad3b3a-3db4-4a00-b8d9-0c35d1c1de9c/dczek80-e615718b-f276-405b-b328-fb09ce0c362d.png)

## Refraction result (using scene 1 (not available in the project anymore))

![Here is the refraction result](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/intermediary/f/3bad3b3a-3db4-4a00-b8d9-0c35d1c1de9c/dd135q3-c4f2e02b-f0eb-4b67-ac37-e9a71c1f5a9e.png)

## Textures result (using a custom scene, 6 in the project)

![Here is an obsidian full set of textures applied on a basic sphere](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/f/3bad3b3a-3db4-4a00-b8d9-0c35d1c1de9c/dd1xmyv-43eaa333-459d-47bd-9428-21b01144b80d.png?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1cm46YXBwOjdlMGQxODg5ODIyNjQzNzNhNWYwZDQxNWVhMGQyNmUwIiwiaXNzIjoidXJuOmFwcDo3ZTBkMTg4OTgyMjY0MzczYTVmMGQ0MTVlYTBkMjZlMCIsIm9iaiI6W1t7InBhdGgiOiJcL2ZcLzNiYWQzYjNhLTNkYjQtNGEwMC1iOGQ5LTBjMzVkMWMxZGU5Y1wvZGQxeG15di00M2VhYTMzMy00NTlkLTQ3YmQtOTQyOC0yMWIwMTE0NGI4MGQucG5nIn1dXSwiYXVkIjpbInVybjpzZXJ2aWNlOmZpbGUuZG93bmxvYWQiXX0.y5z-b8zok_uDd3lcDKtfI7ABCiWT55LwLeCzHpoe45E)

## Custom scene (for fun, scene 6)

![All the things I have done in one picture](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/f/3bad3b3a-3db4-4a00-b8d9-0c35d1c1de9c/dd1xmy4-9efeeb94-feef-49d7-a15b-961b9ba961f9.png?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1cm46YXBwOjdlMGQxODg5ODIyNjQzNzNhNWYwZDQxNWVhMGQyNmUwIiwiaXNzIjoidXJuOmFwcDo3ZTBkMTg4OTgyMjY0MzczYTVmMGQ0MTVlYTBkMjZlMCIsIm9iaiI6W1t7InBhdGgiOiJcL2ZcLzNiYWQzYjNhLTNkYjQtNGEwMC1iOGQ5LTBjMzVkMWMxZGU5Y1wvZGQxeG15NC05ZWZlZWI5NC1mZWVmLTQ5ZDctYTE1Yi05NjFiOWJhOTYxZjkucG5nIn1dXSwiYXVkIjpbInVybjpzZXJ2aWNlOmZpbGUuZG93bmxvYWQiXX0.4qQZL4LCLK-fbNj9rRQxcgihu13IvjJALjItzocntIg)

## Complex geometry scene (scene 9)
![2 wolves and a deer in low poly](https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/f/3bad3b3a-3db4-4a00-b8d9-0c35d1c1de9c/dd26s1k-5cf86086-bcfa-45f1-83c3-5af52d39cf09.png?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1cm46YXBwOjdlMGQxODg5ODIyNjQzNzNhNWYwZDQxNWVhMGQyNmUwIiwiaXNzIjoidXJuOmFwcDo3ZTBkMTg4OTgyMjY0MzczYTVmMGQ0MTVlYTBkMjZlMCIsIm9iaiI6W1t7InBhdGgiOiJcL2ZcLzNiYWQzYjNhLTNkYjQtNGEwMC1iOGQ5LTBjMzVkMWMxZGU5Y1wvZGQyNnMxay01Y2Y4NjA4Ni1iY2ZhLTQ1ZjEtODNjMy01YWY1MmQzOWNmMDkucG5nIn1dXSwiYXVkIjpbInVybjpzZXJ2aWNlOmZpbGUuZG93bmxvYWQiXX0.tjfDiAwe-FwLe1mJqeGWu_ohxdiRF8AzmiQKS3nMqLA)
