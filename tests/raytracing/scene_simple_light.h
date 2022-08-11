#pragma once

#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "quad.h"
#include "scene.h"
#include "sphere.h"
#include "texture.h"


class scene_simple_light : public scene
{
public:
    scene_simple_light()
    {
        init(800, 16.0 / 9.0, 5000, 20, color(0, 0, 0));


        cam.aperture = 0.0;
        cam.vfov = 20.0;
        cam.lookfrom = point3(26, 3, 6);
        cam.lookat = point3(0, 2, 0);

        //hittable_list& world = scene_desc.world;

        auto pertext = make_shared<noise_texture>(4);
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
        world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

        auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
        world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
        world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

        auto m = shared_ptr<material>();
        lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), m));
        lights.add(make_shared<sphere>(point3(190, 90, 190), 90, m));

    }
};
