#pragma once
#include "WindowApp.h"
#include "DirectXCommon.h"
#include"Input.h"
#include"Audio.h"
#include"Object3d.h"
#include"Sprite.h"
#include"DebugText.h"
#include"Obj.h"
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")
#include<sstream>
#include <iomanip>

using namespace DirectX;
using namespace Microsoft::WRL;

class GameSystem
{
public://クラス内部で使用する
	//DirectXとキーの初期化
	void systemInit(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName);
	//深度生成
	void afterInit(WindowApp* winApp);

public://メインに書く
	//初期化処理
	void init(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName);
	//更新処理
	void upDate(WindowApp* winApp);
	//デバッグ用数字
	void debugNum(float x, float y, float z);
	//描画処理
	void draw();
	//フレームレート固定
	void frameRateKeep();
	//エスケープが入力されたら終了する処理
	bool gameFin(WindowApp* winApp);
	//解放
	void systemDelete();

	//dxCommonを渡す
	DirectXCommon* getdXCommon() { return dXCommon; }

private://いじらないクラスの宣言
	//directX初期化
	DirectXCommon* dXCommon = nullptr;

	//数字表示デバッグ用
	wchar_t str[256] = {};

	//フレームレート固定用
	const float MIN_FREAM_TIME = 1.0f / 60;
	float frameTime = 0;
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timeFreq;
	float fps = 0;
};