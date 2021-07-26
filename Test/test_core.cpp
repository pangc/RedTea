#include "../Engine/Core/world.h"
#include "../Engine/Core/component_manager.h"
#include "../Engine/Core/entity.h"
#include <gtest/gtest.h>

TEST(CORE_TEST, world)
{
	using namespace redtea::core;
	World world;
	auto sa = world.CreateSection();
	auto sb = world.CreateSection();
	auto sc = world.CreateSection();
	sa->SetActive();
	sa->CreateEntity();
	sa->CreateEntity();

	auto cur_active = world.GetActiveSection();
	EXPECT_EQ(sa->GetId(), cur_active->GetId());
}

TEST(CORE_TEST, component_manager)
{
	using namespace redtea::core;
	struct Vector3
	{
		Vector3() = default;
		Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		float x;
		float y;
		float z;
	};

	class MoveManager : public ComponentManagerBase<Vector3, Vector3>
	{
	public:
		using Instance = redtea::core::ComponentInstance::Type;
		enum
		{
			Direction,
			Position
		};

		struct Proxy
		{
			Proxy(MoveManager& sim, Instance i) noexcept
				: directoin{ sim, i } { }
			union {
				Field<Direction>           directoin;
				Field<Position>             positoin;
			};
		};

		inline Proxy operator[](Instance i) noexcept {
			return { *this, i };
		}

		inline const Proxy operator[](Instance i) const noexcept {
			return { const_cast<MoveManager&>(*this), i };
		}
	};

	World world;
	MoveManager manager;
	auto section = world.CreateSection();
	auto e = section->CreateEntity();
	auto i = manager.AddComponent(e);
	manager[i].directoin = { 1, 2, 3 };
	manager[i].positoin = { 4, 5, 6 };
	EXPECT_EQ(manager.HasComponent(e), true);

	Vector3& direction = manager[i].directoin;
	EXPECT_EQ(direction.x, 1);
	EXPECT_EQ(direction.y, 2);
	EXPECT_EQ(direction.z, 3);
	
	manager.RemoveComponent(e);
	EXPECT_EQ(manager.HasComponent(e), false);
}
