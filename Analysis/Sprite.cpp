#include "Sprite.h"
#include <cassert>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

bool Sprite::useInit = false;//Sprite���������񂩂ǂ����̊m�F
bool Sprite::useDraw = false;//Sprite�`�悪���񂩂ǂ����̊m�F
ComPtr<ID3D12PipelineState> Sprite::pipelinestate;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
ComPtr<ID3D12RootSignature> Sprite::rootsignature;//���[�g�V�O�l�`��
XMMATRIX Sprite::matProjection;//�ˉe�s��
ComPtr<ID3D12DescriptorHeap> Sprite::descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
ComPtr<ID3D12Resource> Sprite::texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��


//-----------------�g�p�����ڂ��������̏������s��--------------------//
//�X�v���C�g�p�p�C�v���C��
void Sprite::Pipeline(ID3D12Device *dev)
{
	HRESULT result;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVS.hlsl",  // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	////�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePS.hlsl",   // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	////���_���C�A�E�g�z��̐錾�Ɛݒ�
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

	////�p�C�v���C���X�e�[�g�ݒ�
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//���X�^���C�U�X�e�[�g
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�w�ʗ̓����O�����Ȃ�

	//�u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//�W���ݒ�
	blenddesc.BlendEnable = true;//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�e�X�g�̒l��0%�g��
	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//�e�X�g�̒l��100%�g��

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//�[�x�X�e���V���X�e�[�g�ݒ�
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//�[�x�l�t�H�[�}�b�g
	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0���W�X�^

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);//�萔�o�b�t�@�r���[�Ƃ��ď�����
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//�e�N�X�`���T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//�o�[�W������������ł̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	//���[�g�V�O�l�`���̐���
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = rootsignature.Get();

	////�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐���
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
}

//�X�v���C�g���ʃf�[�^����
void Sprite::CommonCreate(WindowApp *winApp, ID3D12Device *dev)
{
	HRESULT result = S_FALSE;

	//�f�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureNum;
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	//���s���e�̎ˉe�s�񐶐�
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)winApp->windowWidth, (float)winApp->windowHeight, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------//

