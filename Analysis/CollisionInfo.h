#pragma once
#include<DirectXMath.h>

class Obj;
class BaseCollider;

/// <summary>
/// �Փˏ��
/// </summary>
class CollisionInfo
{
public:
	CollisionInfo(Obj* obj, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		this->obj = obj;
		this->collider = collider;
		this->inter = inter;
	};

	//�Փˑ���̃I�u�W�F�N�g
	Obj* obj;
	//�Փˑ���̃R���C�_�[
	BaseCollider* collider;
	//�Փ˓_
	DirectX::XMVECTOR inter;
};

