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



#include "scene_cornell_box.h"
#include "scene_cornell_smoke.h"
#include "scene_final.h"
#include "scene_earth.h"
#include "scene_simple_light.h"


#include "gui.h"

//cornell_box mainScene;
scene_cornell_smoke mainScene;
//scene_final mainScene;
//scene_earth mainScene;
//scene_simple_light mainScene;

void onFrame()
{
    if (!mainScene.renderContinue())
    {
        recordingStop();
        //halt();
    }
}

void setup() 
{
    setCanvasSize(mainScene.image_width, mainScene.image_height);

    mainScene.renderBegin();
    recordingStart();
}
