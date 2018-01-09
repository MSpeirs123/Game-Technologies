#pragma once
#include <thrust\device_ptr.h>
#include <thrust\binary_search.h>
#include <thrust\iterator\counting_iterator.h>
#include <thrust\sort.h>
#include "CudaCommon.cuh"
#include <nclgl\common.h>
#include "../ncltech/Constraint.h"

//The header files here will be including in c++ code,
// so can't have any cuda kernels within it.

//Size of our particles in world space
#define PARTICLE_RADIUS 0.1f

//defines a 3D grid of cells 128x128x128
#define PARTICLE_GRID_SIZE 128

//defines a world transform from grid to world, e.g. particles can move from 0 to (PARTICLE_GRID_SIZE * PARTICLE_GRID_CELL_SIZE)
// in each axis.
//!! This MUST be equal or larger than particle_radius for the broadphase to work !!
#define PARTICLE_GRID_CELL_SIZE 0.15f

#define COLLISION_ELASTICITY 0.5f

//#pragma pack(push, 16)
struct Particle
{
	float3 _vel;
	float padding1;
	float3 _pos;
	float padding2;
};
//#pragma pack(pop)

class CudaSpringConstraint : public Constraint
{
	CudaSpringConstraint();
	~CudaSpringConstraint();


};