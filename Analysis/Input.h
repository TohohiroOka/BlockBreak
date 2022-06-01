#pragma once
#include"WindowApp.h"
#include <DirectXMath.h>
#include<wrl.h>
#include<dinput.h>
#define DIRECTINPUT_VERSION 0x0800

class Input final
{
public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private://シングルトンパターン
	Input() = default;
	~Input() = default;

public://シングルトンパターン
	Input(const Input& input) = delete;
	Input& operator=(const Input& input) = delete;

	static Input* GetInstance();


public://関数宣言

	///summary
	///キーの押下をチェック
	///</summary>
	///<param nam="KeyNumber">キー番号(DIK_0等)</param>
	///<return>	押されているか</retrun>
	bool PushKey(BYTE keyNumber);

	///summary
	///キーのトリガーをチェック
	///</summary>
	///<param nam="KeyNumber">キー番号(DIK_0等)</param>
	///<return>	トリガーか</retrun>
	bool TriggerKey(BYTE keyNumber);

	///<summary>
	///キーの左ボタン押下をチェック
	///</summary>
	///<returns>押されているか</returns>
	bool PushMouseLeft();

	///<summary>
	///キーの中ボタン押下をチェック
	///</summary>
	///<returns>押されているか</returns>
	bool PushMouseMiddle();

	///<summary>
	///キーの左ボタントリガーをチェック
	///</summary>
	///<returns>トリガーか</returns>
	bool TriggerMouseLeft();

	///<summary>
	///キーの中ボタントリガーをチェック
	///</summary>
	///<returns>トリガーか</returns>
	bool TriggerMouseMiddle();

	///<summary>
	///左スティックの傾け方向をチェック
	///</summary>
	///point +1右/-1左
	///<returns>押し込みはどちらか</returns>
	char LEFT_PadStickX();

	///<summary>
	///左スティックの傾け方向をチェック
	///</summary>
	///point +1下/-1上
	///<returns>押し込みはどちらか</returns>
	char LEFT_PadStickY();

	///<summary>
	///右スティックの傾け方向をチェック
	///</summary>
	///point +1右/-1左
	///<returns>押し込みはどちらか</returns>
	char RIGHT_PadStickX();

	///<summary>
	///右スティックの傾け方向をチェック
	///</summary>
	///point +1下/-1上
	///<returns>押し込みはどちらか</returns>
	char RIGHT_PadStickY();

	///<summary>
	///ゲームパッドのボタンチェック
	///</summary>
	///<returns>押されているか</returns>
	bool PushPadButtons(BYTE keyNumber);

	///<summary>
	///ゲームパッドのボタンチェック
	///</summary>
	///<returns>トリガーか</returns>
	bool TriggerPadButtons(BYTE keyNumber);

	void init(WindowApp* winApp);
	void upDate();

public://ゲームパッドキーコンフィグ
	enum PudButton {
		PAD_A = 0,//0
		PAD_B,//1
		PAD_X,//2
		PAD_Y,//3
		PAD_LB,//4
		PAD_RB,//5
		PAD_BUCK,//6
		PAD_START,//7
		PAD_PUSH_L_STICK,//8
		PAD_PUSH_R_STICK,//9
		PAD_UP,//10
		PAD_LEFT,//11
		PAD_DOWN,//12
		PAD_RIGHT,//13
		//LT,RTがわからない
	};

private://変数
	//キー
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8>devkeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};//前回キーの状態

	//マウス
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};

	//マウス
	ComPtr<IDirectInputDevice8> devStick;
	LPVOID parameter;
	DIJOYSTATE  padData;
	DIJOYSTATE  padDataPre;
	bool isPad = false;

};

