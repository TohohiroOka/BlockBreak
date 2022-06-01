#include "Input.h"
#include <cassert>

//ライブラリのリンク
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//シングルトンパターン
Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

void Input::init(WindowApp* winApp) {
	HRESULT result;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(winApp->getWinInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&dinput, nullptr);
	
	//デバイスの列挙
	result = dinput->EnumDevices(DI8DEVTYPE_JOYSTICK, DeviceFindCallBack,
		&parameter, DIEDFL_ATTACHEDONLY);

	//キーボードデバイス生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	//ジョイステックの生成	
	result = dinput->CreateDevice(GUID_Joystick, &devStick, NULL);

	if (!FAILED(result))
	{
		isPad = true;
	}

	//入力データ形式のセット(キー)
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	//排他制御レベルのセット(キー)
	result = devkeyboard->SetCooperativeLevel(winApp->getHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	// 入力データ形式のセット(マウス)
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	// 排他制御レベルのセット(マウス)
	result = devMouse->SetCooperativeLevel(winApp->getHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	
	//スティックがついている時
	if (isPad)
	{
		// 入力データ形式のセット(ジョイステック)
		result = devStick->SetDataFormat(&c_dfDIJoystick); // 標準形式

		//軸モードを絶対値モードとして設定
		DIPROPDWORD diprop;
		ZeroMemory(&diprop, sizeof(diprop));
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モードの指定(DIPROPAXISMODE_RELにしたら相対値)

		devStick->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		//軸モードの設定
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.diph.dwObj = DIJOFS_X;
		diprg.lMin = -1000;
		diprg.lMax = 1000;
		devStick->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_Y;
		devStick->SetProperty(DIPROP_RANGE, &diprg.diph);
		// RX軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_RX;
		devStick->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_RY;
		devStick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// 排他制御レベルのセット(ジョイステック)
		result = devStick->SetCooperativeLevel(winApp->getHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	}
}

//キーの押下をチェック
bool Input::PushKey(BYTE keyNumber) {
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	//指定キーを押していなければtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//キーのトリガーをチェック
bool Input::TriggerKey(BYTE keyNumber) {
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	//指定キーを押していなければtrueを返す
	if (key[keyNumber]&& !keyPre[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//キーの左ボタン押下をチェック
bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;
}

//キーの中ボタン押下をチェック
bool Input::PushMouseMiddle()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// 押していない
	return false;
}

//キーの左ボタントリガーをチェック
bool Input::TriggerMouseLeft()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// トリガーでない
	return false;
}

//スティックの傾け方向をチェック
char Input::LEFT_PadStickX()
{
	char point = 0;

	// スティックの方向判定
	if (padData.lX <= -500)
	{
		//左に傾けた
		point = -1;
	} else if (padData.lX >= 500)
	{
		//右に傾けた
		point = +1;

	}

	return point;
}

//スティックの傾け方向をチェック
char Input::LEFT_PadStickY()
{
	char point = 0;

	if (padData.lY <= -500)
	{
		//上に傾けた
		point = +1;
	} else if (padData.lY >= 500)
	{
		//下に傾けた
		point = -1;
	}

	return point;
}

//スティックの傾け方向をチェック
char Input::RIGHT_PadStickX()
{
	char point = 0;

	// スティックの方向判定
	if (padData.lRx <= -500)
	{
		//左に傾けた
		point = -1;
	} else if (padData.lRx >= 500)
	{
		//右に傾けた
		point = +1;

	}

	return point;
}

//スティックの傾け方向をチェック
char Input::RIGHT_PadStickY()
{
	char point = 0;

	if (padData.lRy <= -500)
	{
		//上に傾けた
		point = +1;
	} else if (padData.lRy >= 500)
	{
		//下に傾けた
		point = -1;
	}

	return point;
}

//ゲームパッドのボタンチェック
bool Input::PushPadButtons(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 31);

	if (keyNumber <= 9) {
		if (padData.rgbButtons[keyNumber] & 0x80)
		{
			return true;
		} 
	}
	else if (keyNumber >= 10 && keyNumber <= 13 && padData.rgdwPOV[0] != 0xFFFFFFFF) {
		const int Angle = 9000;
		const int maxButton = 10;
		if (keyNumber == 10 && padData.rgdwPOV[0] == Angle * (keyNumber - maxButton))
		{
			return true;
		} else if (keyNumber == 11 && padData.rgdwPOV[0] == Angle * (keyNumber - maxButton))
		{
			return true;
		} else if (keyNumber == 12 && padData.rgdwPOV[0] == Angle * (keyNumber - maxButton))
		{
			return true;
		} else if (keyNumber == 13 && padData.rgdwPOV[0] == Angle * (keyNumber - maxButton))
		{
			return true;
		}
	}

	return false;
}

//ゲームパッドのボタントリガーチェック
bool Input::TriggerPadButtons(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 31);

	if (keyNumber <= 9) {
		if (padData.rgbButtons[keyNumber] & 0x80 && !padDataPre.rgbButtons[keyNumber])
		{
			return true;
		}
	} else if (keyNumber >= 10 && keyNumber <= 13) {
		if (padData.rgdwPOV[keyNumber - 10] != 0xFFFFFFFF && !padDataPre.rgdwPOV[keyNumber - 10] != 0xFFFFFFFF)
		{
			return true;
		}
	}

	return false;
}


void Input::upDate() {
	HRESULT result;

	//-----------キー--------------//
	//キーボード情報の取得開始
	result = devkeyboard->Acquire();

	//前回キー入力を保存
	memcpy(keyPre, key, sizeof(key));

	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);

	//-----------マウス--------------//
	// マウス動作開始
	result = devMouse->Acquire();

	// 前回の入力を保存
	mouseStatePre = mouseState;

	// マウスの入力
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);

	//-----------スティック--------------//
	if (isPad)
	{
		// スティック動作開始
		result = devStick->Acquire();

		// 前回の入力を保存
		padDataPre = padData;

		// スティック入力
		result = devStick->GetDeviceState(sizeof(padData), &padData);
	}
}
