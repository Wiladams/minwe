#include "apphost.h"

#include "vec.h"
#include "matrix.h"

//using point3 = vec3;   // 3D point
//using color = vec3;

template<typename T>
void printVec(const vec<3,T>& v)
{
	printf("[%3.2f, %3.2f, %3.2f]\n", v[0], v[1], v[2]);
}

template<typename T>
void printVec(const vec<2, T>& v)
{
	printf("[%3.2f, %3.2f]\n", v[0], v[1]);
}

template<size_t DIM, typename T>
void printVec(const vec<DIM, T>& v)
{
	printf("----\n");
	for (size_t i = 0; i < DIM; i++)
		printf("%3.2f ", (float)v[i]);
	printf("\n----\n");
}



void printMat3(const mat3& m)
{
	printf("== mat3 ==\n");
	size_t idx = 0;
	for (int row = 0; row < 3; row++)
	{
		printf("|");
		for (int col = 0; col < 3; col++)
		{
			printf("%3.2f ", m.e[idx]);
			idx++;
		}
		printf("|\n");
	}
}



// Setup some convenient aliases
using vec2i = vec<2,int>;
using vec2u = vec<2,uint32_t>;
using vec2f = vec<2,float>;
using vec2d = vec<2,double>;

using vec3f = vec<3,float>;
using vec3d = vec<3,double>;

void onLoad()
{
	
	mat3 m1(1, 0, 0, 0, 1, 0, 0, 0, 1);
	mat3 m2(m1);
	printMat3(m2);

	printf("== vec3f ==\n");
	vec3f a({ 1, 2, 3 });
	vec3f b = -a;
	printVec(a);
	printVec(b);

	//printf("== vec<3,float>==\n");
	//vec<3, float> c;
	//c[0] = 1; c[1] = 2; c[2] = 3;
	//vec<3, float> d = -c;
	//printVec(c);
	//printVec(d);

	printf("== vec2f ====\n");
	vec2f e({ 1.0f, 2.0f });
	vec2f f = e * 3;
	vec2f g = 3.0 * e;

	printVec(e);
	printVec(f);
}