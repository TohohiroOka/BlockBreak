#include "Sprite.h"
#include <cassert>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

bool Sprite::useInit = false;//Sprite生成が初回かどうかの確認
bool Sprite::useDraw = false;//Sprite描画が初回かどうかの確認
ComPtr<ID3D12PipelineState> Sprite::pipelinestate;//パイプラインステートオブジェクト
ComPtr<ID3D12RootSignature> Sprite::rootsignature;//ルートシグネチャ
XMMATRIX Sprite::matProjection;//射影行列
ComPtr<ID3D12DescriptorHeap> Sprite::descHeap;//テクスチャ用デスクリプタヒープの生成
ComPtr<ID3D12Resource> Sprite::texBuffer[textureNum];//テクスチャリソース(テクスチャバッファ)の配列


//-----------------使用時一回目だけこれらの処理を行う--------------------//
//スプライト用パイプライン
void Sprite::Pipeline(ID3D12Device *dev)
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVS.hlsl",  // シェーダファイル名
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

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	////ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePS.hlsl",   // シェーダファイル名
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

		std::copy_n((char *)errorBlob->GetBufferPointer(),
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
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//背面力リングをしない

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
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
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//深度値フォーマット
	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

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
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	////グラフィックスパイプラインステートの生成
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
}

//スプライト共通データ生成
void Sprite::CommonCreate(WindowApp *winApp, ID3D12Device *dev)
{
	HRESULT result = S_FALSE;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	//並行投影の射影行列生成
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)winApp->windowWidth, (float)winApp->windowHeight, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------//

//スプライトの共通テクスチャ読み込み
void Sprite::LoadTexture(WindowApp *winApp, ID3D12Device *dev, UINT texNum, const wchar_t *filename)
{
	//異常な番号の指定
	assert(texNum <= textureNum - 1);

	HRESULT result;

	////WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImage);

	const Image *img = scratchImage.GetImage(0, 0, 0);

	//テクスチャバッファの生成
	//リソース設定
	D3D12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//テクスチャバッファ生成
	result = dev->CreateCommittedResource(//GPUリソースの生成
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
	dev->CreateShaderResourceView(
		texBuffer[texNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), texNum,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

//------------------生成したい個数分行う処理------------------------------//
//スプライト生成
void Sprite::Create(WindowApp *winApp, ID3D12Device *dev, XMFLOAT2 anchor)
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] = {
		{{0.0f,0.0f,0.0f},{0.0f,1.0f}},//左上
		{{0.0f,10.0f,0.0f},{0.0f,0.0f}},//左下
		{{10.0f,10.0f,0.0f},{1.0f,1.0f}},//右下
		{{10.0f,0.0f,0.0f},{1.0f,0.0f}},//右上
	};

	//頂点バッファ生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//アンカーポイントをコピー
	anchorpoint = anchor;

	//頂点バッファの転送
	TransferVertexBuffer();

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	upDate(0, { 0,0 }, 0, { 0,0 }, { 1,1,1,1 }, false, false, false, false);

}

//メインに書く（初期化）
void Sprite::init(WindowApp *winApp, DirectXCommon *dXCommon, XMFLOAT2 anchor) {
	//二回目の呼び出しから呼ばれない
	if (useInit == false)
	{
		//共通データ生成
		CommonCreate(winApp, dXCommon->getdev());

		//パイプライン設定
		Pipeline(dXCommon->getdev());

		useInit = true;
	}
	//スプライト生成
	Create(winApp, dXCommon->getdev(), anchor);
}

//スプライト単体更新
void Sprite::upDate(UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color,
	bool isFlipX, bool isFlipY, bool Cut, bool copy)
{
	if (copy == true) {
		//位置コピー
		position = pos;
		//サイズをコピー
		size = size_C;
	}
	//テクスチャ番号コピー
	texNumber = texNum;
	//回転コピー
	rotation = rota;
	//反転フラグをコピー
	isFlip[0] = isFlipX;
	isFlip[1] = isFlipY;
	//切り取って表示するか
	isCut = Cut;

	//ワールド行列の更新
	XMMATRIX matWorld = XMMatrixIdentity();
	//z軸回転
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(0));
	//並行移動
	matWorld *= XMMatrixTranslation(
		position.x, position.y, 0.0f);

	TransferVertexBuffer();

	//定数バッファの転送
	ConstBufferData *constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->mat = matWorld * matProjection;
	constMap->color = color;
	constBuff->Unmap(0, nullptr);
}

