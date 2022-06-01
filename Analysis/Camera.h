#pragma once
#include "WindowApp.h"
#include <DirectXMath.h>

class Camera final
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static void init(WindowApp* winApp);

	//更新
	static void upDate(XMFLOAT3 move, float speed);

	//追従するオブジェクトの位置を記録
	void SetPositon(XMFLOAT3 pos);

	static XMMATRIX View() { return view; }

	static XMMATRIX Projection() { return matProjection; }

private:
	static XMFLOAT3 positon;
	static XMFLOAT3 f0;
	static XMVECTOR v0;
	static XMFLOAT3 up;
	static XMMATRIX view;
	static XMMATRIX matProjection;//射影行列
};

