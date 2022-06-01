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
	// 全てのスプライトデータについて
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		spriteDatas[i] = new Sprite();
		// スプライトを生成する
		spriteDatas[i]->init(winApp, dXCommon, { 0.0f,0.0f });
	}
}

// 1文字列追加
void DebugText::Print(const std::string &text, float x, float y, float scale)
{
	// 全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		// 最大文字数超過
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char &character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// 座標計算
		DirectX::XMFLOAT2 position = { x + fontWidth * scale * i, y };
		DirectX::XMFLOAT2 texLeftTop = { (float)fontIndexX * fontWidth,(float)fontIndexY * fontHeight };
		DirectX::XMFLOAT2 texSize = { fontWidth,fontHeight };;
		DirectX::XMFLOAT2 size = { fontWidth * scale,fontWidth * scale };

		spriteDatas[spriteIndex]->SetCoordinate(position, texLeftTop, texSize, size);

		// 文字を１つ進める
		spriteIndex++;
	}
}

// まとめて描画
void DebugText::DrawAll(DirectXCommon *dXCommon)
{
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		// スプライト描画
		spriteDatas[i]->draw(dXCommon, 0, { 0, 0 }, 0, { 0,0 });
	}

	spriteIndex = 0;
}