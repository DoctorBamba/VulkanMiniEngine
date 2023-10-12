#pragma once
#include "../Workspace.h"


namespace Physics
{
	enum class ColliderType
	{
		Unknow,
		Box,
		Ellipsoid
	};

	class Collider
	{
		public:
			const ColliderType collider_type;
			Vector3D extents;

			Collider(ColliderType collider_type_) : collider_type(collider_type_) {}

			virtual Vector4D GetInertiaFactors();
	};

	class BoxCollider : public Collider
	{
		public:

			BoxCollider(Vector3D extents_) : Collider(ColliderType::Box)
			{
				extents = extents_;
			}

			Vector4D GetInertiaFactors() override
			{
				return Vector4D(0.5f * (extents.y * extents.y + extents.z * extents.z),
								0.5f * (extents.x * extents.x + extents.z * extents.z),
								0.5f * (extents.x * extents.x + extents.y * extents.y), 1.0f);
			}
	};

	class EllipsoidCollider : public Collider
	{
		public:

			EllipsoidCollider(Vector3D extents_) : Collider(ColliderType::Ellipsoid)
			{
				extents = extents_;
			}

			Vector4D GetInertiaFactors() override
			{
				return Vector4D(0.2f * (extents.y * extents.y + extents.z * extents.z),
								0.2f * (extents.x * extents.x + extents.z * extents.z),
								0.2f * (extents.x * extents.x + extents.y * extents.y), 1.0f);
			}
	};
}
