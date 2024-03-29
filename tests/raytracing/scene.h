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

#include "apphost.h"
#include "rtweekend.h"

#include "camera.h"
#include "hittable_list.h"


class scene {
public:
    hittable_list world;
    hittable_list lights;
    camera        cam;

    double aspect_ratio = 1.0;
    int    image_width = 100;
    int    image_height = 100;
    int    samples_per_pixel = 10;
    int    max_depth = 20;
    color  background = color(0, 0, 0);

    int fCurrentRow = 0;

  public:
      void init(int iwidth, double aspect, int spp, int maxd, const color& bkgd)
      {
          image_width = iwidth;
          aspect_ratio = aspect;
          image_height = static_cast<int>(image_width / aspect_ratio);
          samples_per_pixel = spp;
          max_depth = maxd;
          background = bkgd;
      }

      bool renderContinue()
      {
          if (fCurrentRow >= image_height)
              return false;


          int j = image_height - 1 - fCurrentRow;

          color out_color;

          for (int i = 0; i < image_width; ++i) {
              color pixel_color(0, 0, 0);
              for (int s = 0; s < samples_per_pixel; ++s) {
                  auto u = (i + random_double()) / (image_width - 1);
                  auto v = (j + random_double()) / (image_height - 1);
                  ray r = cam.get_ray(u, v);
                  pixel_color += ray_color(r, max_depth);
              }

              fit_color(out_color, pixel_color, samples_per_pixel);
              //write_color(std::cout, pixel_color, samples_per_pixel);
              gAppSurface->copyPixel(i, fCurrentRow, PixelRGBA(out_color[0], out_color[1], out_color[2]));
          }

          fCurrentRow = fCurrentRow + 1;

          return true;
      }

      void renderBegin()
      {
        cam.initialize(aspect_ratio);

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    }



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
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, srec))
            return color_from_emission;

        if (srec.skip_pdf) {
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth-1);
        }

        auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);

        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_val = p.value(scattered.direction());

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

        color color_from_scatter =
            (srec.attenuation * scattering_pdf * ray_color(scattered, depth-1)) / pdf_val;

        return color_from_emission + color_from_scatter;
    }
};


#endif
