#include "gtest/gtest.h"

#include "HandleMap/HandleMap.h"

struct TextureResource
{
public:
	uint32_t width;
	uint32_t height;
	uint8_t  data[100];
};

TEST(HandleMap, Construction)
{
	const sp::container::HandleMap<TextureResource> handleMap(100);
	ASSERT_EQ(handleMap.Capacity(), 100) << "HandleMap has wrong initial capacity";
	ASSERT_EQ(handleMap.Size(), 0) << "HandleMap has wrong initial size";
}

TEST(HandleMap, Insert)
{
	sp::container::HandleMap<TextureResource> handleMap(100);
	TextureResource diffuse;

	handleMap.Insert(diffuse);

	ASSERT_EQ(handleMap.Capacity(), 100) << "HandleMap has wrong initial capacity";
	ASSERT_EQ(handleMap.Size(), 1) << "HandleMap has wrong size after insertion";
}

TEST(HandleMap, Lookup_At)
{
	sp::container::HandleMap<TextureResource> handleMap(100);
	const TextureResource diffuse { 100, 200, { 3 }};

	sp::container::Handle textureHandle = handleMap.Insert(diffuse);
	
	TextureResource& diffRef = handleMap.At(textureHandle);
	ASSERT_EQ(diffRef.width, 100) << "Item referenced by Handle had the wrong width value";
	ASSERT_EQ(diffRef.height, 200) << "Item referenced by Handle had the wrong height value";
}

TEST(HandleMap, Lookup_Index_Operator)
{
	sp::container::HandleMap<TextureResource> handleMap(100);
	const TextureResource diffuse{ 100, 200,{ 3 } };

	sp::container::Handle textureHandle = handleMap.Insert(diffuse);

	TextureResource& diffRef = handleMap[textureHandle];
	ASSERT_EQ(diffRef.width, 100) << "Item referenced by Handle had the wrong width value";
	ASSERT_EQ(diffRef.height, 200) << "Item referenced by Handle had the wrong height value";
}

TEST(HandleMap, Erase)
{
	sp::container::HandleMap<TextureResource> handleMap(100);
	const TextureResource diffuse{ 100, 200,{ 3 } };

	sp::container::Handle textureHandle = handleMap.Insert(diffuse);

	handleMap.Erase(textureHandle);
	ASSERT_FALSE(handleMap.IsValid(textureHandle)) << "TextureHandle should be invalid after Erase()";
}

TEST(HandleMap, Assert_On_Cap_Reached)
{
	sp::container::HandleMap<TextureResource> handleMap(10);
	const TextureResource diffuse{ 100, 200,{ 3 } };

	std::vector<sp::container::Handle> textureHandles(10);
	for (size_t i = 0u; i < 10; ++i)
	{
		textureHandles.push_back(handleMap.Insert(diffuse));
	}

	ASSERT_DEATH(handleMap.Insert(diffuse), "Item count reached maximum, cannot insert anymore. Maybe alter the max item cap?");
}

TEST(HandleMap, Clear)
{
	sp::container::HandleMap<TextureResource> handleMap(10);
	const TextureResource diffuse{ 100, 200,{ 3 } };

	std::vector<sp::container::Handle> textureHandles(10);
	for (size_t i = 0u; i < 10; ++i)
	{
		textureHandles.push_back(handleMap.Insert(diffuse));
	}

	handleMap.Clear();

	for (size_t i = 0u; i < 10; ++i)
	{
		ASSERT_FALSE(handleMap.IsValid(textureHandles[i])) << "Every handle should be invalid after Clear() was called";
	}
}

TEST(HandleMap, Item_Iteration)
{
	sp::container::HandleMap<TextureResource> handleMap(10);
	const TextureResource diffuse{ 100, 200,{ 3 } };

	std::vector<sp::container::Handle> textureHandles(10);
	for (size_t i = 0u; i < 10; ++i)
	{
		textureHandles.push_back(handleMap.Insert(diffuse));
	}

	TextureResource* texture = handleMap.GetItems();
	for (size_t idx = 0u; idx < handleMap.Size(); ++idx)
	{
		ASSERT_EQ(texture[idx].width, 100) << "Texture width was corrupted";
		ASSERT_EQ(texture[idx].height, 200) << "Texture height was corrupted";
	}
}