#pragma once
#include"DirectXCommon.h"

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class Sprite
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://静的const引数
	static const int textureNum = 512;

public://構造体宣言
	//定数バッファの構造体
	struct ConstBufferData {
		XMFLOAT4 color;//色
		XMMATRIX mat;//3D変換行列
	};

	//頂点データ2D
	struct VertexPosUv {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

public://関数宣言
	//一回のみ行う処理
	//パイプライン生成
	void Pipeline(ID3D12Device *dev);
	//テクスチャデスクリプタの生成
	void CommonCreate(WindowApp *winApp, ID3D12Device *dev);

	//共通テクスチャ読み込み
	static void LoadTexture(WindowApp *winApp, ID3D12Device *dev, UINT texNum, const wchar_t *filename);

	//スプライト生成
	void Create(WindowApp *winApp, ID3D12Device *dev, XMFLOAT2 anchor);

	//位置、回転等の更新
	void upDate(UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color,
		bool isFlipX, bool isFlipY, bool Cut, bool copy);
	//描画前設定
	void Predraw(DirectXCommon *dXCommon);
	//スプライト単体頂点バッファの転送
	void TransferVertexBuffer();
	//座標セット
	void SetCoordinate(XMFLOAT2 position_C, XMFLOAT2 texLeftTop_C, XMFLOAT2 texSize_C, XMFLOAT2 size_C);
	//useDrawを毎フレーム初期化する
	static void SetDraw();

	//メインに書くもの
	void init(WindowApp *winApp, DirectXCommon *dXCommon, XMFLOAT2 anchor);
	void draw(DirectXCommon *dXCommon, UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color = { 1,1,1,1 },
		bool isFlipX = false, bool isFlipY = false, bool isCut = false, bool Invisible = false, bool copy = true);

private://静的メンバ変数
	static bool useInit;//Sprite生成が初回かどうかの確認
	static bool useDraw;//Sprite描画が初回かどうかの確認
	static ComPtr<ID3D12PipelineState>pipelinestate;//パイプラインステートオブジェクト
	static ComPtr<ID3D12RootSignature>rootsignature;//ルートシグネチャ
	static XMMATRIX matProjection;//射影行列
	static ComPtr<ID3D12DescriptorHeap>descHeap;//テクスチャ用デスクリプタヒープの生成
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列

private://動的メンバ変数
	ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};//頂点バッファビュー
	ComPtr<ID3D12Resource> constBuff;//定数バッファ
	float rotation = 0.0f;//z軸周りの回転角
	XMFLOAT2 position = { 0,0 };//座標
	XMFLOAT4 color = { 1,1,1,1 };//色
	UINT texNumber = 0;//テクスチャ番号
	XMFLOAT2 size;//拡大率
	XMFLOAT2 anchorpoint = { 0.0f,0.0f };//アンカーポイント
	bool isFlip[2] = { false,false };//左右,上下反転
	bool isCut = false;//切り出すか
	XMFLOAT2 texLeftTop = { 0,0 };//テクスチャの上下座標
	XMFLOAT2 texSize = { 500,500 };//テクスチャ切り出しサイズ
};

