#pragma once
#include"CollisionTypes.h"
#include"CollisionInfo.h"
#include"Obj.h"

/// <summary>
/// �R���C�_�[���N���X
/// </summary>
class BaseCollider
{
public:
	BaseCollider() = default;
	//���z�f�X�g���N�^
	virtual ~BaseCollider() = default;

	inline void SetObject(Obj* object) { this->obj = obj; }

	inline Obj* GetObject3d() { return obj; }

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	//�`��^�C�v
	inline CollisionShapeType GetShapeType() { return shapeType; }

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	inline void OnCollision(const CollisionInfo& info) {
		obj->OnCollision(info);
	}

protected:
	Obj* obj;
	//�`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;

};