#include "DebugText.h"

int DebugText::spriteIndex = 0;

DebugText::DebugText()
{
}

DebugText::~DebugText()
{
	for (int i = 0; i < _countof(spriteDatas); i++) {
		delete spriteDatas[i];
	}
}

void DebugText::init(WindowApp *winApp, DirectXCommon *dXCommon)
{
	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		spriteDatas[i] = new Sprite();
		// �X�v���C�g�𐶐�����
		spriteDatas[i]->init(winApp, dXCommon, { 0.0f,0.0f });
	}
}

// 1������ǉ�
void DebugText::Print(const std::string &text, float x, float y, float scale)
{
	// �S�Ă̕����ɂ���
	for (int i = 0; i < text.size(); i++)
	{
		// �ő啶��������
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char &character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// ���W�v�Z
		DirectX::XMFLOAT2 position = { x + fontWidth * scale * i, y };
		DirectX::XMFLOAT2 texLeftTop = { (float)fontIndexX * fontWidth,(float)fontIndexY * fontHeight };
		DirectX::XMFLOAT2 texSize = { fontWidth,fontHeight };;
		DirectX::XMFLOAT2 size = { fontWidth * scale,fontWidth * scale };

		spriteDatas[spriteIndex]->SetCoordinate(position, texLeftTop, texSize, size);

		// �������P�i�߂�
		spriteIndex++;
	}
}

// �܂Ƃ߂ĕ`��
void DebugText::DrawAll(DirectXCommon *dXCommon)
{
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++)
	{
		// �X�v���C�g�`��
		spriteDatas[i]->draw(dXCommon, 0, { 0, 0 }, 0, { 0,0 });
	}

	spriteIndex = 0;
}