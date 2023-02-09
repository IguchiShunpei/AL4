#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include <DirectXMath.h>

//球衝突判定オブジェクト
class SphereCollider : public BaseCollider,public Sphere
{
private:
	//DirectXを省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset(offset),
		radius(radius)
	{
		// 球形状をセット
		shapeType = COLLISIONSHAPE_SPHERE;
	}
	//更新
	void Update() override;

	inline void SetRadius(float radius) { this->radius = radius; }

private:
	//オブジェクト中心からのオフセット
	XMVECTOR offset;
	//半径
	float radius;
};