//�X�v���C�g�̋��ʃe�N�X�`���ǂݍ���
void Sprite::LoadTexture(WindowApp *winApp, ID3D12Device *dev, UINT texNum, const wchar_t *filename)
{
	//�ُ�Ȕԍ��̎w��
	assert(texNum <= textureNum - 1);

	HRESULT result;

	////WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImage{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImage);

	const Image *img = scratchImage.GetImage(0, 0, 0);

	//�e�N�X�`���o�b�t�@�̐���
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//�e�N�X�`���o�b�t�@����
	result = dev->CreateCommittedResource(//GPU���\�[�X�̐���
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texBuffer[texNum]));

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuffer[texNum]->WriteToSubresource(
		0,
		nullptr,//�S�̈�փR�s�[
		img->pixels,//���f�[�^�A�h���X
		(UINT)img->rowPitch,//�P���C���T�C�Y
		(UINT)img->slicePitch//1���T�C�Y
	);

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v��texnumber�ԖڂɃV�F�[�_�[���\�[�X�r���[���쐬
	dev->CreateShaderResourceView(
		texBuffer[texNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), texNum,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

//------------------���������������s������------------------------------//
//�X�v���C�g����
void Sprite::Create(WindowApp *winApp, ID3D12Device *dev, XMFLOAT2 anchor)
{
	HRESULT result = S_FALSE;

	//���_�f�[�^
	VertexPosUv vertices[] = {
		{{0.0f,0.0f,0.0f},{0.0f,1.0f}},//����
		{{0.0f,10.0f,0.0f},{0.0f,0.0f}},//����
		{{10.0f,10.0f,0.0f},{1.0f,1.0f}},//�E��
		{{10.0f,0.0f,0.0f},{1.0f,0.0f}},//�E��
	};

	//���_�o�b�t�@����
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//�A���J�[�|�C���g���R�s�[
	anchorpoint = anchor;

	//���_�o�b�t�@�̓]��
	TransferVertexBuffer();

	//���_�o�b�t�@�r���[�̐���
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�萔�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	upDate(0, { 0,0 }, 0, { 0,0 }, { 1,1,1,1 }, false, false, false, false);

}

//���C���ɏ����i�������j
void Sprite::init(WindowApp *winApp, DirectXCommon *dXCommon, XMFLOAT2 anchor) {
	//���ڂ̌Ăяo������Ă΂�Ȃ�
	if (useInit == false)
	{
		//���ʃf�[�^����
		CommonCreate(winApp, dXCommon->getdev());

		//�p�C�v���C���ݒ�
		Pipeline(dXCommon->getdev());

		useInit = true;
	}
	//�X�v���C�g����
	Create(winApp, dXCommon->getdev(), anchor);
}

//�X�v���C�g�P�̍X�V
void Sprite::upDate(UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color,
	bool isFlipX, bool isFlipY, bool Cut, bool copy)
{
	if (copy == true) {
		//�ʒu�R�s�[
		position = pos;
		//�T�C�Y���R�s�[
		size = size_C;
	}
	//�e�N�X�`���ԍ��R�s�[
	texNumber = texNum;
	//��]�R�s�[
	rotation = rota;
	//���]�t���O���R�s�[
	isFlip[0] = isFlipX;
	isFlip[1] = isFlipY;
	//�؂����ĕ\�����邩
	isCut = Cut;

	//���[���h�s��̍X�V
	XMMATRIX matWorld = XMMatrixIdentity();
	//z����]
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(0));
	//���s�ړ�
	matWorld *= XMMatrixTranslation(
		position.x, position.y, 0.0f);

	TransferVertexBuffer();

	//�萔�o�b�t�@�̓]��
	ConstBufferData *constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->mat = matWorld * matProjection;
	constMap->color = color;
	constBuff->Unmap(0, nullptr);
}

//�`��O�ݒ�
void Sprite::Predraw(DirectXCommon *dXCommon) {
	//�p�C�v���C���X�e�[�g�̐ݒ�
	dXCommon->getcmdList()->SetPipelineState(pipelinestate.Get());

	//���[�g�V�O�l�`���̐ݒ�
	dXCommon->getcmdList()->SetGraphicsRootSignature(rootsignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dXCommon->getcmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//�l�p

	//�f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap *ppHeaps[] = { descHeap.Get() };
	dXCommon->getcmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

//�X�v���C�g�`��
void Sprite::draw(DirectXCommon *dXCommon, UINT texNum, XMFLOAT2 pos, float rota,
	XMFLOAT2 size_C, XMFLOAT4 color, bool isFlipX, bool isFlipY, bool Cut, bool isInvisible, bool copy)
{
	//��\���t���O��true
	if (isInvisible == true)
	{
		//�`�悹���ɔ�����
		return;
	}

	upDate(texNum, pos, rota, size_C, color, isFlipX, isFlipY, Cut, copy);

	//���ڂ̌Ăяo������Ă΂�Ȃ�
	if (useDraw == false)
	{
		Predraw(dXCommon);
		useDraw = true;
	}

	//���_�o�b�t�@���Z�b�g
	dXCommon->getcmdList()->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@���Z�b�g
	dXCommon->getcmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	dXCommon->getcmdList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			dXCommon->getdev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�`��R�}���h(�l�p)
	dXCommon->getcmdList()->DrawInstanced(4, 1, 0, 0);
}

//�X�v���C�g�P�̒��_�o�b�t�@�̓]��
void Sprite::TransferVertexBuffer()
{
	HRESULT result = S_FALSE;

	//���_�f�[�^
	VertexPosUv vertices[] = {
		{{},{0.0f,1.0f}},
		{{},{0.0f,0.0f}},
		{{},{1.0f,1.0f}},
		{{},{1.0f,0.0f}},
	};

	//	�����A����A�E���A�E��
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorpoint.x) * size.x;
	float right = (1.0f - anchorpoint.x) * size.x;
	float top = (0.0f - anchorpoint.y) * size.y;
	float bottom = (1.0f - anchorpoint.y) * size.y;

	if (isFlip[0] == true)
	{
		//���E���]
		left = -left;
		right = -right;
	}

	if (isFlip[1] == true)
	{
		//���E���]
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,bottom,0.0f };
	vertices[LT].pos = { left,top,0.0f };
	vertices[RB].pos = { right,bottom,0.0f };
	vertices[RT].pos = { right,top,0.0f };

	//�w��ԍ��̉摜�̓ǂݍ��݂Ȃ�
	if (texBuffer[texNumber] != nullptr)
	{
		if (isCut == true)
		{
			//�e�N�X�`�����擾
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

//���W�Z�b�g
void Sprite::SetCoordinate(XMFLOAT2 position_C, XMFLOAT2 texLeftTop_C, XMFLOAT2 texSize_C, XMFLOAT2 size_C) {
	position = position_C;
	texLeftTop = texLeftTop_C;
	texSize = texSize_C;
	size = size_C;
}

//useDraw�̏�����
void Sprite::SetDraw() {
	useDraw = false;
}
