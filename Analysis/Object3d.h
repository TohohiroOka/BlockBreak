#pragma once
#include"DirectXCommon.h"

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;

class Object3d
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://構造体宣言
	//定数バッファの構造体
	struct ConstBufferData {
		XMFLOAT4 color;//色
		XMMATRIX mat;//3D変換行列
	};

	//頂点データ3D
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

public://関数宣言
	//一回のみ行う処理
	//パイプライン生成
	static void Pipeline();
	//テクスチャデスクリプタの生成
	static void CommonCreate();

	//3Dオブジェクトの共通テクスチャ読み込み
	static void LoadTexture(WindowApp* winApp, UINT texNum, const wchar_t* filename);

	//描画準備
	void Predraw();
	//位置、回転等の更新
	void upDate(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color);
	//useDrawを毎フレーム初期化する
	static void SetDraw();

	//メインに書く
	static void init(DirectXCommon* dXCommon);
	void Create(UINT texNumber);//Object生成
	void draw(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color, bool Invisible);


private://静的メンバ変数
	static ID3D12Device* device;//デバイス
	static ID3D12GraphicsCommandList* cmdList;//コマンドリスト
	static bool useDraw;//Object描画が初回かどうかの確認
	static ComPtr<ID3D12PipelineState>pipelinestate;//パイプラインステートオブジェクト
	static ComPtr<ID3D12RootSignature>rootsignature;//ルートシグネチャ
	static ComPtr<ID3D12DescriptorHeap>descHeap;//テクスチャ用デスクリプタヒープの生成
	static const int textureNum = 512;//テクスチャ最大登録数
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列
	static const int edge = 5;//一辺の長さ
	static Vertex vertices[24];//頂点データ
	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	static const UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));
	static const unsigned short indices[36];//インデックスデータ
	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));

private://動的メンバ変数
	UINT texNumber = 0;//テクスチャ番号
	ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};//頂点バッファビュー
	ComPtr<ID3D12Resource> indexBuff;//インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView{};//インデックスバッファビュー
	ComPtr<ID3D12Resource> constBuff;//定数バッファ
	XMFLOAT2 texLeftTop = { 0,0 };//テクスチャの上下座標
	XMFLOAT2 texSise = { 500,500 };//テクスチャ切り出しサイズ
};

