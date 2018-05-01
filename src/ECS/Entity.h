#pragma once

#include <cstdint>

struct Entity
{
	Entity() = default;
	Entity(uint32_t id, uint32_t idx)
		: id(id)
		, idx(idx)
	{}

	uint32_t id;
	uint32_t idx;
};

struct Index
{
	Index() = default;
	Index(uint32_t id, uint32_t idx)
		: id(id)
		, idx(idx)
	{}

	uint32_t id;
	uint32_t idx;
};