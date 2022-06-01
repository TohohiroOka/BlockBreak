#pragma once
#include "Sprite.h"
#include <string>

/// <summary>
/// デバッグ用文字表示
/// </summary>
class DebugText
{
public:
	// デバッグテキスト用のテクスチャ番号を指定
	static const int maxCharCount = 256;	// 最大文字数
	static const int fontWidth = 9;			// フォント画像内1文字分の横幅
	static const int fontHeight = 18;		// フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;	// フォント画像内1行分の文字数

	DebugText();
	~DebugText();

	void init(WindowApp* winApp, DirectXCommon* dXCommon);

	void Print(const std::string& text, float x, float y, float size);

	void DrawAll(DirectXCommon* dXCommon);

private:
	// スプライトデータの配列
	Sprite* spriteDatas[maxCharCount] = { };
	// スプライトデータ配列の添え字番号
	static int spriteIndex;
};

