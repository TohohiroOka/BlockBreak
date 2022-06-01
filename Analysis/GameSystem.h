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
public://�N���X�����Ŏg�p����
	//DirectX�ƃL�[�̏�����
	void systemInit(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName);
	//�[�x����
	void afterInit(WindowApp* winApp);

public://���C���ɏ���
	//����������
	void init(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName);
	//�X�V����
	void upDate(WindowApp* winApp);
	//�f�o�b�O�p����
	void debugNum(float x, float y, float z);
	//�`�揈��
	void draw();
	//�t���[�����[�g�Œ�
	void frameRateKeep();
	//�G�X�P�[�v�����͂��ꂽ��I�����鏈��
	bool gameFin(WindowApp* winApp);
	//���
	void systemDelete();

	//dxCommon��n��
	DirectXCommon* getdXCommon() { return dXCommon; }

private://������Ȃ��N���X�̐錾
	//directX������
	DirectXCommon* dXCommon = nullptr;

	//�����\���f�o�b�O�p
	wchar_t str[256] = {};

	//�t���[�����[�g�Œ�p
	const float MIN_FREAM_TIME = 1.0f / 60;
	float frameTime = 0;
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timeFreq;
	float fps = 0;
};