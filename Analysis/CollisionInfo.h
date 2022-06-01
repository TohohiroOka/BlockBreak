#pragma once
#include<DirectXMath.h>

class Obj;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
class CollisionInfo
{
public:
	CollisionInfo(Obj* obj, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		this->obj = obj;
		this->collider = collider;
		this->inter = inter;
	};

	//衝突相手のオブジェクト
	Obj* obj;
	//衝突相手のコライダー
	BaseCollider* collider;
	//衝突点
	DirectX::XMVECTOR inter;
};

