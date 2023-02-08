#pragma once

#include "CollisionPrimitive.h"

class Collision
{
public:
	//‹…‚Æ•½–Ê‚Ì“–‚½‚è”»’è
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter = nullptr);
};

