#pragma once

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "scene.h"
#include "sphere.h"


class scene_cornell_smoke : public scene
{
public:
    scene_cornell_smoke()
    {
        init(300, 1.0, 4096, 4, color(0, 0, 0));



        cam.aperture = 0.0;
        cam.vfov = 40.0;
        cam.lookfrom = point3(278, 278, -800);
        cam.lookat = point3(278, 278, 0);

        auto red = make_shared<lambertian>(color(.65, .05, .05));
        auto white = make_shared<lambertian>(color(.73, .73, .73));
        auto green = make_shared<lambertian>(color(.12, .45, .15));
        auto light = make_shared<diffuse_light>(color(7, 7, 7));

        world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
        world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
        world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

        // Light
        lights.add(make_shared<quad>(point3(213, 554, 227), vec3(130, 0, 0), vec3(0, 0, 105), light));

        shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));

        shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
        box2 = make_shared<rotate_y>(box2, -18);
        box2 = make_shared<translate>(box2, vec3(130, 0, 65));

        world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
        world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    }
};
