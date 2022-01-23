#pragma once

//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "maths.hpp"

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

//#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)    malloc(sz)
#define STBI_REALLOC(p,sz) realloc(p,sz)
#define STBI_FREE(p)       free(p)
//#endif
// Usings

using std::shared_ptr;
using std::make_shared;
//using std::sqrt;


// Common Headers
#include "interval.h"
#include "ray.h"

