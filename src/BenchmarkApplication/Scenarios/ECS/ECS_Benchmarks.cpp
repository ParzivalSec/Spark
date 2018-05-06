#include "ECS_Benchmarks.h"

#include "Entity.h"
#include "World.h"

static const size_t NUM_ENTITIES = 10000;
static const size_t LARGE_NUM_ENTITIES = 100000;

void ecs_create_10000_with_pos()
{
	Entity entities[NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}
}

void ecs_create_100000_with_pos()
{
	Entity entities[LARGE_NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}
}

void ecs_create_10000_pos_vel()
{
	Entity entities[NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		world.addComponent(entity, Velocity(10, 10));
		entities[idx] = entity;
	}
}

void ecs_create_100000_pos_vel()
{
	Entity entities[LARGE_NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		world.addComponent(entity, Velocity(10, 10));
		entities[idx] = entity;
	}
}

void ecs_iterate_10000_pos()
{
	Entity entities[NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}

	for (size_t idx = 0; idx < NUM_ENTITIES; ++idx)
	{
		Position* pos = world.getComponent<Position>(entities[idx]);
		pos->x += 100;
	}
}

void ecs_iterate_100000_pos()
{
	Entity entities[LARGE_NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES; ++idx)
	{
		Position* pos = world.getComponent<Position>(entities[idx]);
		pos->x += 100;
	}
}

void ecs_remove_5000_pos()
{
	Entity entities[NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}

	for (size_t idx = 0; idx < NUM_ENTITIES / 500; ++idx)
	{
		world.removeComponent<Position>(entities[idx]);
	}
}

void ecs_remove_50000_pos()
{
	Entity entities[LARGE_NUM_ENTITIES];
	World world;

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES; ++idx)
	{
		const Entity entity = world.make();
		world.addComponent(entity, Position(1, 2, 3));
		entities[idx] = entity;
	}

	for (size_t idx = 0; idx < LARGE_NUM_ENTITIES / 500; ++idx)
	{
		world.removeComponent<Position>(entities[idx]);
	}
}
