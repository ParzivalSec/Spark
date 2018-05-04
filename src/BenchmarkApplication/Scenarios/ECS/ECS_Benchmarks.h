#pragma once

#include <cstdint>
#include "Component.h"

class Position : public Component
{
public:
	Position(float x, float y, float z) : x(x), y(y), z(z) {}
	
	static uint32_t UID() { return 1u; }
	
	float x;
	float y;
	float z;
};

class Velocity : public Component
{
public:
	Velocity(float dx, float dy) : dx(dx), dy(dy) {}

	static uint32_t UID() { return 2u; }

	float dx;
	float dy;
};

void ecs_create_10000_with_pos();
void ecs_create_100000_with_pos();
void ecs_create_10000_pos_vel();
void ecs_create_100000_pos_vel();
void ecs_iterate_10000_pos();
void ecs_iterate_100000_pos();
void ecs_remove_5000_pos();
void ecs_remove_50000_pos();
