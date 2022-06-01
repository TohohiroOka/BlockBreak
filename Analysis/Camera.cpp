#include "Camera.h"

using namespace DirectX;

XMFLOAT3 Camera::positon;
XMFLOAT3 Camera::f0 = { 0, 0, -200 };
XMVECTOR Camera::v0 = { f0.x, f0.y, f0.z, 0 };
XMFLOAT3 Camera::up = { 0, 1, 0 };
XMMATRIX Camera::view = DirectX::XMMatrixIdentity();
XMMATRIX Camera::matProjection;//射影行列

void Camera::init(WindowApp* winApp)
{
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)winApp->windowWidth / winApp->windowHeight,
		0.1f, 3000.0f//奥行/手前,最奥
	);
}

//更新
void Camera::upDate(XMFLOAT3 move, float speed)
{
	const float radian = XM_PI / 180.0f;

	XMMATRIX rotM = XMMatrixIdentity();
	XMVECTOR v = XMVector3TransformNormal(v0, rotM);

	XMFLOAT3 targetF = { positon.x - static_cast<float>(cos(move.x * radian) * speed),
		positon.y - static_cast<float>(sin(move.y * radian) * speed),
		positon.z - static_cast<float>(sin(move.x * radian) * speed) };
	XMVECTOR targetV = { targetF.x, targetF.y, targetF.z, 0 };

	XMVECTOR eyeV = targetV + v;

	XMFLOAT3 eyeF = { eyeV.m128_f32[0], eyeV.m128_f32[1], eyeV.m128_f32[2] };

	eyeF.x = positon.x;
	eyeF.y = positon.y;
	eyeF.z = positon.z;

	view = XMMatrixLookAtLH(XMLoadFloat3(&eyeF), XMLoadFloat3(&targetF), XMLoadFloat3(&up));
}

//追従するオブジェクトの位置を記録
void Camera::SetPositon(XMFLOAT3 pos)
{
	positon = pos;
}