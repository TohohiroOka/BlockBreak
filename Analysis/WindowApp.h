#pragma once
#include <Windows.h>

class WindowApp
{
public:
	int windowWidth;
	int windowHeight;
	//�E�B���h�E�v���\�W���̐���
	static LRESULT WindowProcdure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	MSG msg = {};

	//������
	void winStart(const int windowWidth, const int windowHeight, const wchar_t* gameName);

	//���b�Z�[�W�X�V
	bool upDate();

	//�o�^����
	void winRelease();

	//�O���Ŏg��������n��
	HWND getHwnd() { return hwnd; }
	HINSTANCE getWinInstance() { return winClass.hInstance; }

private:
	WNDCLASSEX winClass{}; // �E�B���h�E�N���X�̐ݒ�
	HWND hwnd = nullptr;
};

