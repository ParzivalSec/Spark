#include "gtest/gtest.h"

#include "World.h"
#include "Component.h"

class Position : public Component
{
public:
	Position(uint32_t x, uint32_t y, uint32_t z)
		: x(x)
		, y(y)
		, z(z)
	{}

	static uint32_t UID() { return 1u; }

	uint32_t x;
	uint32_t y;
	uint32_t z;
};

class Velocity : public Component
{
public:
	Velocity(uint32_t x, uint32_t y, uint32_t z)
		: x(x)
		, y(y)
		, z(z)
	{}

	static uint32_t UID() { return 2u; }

	uint32_t x;
	uint32_t y;
	uint32_t z;
};

TEST(World_Tests, Entity_Creation)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	const Entity two = ecs_world.make();
	ASSERT_NE(one.id, two.id) << "World did not increase entity id";
}

TEST(World_Tests, Entity_Alive_Check)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ASSERT_TRUE(ecs_world.contains(one)) << "Entity one is not part if the ECS world";
}

TEST(World_Tests, Single_Component_Insertion)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(0, 0, 0));
	ASSERT_TRUE(ecs_world.hasComponent<Position>(one)) << "Entity did not own a Position component";
}

TEST(World_Tests, Multiple_Component_Insertion)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(0, 0, 0));
	ecs_world.addComponent(one, Velocity(0, 0, 0));
	ASSERT_TRUE(ecs_world.hasComponent<Position>(one)) << "Entity did not own a Position component";
	ASSERT_TRUE(ecs_world.hasComponent<Velocity>(one)) << "Entity did not own a Velocity component";
}

TEST(World_Tests, Get_Entities_With_Component)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(0, 0, 0));
	ecs_world.addComponent(one, Velocity(0, 0, 0));
	const Entity two = ecs_world.make();
	ecs_world.addComponent(two, Position(0, 0, 0));
	ecs_world.addComponent(two, Velocity(0, 0, 0));
	const Entity three = ecs_world.make();
	ecs_world.addComponent(three, Position(0, 0, 0));

	const std::vector<Entity>* velocityEntities = ecs_world.entitiesWithComponent<Velocity>();
	ASSERT_EQ(velocityEntities->size(), 2u) << "Not all entities with a Velocity Component were returned";
}

TEST(World_Tests, Remove_Component)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(0, 0, 0));
	ecs_world.addComponent(one, Velocity(0, 0, 0));
	ASSERT_TRUE(ecs_world.hasComponent<Position>(one)) << "Entity did not own a Position component";
	ASSERT_TRUE(ecs_world.hasComponent<Velocity>(one)) << "Entity did not own a Velocity component";
	ecs_world.removeComponent<Velocity>(one);
	ASSERT_TRUE(ecs_world.hasComponent<Position>(one)) << "Entity did not own a Position component";
	ASSERT_FALSE(ecs_world.hasComponent<Velocity>(one)) << "Velocity component was not deleted from Entity";
}

TEST(World_Tests, Remove_Entity)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(0, 0, 0));
	ecs_world.addComponent(one, Velocity(0, 0, 0));

	ecs_world.removeEntity(one);
	ASSERT_FALSE(ecs_world.contains(one)) << "Entity was not removed from world";
}

TEST(World_Tests, Get_Component)
{
	World ecs_world;
	const Entity one = ecs_world.make();
	ecs_world.addComponent(one, Position(12, 12, 12));
	ecs_world.addComponent(one, Velocity(1, 2, 3));

	Position* pos = ecs_world.getComponent<Position>(one);
	ASSERT_EQ(pos->x, 12) << "Pos->x was corrupted";
	ASSERT_EQ(pos->y, 12) << "Pos->y was corrupted";
	ASSERT_EQ(pos->z, 12) << "Pos->z was corrupted";
}

TEST(World_Tests, Iterate_Components)
{
	const uint32_t POS_VEL_SIZE = 1000u;
	const uint32_t POS_SIZE = 9000u;

	World ecs_world;

	// Create pos-vel entities
	for (size_t idx = 0u; idx < POS_VEL_SIZE; ++idx)
	{
		const Entity e = ecs_world.make();
		ecs_world.addComponent(e, Position(1, 1, 1));
		ecs_world.addComponent(e, Velocity(1, 1, 1));
	}

	// Create only pos entities
	for (size_t idx = 0u; idx < POS_SIZE; ++idx)
	{
		const Entity e = ecs_world.make();
		ecs_world.addComponent(e, Position(1, 1, 1));
	}

	// Update all velocities
	const std::vector<Entity>* withVelocity = ecs_world.entitiesWithComponent<Velocity>();
	ASSERT_EQ(withVelocity->size(), POS_VEL_SIZE) << "Too few entities with velocity components";
	for (const Entity& entity : *withVelocity)
	{
		if (ecs_world.hasComponent<Position>(entity))
		{
			Velocity* vel = ecs_world.getComponent<Velocity>(entity);
			Position* pos = ecs_world.getComponent<Position>(entity);

			pos->x += vel->x;
			pos->y += vel->y;
			pos->z += vel->z;
		}
	}

	// Render with positions
	const std::vector<Entity>* withPositions = ecs_world.entitiesWithComponent<Position>();
	ASSERT_EQ(withPositions->size(), POS_SIZE + POS_VEL_SIZE) << "Too few entities with velocity components";
}