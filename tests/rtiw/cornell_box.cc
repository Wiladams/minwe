//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "gui.h"
#include "draw2d.h"
#include "Thread.h"
#include "checkerboard.h"
#include "recorder.h"

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "scene.h"
#include "sphere.h"

/*
    Cornell Box scene rendered using winme
*/
void cornell_box(scene * scene_desc, int width) 
{
    scene_desc->image_width = width;
    scene_desc->aspect_ratio      = 1.0;
    scene_desc->samples_per_pixel = 150;    // 100
    scene_desc->max_depth         = 50;   // 50
    scene_desc->background        = color(0,0,0);

    scene_desc->cam.lookfrom     = point3(278, 278, -800);
    scene_desc->cam.lookat       = point3(278, 278, 0);
    scene_desc->cam.vup          = vec3(0, 1, 0);
    scene_desc->cam.vfov         = 40.0;
    scene_desc->cam.aperture     = 0.0;
    scene_desc->cam.focus_dist   = 10.0;

    hittable_list& world = scene_desc->world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    // Cornell box sides
    world.add(make_shared<quad>(point3(555,0,0), vec3(0,0,555), vec3(0,555,0), green));
    world.add(make_shared<quad>(point3(0,0,555), vec3(0,0,-555), vec3(0,555,0), red));
    world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(555,0,555), vec3(-555,0,0), vec3(0,555,0), white));

    // Light
    world.add(make_shared<quad>(point3(213,554,227), vec3(130,0,0), vec3(0,0,105), light));

    // Box
    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    // Glass Sphere
    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(190,90,190), 90, glass));

    // Light Sources
    hittable_list& lights = scene_desc->lights;
    auto m = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(343,554,332), vec3(-130,0,0), vec3(0,0,-105), m));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, m));
}

// Since the rendering will be multi-threaded
// we need to ensure the lifetime of the scene
// is long enough, so we allocate the scene instead
// of allowing it to be created on the stack in setup()
scene* scene_desc = nullptr;
Recorder* reco = nullptr;

void onFrame()
{
    reco->saveFrame();
}

// create the canvas size
void setup() 
{
    int sceneWidth = 800;

    // Create a scene
    scene_desc = new scene();
    cornell_box(scene_desc, sceneWidth);

    // Use the size of the scene to construct the canvas
    setCanvasSize(sceneWidth, scene_desc->getHeight());

    // set a frame rate so we can see progress
    // it does not have to be a high frame rate, because
    // the raytrace is fairly slow
    setFrameRate(5);

    // Draw a background to start things
    Checkerboard chkbg({ 0,0,canvasWidth,canvasHeight },
        0xff0c0c0c,
        0xff1c1c1c,32);

    chkbg.draw(gAppSurface);

    // Setup Recorder
    reco = new Recorder(&(*gAppSurface), "frame", 0);
    reco->record();

    // begin scene rendering, specifying number of threads
    // to be used
    scene_desc->render(&*gAppSurface, 16);
}
