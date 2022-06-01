#pragma once
#include"CollisionTypes.h"
#include"CollisionInfo.h"
#include"Obj.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider
{
public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

	inline void SetObject(Obj* object) { this->obj = obj; }

	inline Obj* GetObject3d() { return obj; }

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	//形状タイプ
	inline CollisionShapeType GetShapeType() { return shapeType; }

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	inline void OnCollision(const CollisionInfo& info) {
		obj->OnCollision(info);
	}

protected:
	Obj* obj;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;

};