#pragma once
#include"DirectXCommon.h"
#include"CollisionInfo.h"

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;

class Obj
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://構造体宣言
		//頂点データ3D
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

	//マテリアル用
	struct Material
	{
		std::string name;//マテリアル名
		XMFLOAT3 ambient;//アンビエント影響度
		XMFLOAT3 diffuse;//ディフューズ影響度
		XMFLOAT3 specular;//スペキュラ影響度
		float alpha;//アルファ
		std::string textureFilename;//テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.3f,0.3f,0.3f };
			specular = { 0.3f,0.3f,0.3f };
			alpha = 1.0f;
		}
	};

	//定数バッファ用データB0
	struct ConstBufferDataB0
	{
		XMMATRIX mat;
		XMFLOAT4 color;
	};

	//定数バッファ用データB1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;//アンビエント影響度
		float pad1;
		XMFLOAT3 diffuse;//ディフューズ影響度
		float pad2;
		XMFLOAT3 specular;//スペキュラ影響度
		float alpha;
	};

	//Objデータ
	struct Data
	{
		Material material;
		std::vector<Vertex>vertices;
		std::vector<unsigned short>indices;
	};

private://関数宣言
	//一回のみ行う処理
	//Obj用パイプライン
	static void Pipeline();
	//Obj共通データ生成
	static void CommonCreate();

	//Objマテリアル読み込み
	static void LoadMaterial(UINT objNumber, const std::string& directoryPath, const std::string& filename);
	//Objの共通テクスチャ読み込み
	static void CommonLoadTexture(UINT objNumber, const std::string& directoryPath, const std::string& filename);
	//Objファイルの読み込み
	static void LoadFile(UINT objNum, const std::string modelname);

	//Obj生成
	virtual void Create();
	//描画前設定
	virtual void Predraw();
	//Obj単体更新
	virtual void Update();

public://関数宣言
		/// <summary>
	/// コンストラクタ
	/// </summary>
	Obj() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Obj();

	//メインに書く
	//初期化
	static void Baseinit(DirectXCommon* dXCommon);
	//Objファイルの読み込み
	static void LoadObj(UINT objNumber, const std::string modelname);
	//読み込んだobjを指定して描画に必要なデータを作成する
	virtual void CreateModel(UINT objNumber);
	//Obj単体描画
	virtual void draw(XMFLOAT3 pos, XMFLOAT3 rota, float size, XMFLOAT4 color, bool isInvisible);
	//useDrawを毎フレーム初期化する
	static void SetDraw();

	//ワールド行列の取得
	const XMMATRIX& GetMatWorld() { return matWorld; }

	//コライダーセット
	void SetCollider(BaseCollider* collider);

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}


private://静的メンバ変数
	static ID3D12Device* device;//デバイス
	static ID3D12GraphicsCommandList* cmdList;//コマンドリスト
	static std::vector<Data> data;
	static bool useDraw;//Object描画が初回かどうかの確認
	static ComPtr<ID3D12PipelineState>pipelinestate;//パイプラインステートオブジェクト
	static ComPtr<ID3D12RootSignature>rootsignature;//ルートシグネチャ
	static ComPtr<ID3D12DescriptorHeap>descHeap;//テクスチャ用デスクリプタヒープの生成
	static const int textureNum = 512;//テクスチャ最大登録数
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列
	static int vecSize;//現在の配列数確認

private://動的メンバ変数
	UINT texNumber = 0;//テクスチャ番号
	ComPtr<ID3D12Resource> constBuffB0;//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;//定数バッファ
	ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;//頂点バッファビュー
	ComPtr<ID3D12Resource> indexBuff;//インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView;//インデックスバッファビュー
	XMMATRIX matWorld;// ローカルワールド変換行列
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	float scale = 0.0f;
	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

protected:
	//クラス名
	const char* name = nullptr;
	//コライダー
	BaseCollider* collider = nullptr;

};