//描画前設定
void Sprite::Predraw(DirectXCommon *dXCommon) {
	//パイプラインステートの設定
	dXCommon->getcmdList()->SetPipelineState(pipelinestate.Get());

	//ルートシグネチャの設定
	dXCommon->getcmdList()->SetGraphicsRootSignature(rootsignature.Get());

	//プリミティブ形状の設定コマンド
	dXCommon->getcmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//四角

	//デスクリプタヒープをセット
	ID3D12DescriptorHeap *ppHeaps[] = { descHeap.Get() };
	dXCommon->getcmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

//スプライト描画
void Sprite::draw(DirectXCommon *dXCommon, UINT texNum, XMFLOAT2 pos, float rota,
	XMFLOAT2 size_C, XMFLOAT4 color, bool isFlipX, bool isFlipY, bool Cut, bool isInvisible, bool copy)
{
	//非表示フラグがtrue
	if (isInvisible == true)
	{
		//描画せずに抜ける
		return;
	}

	upDate(texNum, pos, rota, size_C, color, isFlipX, isFlipY, Cut, copy);

	//二回目の呼び出しから呼ばれない
	if (useDraw == false)
	{
		Predraw(dXCommon);
		useDraw = true;
	}

	//頂点バッファをセット
	dXCommon->getcmdList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	dXCommon->getcmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	dXCommon->getcmdList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			dXCommon->getdev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画コマンド(四角)
	dXCommon->getcmdList()->DrawInstanced(4, 1, 0, 0);
}

//スプライト単体頂点バッファの転送
void Sprite::TransferVertexBuffer()
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] = {
		{{},{0.0f,1.0f}},
		{{},{0.0f,0.0f}},
		{{},{1.0f,1.0f}},
		{{},{1.0f,0.0f}},
	};

	//	左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorpoint.x) * size.x;
	float right = (1.0f - anchorpoint.x) * size.x;
	float top = (0.0f - anchorpoint.y) * size.y;
	float bottom = (1.0f - anchorpoint.y) * size.y;

	if (isFlip[0] == true)
	{
		//左右反転
		left = -left;
		right = -right;
	}

	if (isFlip[1] == true)
	{
		//左右反転
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,bottom,0.0f };
	vertices[LT].pos = { left,top,0.0f };
	vertices[RB].pos = { right,bottom,0.0f };
	vertices[RT].pos = { right,top,0.0f };

	//指定番号の画像の読み込みなら
	if (texBuffer[texNumber] != nullptr)
	{
		if (isCut == true)
		{
			//テクスチャ情報取得
			D3D12_RESOURCE_DESC resDesc = texBuffer[texNumber]->GetDesc();
			float tex_left = texLeftTop.x / resDesc.Width;
			float tex_right = (texLeftTop.x + texSize.x) / resDesc.Width;
			float tex_top = texLeftTop.y / resDesc.Height;
			float tex_bottom = (texLeftTop.y + texSize.y) / resDesc.Height;

			vertices[LB].uv = { tex_left,tex_bottom };
			vertices[LT].uv = { tex_left,tex_top };
			vertices[RB].uv = { tex_right,tex_bottom };
			vertices[RT].uv = { tex_right,tex_top };
		}
	}
	VertexPosUv *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}

//座標セット
void Sprite::SetCoordinate(XMFLOAT2 position_C, XMFLOAT2 texLeftTop_C, XMFLOAT2 texSize_C, XMFLOAT2 size_C) {
	position = position_C;
	texLeftTop = texLeftTop_C;
	texSize = texSize_C;
	size = size_C;
}

//useDrawの初期化
void Sprite::SetDraw() {
	useDraw = false;
}
