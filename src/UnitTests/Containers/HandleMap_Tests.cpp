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

