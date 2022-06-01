#pragma once
#include <Windows.h>

class WindowApp
{
public:
	int windowWidth;
	int windowHeight;
	//ウィンドウプロ―ジャの生成
	static LRESULT WindowProcdure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	MSG msg = {};

	//初期化
	void winStart(const int windowWidth, const int windowHeight, const wchar_t* gameName);

	//メッセージ更新
	bool upDate();

	//登録解除
	void winRelease();

	//外部で使う引数を渡す
	HWND getHwnd() { return hwnd; }
	HINSTANCE getWinInstance() { return winClass.hInstance; }

private:
	WNDCLASSEX winClass{}; // ウィンドウクラスの設定
	HWND hwnd = nullptr;
};

