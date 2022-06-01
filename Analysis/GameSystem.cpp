#include "GameSystem.h"

//DirectXの初期化
void GameSystem::systemInit(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName) {
	// メインループに入る前に精度を取得しておく
	QueryPerformanceFrequency(&timeFreq);

	// 1度取得しておく(初回計算用)
	QueryPerformanceCounter(&timeStart);

	//directX初期化
	dXCommon = new DirectXCommon();
	dXCommon->init(winApp);
}

//メインに書く（初期化処理）
void GameSystem::init(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName) {
	//directXのと入力の初期化
	systemInit(winApp, window_width, window_height, gameName);

	Object3d::init(dXCommon);
	Obj::Baseinit(dXCommon);
}

void GameSystem::afterInit(WindowApp* winApp) {
	Sprite::LoadTexture(winApp, dXCommon->getdev(), 0, L"Resources/LetterResources/debugfont.png");

	//深度の初期化
	dXCommon->createDepth(winApp);
}

//メインに書く（更新処理）
void GameSystem::upDate(WindowApp* winApp) {
	dXCommon->beforeDraw(winApp);
}

//デバッグ用数字
void GameSystem::debugNum(float x, float y, float z) {
	//数字のデバッグ
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

//メインに書く（描画処理）
void GameSystem::draw() {
	Object3d::SetDraw();
	Sprite::SetDraw();
	Obj::SetDraw();

	//コマンド実行
	dXCommon->afterDraw();
}

void GameSystem::frameRateKeep() {
	// 今の時間を取得
	QueryPerformanceCounter(&timeEnd);
	// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < MIN_FREAM_TIME) { // 時間に余裕がある
		// ミリ秒に変換
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000);

		timeBeginPeriod(1); // 分解能を上げる(こうしないとSleepの精度はガタガタ)
		Sleep(sleepTime);   // 寝る
		timeEndPeriod(1);   // 戻す

		// 次週に持ち越し(こうしないとfpsが変になる?)
		return;
	}

	if (frameTime > 0.0) { // 経過時間が0より大きい(こうしないと下の計算でゼロ除算になると思われ)
		fps = (fps * 0.99f) + (0.01f / frameTime); // 平均fpsを計算
#ifdef _DEBUG
// デバッグ用(デバッガにFSP出す)
#ifdef UNICODE
		std::wstringstream stream;
#else
		std::stringstream stream;
#endif
		stream << fps << " FPS" << std::endl;
		// カウンタ付けて10回に1回出力、とかにしないと見づらいかもね
		OutputDebugString(stream.str().c_str());
#endif // _DEBUG
	}

	timeStart = timeEnd; // 入れ替え
}

//メインに書く（エスケープが入力されたら終了する処理）
bool GameSystem::gameFin(WindowApp* winApp) {
	//×が押されたとき
	if (winApp->upDate() == true) {
		return true;
	} else { return false; }

}

//メインに書く（解放）
void GameSystem::systemDelete() {
	delete dXCommon;
}
