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

private://�V���O���g���p�^�[��
	Input() = default;
	~Input() = default;

public://�V���O���g���p�^�[��
	Input(const Input& input) = delete;
	Input& operator=(const Input& input) = delete;

	static Input* GetInstance();


public://�֐��錾

	///summary
	///�L�[�̉������`�F�b�N
	///</summary>
	///<param nam="KeyNumber">�L�[�ԍ�(DIK_0��)</param>
	///<return>	������Ă��邩</retrun>
	bool PushKey(BYTE keyNumber);

	///summary
	///�L�[�̃g���K�[���`�F�b�N
	///</summary>
	///<param nam="KeyNumber">�L�[�ԍ�(DIK_0��)</param>
	///<return>	�g���K�[��</retrun>
	bool TriggerKey(BYTE keyNumber);

	///<summary>
	///�L�[�̍��{�^���������`�F�b�N
	///</summary>
	///<returns>������Ă��邩</returns>
	bool PushMouseLeft();

	///<summary>
	///�L�[�̒��{�^���������`�F�b�N
	///</summary>
	///<returns>������Ă��邩</returns>
	bool PushMouseMiddle();

	///<summary>
	///�L�[�̍��{�^���g���K�[���`�F�b�N
	///</summary>
	///<returns>�g���K�[��</returns>
	bool TriggerMouseLeft();

	///<summary>
	///�L�[�̒��{�^���g���K�[���`�F�b�N
	///</summary>
	///<returns>�g���K�[��</returns>
	bool TriggerMouseMiddle();

	///<summary>
	///���X�e�B�b�N�̌X���������`�F�b�N
	///</summary>
	///point +1�E/-1��
	///<returns>�������݂͂ǂ��炩</returns>
	char LEFT_PadStickX();

	///<summary>
	///���X�e�B�b�N�̌X���������`�F�b�N
	///</summary>
	///point +1��/-1��
	///<returns>�������݂͂ǂ��炩</returns>
	char LEFT_PadStickY();

	///<summary>
	///�E�X�e�B�b�N�̌X���������`�F�b�N
	///</summary>
	///point +1�E/-1��
	///<returns>�������݂͂ǂ��炩</returns>
	char RIGHT_PadStickX();

	///<summary>
	///�E�X�e�B�b�N�̌X���������`�F�b�N
	///</summary>
	///point +1��/-1��
	///<returns>�������݂͂ǂ��炩</returns>
	char RIGHT_PadStickY();

	///<summary>
	///�Q�[���p�b�h�̃{�^���`�F�b�N
	///</summary>
	///<returns>������Ă��邩</returns>
	bool PushPadButtons(BYTE keyNumber);

	///<summary>
	///�Q�[���p�b�h�̃{�^���`�F�b�N
	///</summary>
	///<returns>�g���K�[��</returns>
	bool TriggerPadButtons(BYTE keyNumber);

	void init(WindowApp* winApp);
	void upDate();

public://�Q�[���p�b�h�L�[�R���t�B�O
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
		//LT,RT���킩��Ȃ�
	};

private://�ϐ�
	//�L�[
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8>devkeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};//�O��L�[�̏��

	//�}�E�X
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};

	//�}�E�X
	ComPtr<IDirectInputDevice8> devStick;
	LPVOID parameter;
	DIJOYSTATE  padData;
	DIJOYSTATE  padDataPre;
	bool isPad = false;

};

