#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include"Camera.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace std;

ID3D12Device* Object3d::device = nullptr;//デバイス
ID3D12GraphicsCommandList* Object3d::cmdList = nullptr;//コマンドリスト
bool Object3d::useDraw = false;//Sprite描画が初回かどうかの確認
ComPtr<ID3D12PipelineState> Object3d::pipelinestate;//パイプラインステートオブジェクト
ComPtr<ID3D12RootSignature> Object3d::rootsignature;//ルートシグネチャ
ComPtr<ID3D12DescriptorHeap> Object3d::descHeap;//テクスチャ用デスクリプタヒープの生成
ComPtr<ID3D12Resource> Object3d::texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列
//頂点データ
Object3d::Vertex Object3d::vertices[24] = {
	//前
	{{ -edge, -edge, -edge },{},{0.0f,1.0f}}, // 左下0
	{{ -edge, edge, -edge },{},{0.0f,0.0f}}, // 左上1
	{{ edge, -edge, -edge },{}, { 1.0f,1.0f }}, // 右下2
	{{ edge, edge, -edge },{}, {1.0f,0.0f}}, // 右上3
	//後
	{{ -edge, edge, edge },{}, {0.0f,0.0f}}, // 左上4
	{{ -edge, -edge, edge },{}, {0.0f,1.0f}}, // 左下5
	{{ edge, edge, edge },{}, {1.0f,0.0f}}, // 右上6
	{{ edge, -edge, edge },{}, {1.0f,1.0f}}, // 右下7
	//左
	{{ -edge, -edge, -edge },{}, {0.0f,1.0f}}, // 左下8
	{{ -edge, -edge, edge },{}, {0.0f,0.0f}}, // 左上9
	{{ -edge, edge, -edge },{}, {1.0f,1.0f}}, // 右下10
	{{ -edge, edge, edge },{}, {1.0f,0.0f}}, // 右上11
	//右
	{{ edge, -edge, edge },{}, {0.0f,1.0f}}, // 左下12
	{{ edge, -edge, -edge },{}, {0.0f,0.0f}}, // 左上13
	{{ edge, edge, edge },{}, {1.0f,1.0f}}, // 右下14
	{{ edge, edge, -edge },{}, {1.0f,0.0f}}, // 右上15
	//上
	{{ -edge, edge, -edge },{}, {0.0f,1.0f}}, // 左下16
	{{ -edge, edge, edge },{}, {0.0f,0.0f}}, // 左上17
	{{ edge, edge, -edge },{}, {1.0f,1.0f}}, // 右下18
	{{ edge, edge, edge },{}, {1.0f,0.0f}}, // 右上19
	//下
	{{ -edge, -edge, edge },{}, {0.0f,0.0f}}, // 左上20
	{{ -edge, -edge, -edge },{}, {0.0f,1.0f}}, // 左下21
	{{ edge, -edge, edge },{}, {1.0f,0.0f}}, // 右上22
	{{ edge, -edge, -edge },{}, {1.0f,1.0f}}, // 右下23
};
//インデックスデータ
const unsigned short Object3d::indices[36] = {
	//前
	0,1,2,2,1,3,
	//後
	4,5,6,6,5,7,
	//左
	8,9,10,10,9,11,
	//右
	12,13,14,14,13,15,
	//上
	16,17,18,18,17,19,
	//下
	20,21,22,22,21,23,
};

//-----------------使用時一回目だけこれらの処理を行う--------------------//
//3Dオブジェクト用パイプライン
void Object3d::Pipeline()
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicPS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	////頂点レイアウト配列の宣言と設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		 },
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	////パイプラインステート設定
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//ラスタライザステート
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
	blenddesc.BlendEnable = true;//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//テストの値を0%使う
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//テストの値を100%使う

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//深度ステンシルステート設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0レジスタ

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);//定数バッファビューとして初期化
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//テクスチャサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//バージョン自動判定でのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	////グラフィックスパイプラインステートの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
}

//テクスチャデスクリプタの生成
void Object3d::CommonCreate() {
	HRESULT result = S_FALSE;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

//------------------------------------------------------------------------//

//3Dオブジェクトの共通テクスチャ読み込み
void Object3d::LoadTexture(WindowApp* winApp, UINT texNum, const wchar_t* filename)
{
	HRESULT result;

	//異常な番号の指定
	assert(texNum <= textureNum - 1);

	////WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImage);

	const Image* img = scratchImage.GetImage(0, 0, 0);

	//テクスチャバッファの生成
	//リソース設定
	D3D12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//テクスチャバッファ生成
	result = device->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texBuffer[texNum]));

	//テクスチャバッファにデータ転送
	result = texBuffer[texNum]->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		(UINT)img->rowPitch,//１ラインサイズ
		(UINT)img->slicePitch//1枚サイズ
	);

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープのtexnumber番目にシェーダーリソースビューを作成
	device->CreateShaderResourceView(
		texBuffer[texNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), texNum,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

//3Dオブジェクト生成
void Object3d::Create(UINT texNumber) {
	HRESULT result = S_FALSE;

	this->texNumber = texNumber;

	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//法線方向の計算
	for (int i = 0; i < _countof(indices) / 3; i++)
	{
		//一時的に格納
		unsigned short index_0 = indices[i * 3 + 0];
		unsigned short index_1 = indices[i * 3 + 1];
		unsigned short index_2 = indices[i * 3 + 2];

		//頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index_0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index_1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index_2].pos);

		//p0->p1,p0->p2ベクトルを計算
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から直角なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index_0].normal, normal);
		XMStoreFloat3(&vertices[index_1].normal, normal);
		XMStoreFloat3(&vertices[index_2].normal, normal);
	}

	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	memcpy(indexMap, indices, sizeof(indices));
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	upDate({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 1,1,1,1 });
}

//メインに書く 立方体用
void Object3d::init(DirectXCommon* dXCommon)
{
	device = dXCommon->getdev();
	cmdList = dXCommon->getcmdList();

	//共通データ生成
	CommonCreate();

	//パイプライン設定
	Pipeline();
}

//描画準備
void Object3d::Predraw() {
	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate.Get());

	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

//3Dオブジェクト単体更新
void Object3d::upDate(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color)
{
	//ワールド行列変換
	XMMATRIX matWorld = XMMatrixIdentity();
	XMMATRIX matScale = XMMatrixScaling(size.x, size.y, size.z);
	matWorld *= matScale;

	XMMATRIX matRot;//角度
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rota.x));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rota.y));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rota.z));
	matWorld *= matRot;

	XMMATRIX matTrans;//平行方向
	matTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
	matWorld *= matTrans;

	//定数バッファにデータを転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
	constMap->color = color;

	constMap->mat = matWorld * Camera::View() * Camera::Projection();

	constBuff->Unmap(0, nullptr);
}

//3Dオブジェクト単体描画
void Object3d::draw(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color,bool isInvisible)
{
	//非表示フラグがtrue
	if (isInvisible == true)
	{
		//描画せずに抜ける
		return;
	}

	//二回目の呼び出しから呼ばれない
	if (useDraw == false)
	{
		Predraw();
		useDraw = true;
	}

	upDate(pos, rota, size, color);

	//頂点バッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画コマンド
	cmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

//useDrawの初期化
void Object3d::SetDraw() {
	useDraw = false;
}