#ifndef SCENE_H
#define SCENE_H
//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "camera.h"
#include "hittable_list.h"

#include "pixelmap.h"




class scene {
    // This structure is defined in here so we 
    // don't have to declare a friend class or 
    // use any other tricks to get the self reference
    struct RayTraceCtx {
        PixelMap* fPixelMap;
        scene* fScene;
        int fLow;
        int fHigh;

        RayTraceCtx(PixelMap* pmap, scene* s, int starting, int ending)
            : fPixelMap(pmap),
            fScene(s),
            fLow(starting),
            fHigh(ending)
        {
        }
    };
    
    // This is the thread start routine
    // It will call the render() function of the 
    // scene, passing parameters
    static DWORD WINAPI renderScene(void* param)
    {
        RayTraceCtx* ctx = (RayTraceCtx*)param;
        ctx->fScene->renderSegment(ctx->fPixelMap, ctx->fLow, ctx->fHigh);

        // when done, need to delete the context
        // delete ctx

        // The return value is not used
        // but we'll return 1 anyway
        return 1;
    }

public:
    // Return the calculated height of the image
    int getHeight()
    {
          return static_cast<int>(image_width / aspect_ratio);
    }

    // This is the primary function called to render the scene
    // Just pass in a pixelmap and number of segments
    // This will use multiple threads
    void render(PixelMap* pmap, int numSegments = 1)
    {
        // Setup the individual segment threads
        int segLength = getHeight() / numSegments;

        for (int i = 0; i < numSegments; i++)
        {
            int starting = i * segLength;
            int ending = starting + segLength - 1;
            RayTraceCtx* ctx = new RayTraceCtx(&(*gAppSurface), this, ending, starting);
            Thread* t1 = new Thread(renderScene, ctx);
            t1->resume();
        }
    }

    // This is the workhorse routine of the scene
    // We render a single portion of the overall 
    // image, from a starting row to an ending row
    void renderSegment(PixelMap* pmap, int starting, int ending)
    {
        const int image_height = static_cast<int>(image_width / aspect_ratio);

        cam.initialize(aspect_ratio);

        for (int j = starting; j >= ending; --j) {

            //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / ((double)image_width-1);
                    auto v = (j + random_double()) / ((double)image_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, max_depth);
                }

                auto c = get_color(pixel_color, samples_per_pixel);
                pmap->set(i, image_height-1-j, c);
            }
        }

        std::cerr << "\nDone.\n";
    }

  public:
    hittable_list world;
    hittable_list lights;
    camera        cam;

    double aspect_ratio      = 1.0;
    int    image_width       = 100;
    int    samples_per_pixel = 10;
    int    max_depth         = 20;
    color  background        = color(0,0,0);

  private:
    color ray_color(const ray& r, int depth) {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        scatter_record srec;
        color emitted = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, srec))
            return emitted;

        if (srec.skip_pdf) {
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth-1);
        }

        auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);
        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_val = p.value(scattered.direction());

        return emitted
             + srec.attenuation * rec.mat->scattering_pdf(r, rec, scattered)
                                * ray_color(scattered, depth-1) / pdf_val;
    }
};



#endif
