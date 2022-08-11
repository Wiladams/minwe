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

class scene_earth : public scene
{
public:
    scene_earth()
    {
        init(400, 16.0 / 9.0, 100, 20, color(0.70, 0.80, 1.00));

        cam.vup = vec3(0, 1, 0);
        cam.focus_dist = 10.0;

        cam.aperture = 0.0;
        cam.vfov = 20.0;
        cam.lookfrom = point3(0, 0, 12);
        cam.lookat = point3(0, 0, 0);

        auto earth_texture = make_shared<image_texture>("earthmap.jpg");
        auto earth_surface = make_shared<lambertian>(earth_texture);
        auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

        world.add(globe);
        //world = hittable_list(globe);

        auto m = shared_ptr<material>();
        lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), m));
        lights.add(make_shared<sphere>(point3(190, 90, 190), 90, m));

    }
};