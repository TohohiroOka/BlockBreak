#include "GameSystem.h"

//DirectX�̏�����
void GameSystem::systemInit(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName) {
	// ���C�����[�v�ɓ���O�ɐ��x���擾���Ă���
	QueryPerformanceFrequency(&timeFreq);

	// 1�x�擾���Ă���(����v�Z�p)
	QueryPerformanceCounter(&timeStart);

	//directX������
	dXCommon = new DirectXCommon();
	dXCommon->init(winApp);
}

//���C���ɏ����i�����������j
void GameSystem::init(WindowApp* winApp, int window_width, int window_height, const wchar_t* gameName) {
	//directX�̂Ɠ��͂̏�����
	systemInit(winApp, window_width, window_height, gameName);

	Object3d::init(dXCommon);
	Obj::Baseinit(dXCommon);
}

void GameSystem::afterInit(WindowApp* winApp) {
	Sprite::LoadTexture(winApp, dXCommon->getdev(), 0, L"Resources/LetterResources/debugfont.png");

	//�[�x�̏�����
	dXCommon->createDepth(winApp);
}

//���C���ɏ����i�X�V�����j
void GameSystem::upDate(WindowApp* winApp) {
	dXCommon->beforeDraw(winApp);
}

//�f�o�b�O�p����
void GameSystem::debugNum(float x, float y, float z) {
	//�����̃f�o�b�O
	swprintf_s(str, L"%f,%f,%f\n", x, y, z);
	OutputDebugString(str);
}

//���C���ɏ����i�`�揈���j
void GameSystem::draw() {
	Object3d::SetDraw();
	Sprite::SetDraw();
	Obj::SetDraw();

	//�R�}���h���s
	dXCommon->afterDraw();
}

void GameSystem::frameRateKeep() {
	// ���̎��Ԃ��擾
	QueryPerformanceCounter(&timeEnd);
	// (���̎��� - �O�t���[���̎���) / ���g�� = �o�ߎ���(�b�P��)
	frameTime = static_cast<float>(timeEnd.QuadPart - timeStart.QuadPart) / static_cast<float>(timeFreq.QuadPart);

	if (frameTime < MIN_FREAM_TIME) { // ���Ԃɗ]�T������
		// �~���b�ɕϊ�
		DWORD sleepTime = static_cast<DWORD>((MIN_FREAM_TIME - frameTime) * 1000);

		timeBeginPeriod(1); // ����\���グ��(�������Ȃ���Sleep�̐��x�̓K�^�K�^)
		Sleep(sleepTime);   // �Q��
		timeEndPeriod(1);   // �߂�

		// ���T�Ɏ����z��(�������Ȃ���fps���ςɂȂ�?)
		return;
	}

	if (frameTime > 0.0) { // �o�ߎ��Ԃ�0���傫��(�������Ȃ��Ɖ��̌v�Z�Ń[�����Z�ɂȂ�Ǝv���)
		fps = (fps * 0.99f) + (0.01f / frameTime); // ����fps���v�Z
#ifdef _DEBUG
// �f�o�b�O�p(�f�o�b�K��FSP�o��)
#ifdef UNICODE
		std::wstringstream stream;
#else
		std::stringstream stream;
#endif
		stream << fps << " FPS" << std::endl;
		// �J�E���^�t����10���1��o�́A�Ƃ��ɂ��Ȃ��ƌ��Â炢������
		OutputDebugString(stream.str().c_str());
#endif // _DEBUG
	}

	timeStart = timeEnd; // ����ւ�
}

//���C���ɏ����i�G�X�P�[�v�����͂��ꂽ��I�����鏈���j
bool GameSystem::gameFin(WindowApp* winApp) {
	//�~�������ꂽ�Ƃ�
	if (winApp->upDate() == true) {
		return true;
	} else { return false; }

}

//���C���ɏ����i����j
void GameSystem::systemDelete() {
	delete dXCommon;
}
