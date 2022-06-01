#include "WindowApp.h"

LRESULT WindowApp::WindowProcdure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY)
	{
		PostQuitMessage((0));
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//ウィンドウの初期化
void WindowApp::winStart(const int window_Width, const int window_height,const wchar_t* gameName) {
	windowWidth = window_Width;
	windowHeight = window_height;


	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.lpfnWndProc = (WNDPROC)WindowProcdure;// ウィンドウプロシージャを設定
	winClass.lpszClassName = gameName;// ウィンドウクラス名
	winClass.hInstance = GetModuleHandle(nullptr);// ウィンドウハンドル
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);// カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&winClass);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);// 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(winClass.lpszClassName,// クラス名
		L"ブロック崩しの逆襲",// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,// 標準的なウィンドウスタイル
		CW_USEDEFAULT,// 表示X座標（OSに任せる）
		CW_USEDEFAULT,// 表示Y座標（OSに任せる）
		wrc.right - wrc.left,// ウィンドウ横幅
		wrc.bottom - wrc.top,// ウィンドウ縦幅
		nullptr,// 親ウィンドウハンドル
		nullptr,// メニューハンドル

		winClass.hInstance,// 呼び出しアプリケーションハンドル
		nullptr);// オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

//ループ処理
bool WindowApp::upDate(){
	//メッセージ処理
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//アプリケーションが終わる時にmessageがWM_QUITになる
	if (msg.message == WM_QUIT)
	{
		return true;
	}
	else { return false; }
}

//登録解除
void WindowApp::winRelease(){
	UnregisterClass(winClass.lpszClassName, winClass.hInstance);
}