#pragma once
#include"DirectXCommon.h"

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class BaseCollider;

class Object3d
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�\���̐錾
	//�萔�o�b�t�@�̍\����
	struct ConstBufferData {
		XMFLOAT4 color;//�F
		XMMATRIX mat;//3D�ϊ��s��
	};

	//���_�f�[�^3D
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

public://�֐��錾
	//���̂ݍs������
	//�p�C�v���C������
	static void Pipeline();
	//�e�N�X�`���f�X�N���v�^�̐���
	static void CommonCreate();

	//3D�I�u�W�F�N�g�̋��ʃe�N�X�`���ǂݍ���
	static void LoadTexture(WindowApp* winApp, UINT texNum, const wchar_t* filename);

	//�`�揀��
	void Predraw();
	//�ʒu�A��]���̍X�V
	void upDate(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color);
	//useDraw�𖈃t���[������������
	static void SetDraw();

	//���C���ɏ���
	static void init(DirectXCommon* dXCommon);
	void Create(UINT texNumber);//Object����
	void draw(XMFLOAT3 pos, XMFLOAT3 rota, XMFLOAT3 size, XMFLOAT4 color, bool Invisible);


private://�ÓI�����o�ϐ�
	static ID3D12Device* device;//�f�o�C�X
	static ID3D12GraphicsCommandList* cmdList;//�R�}���h���X�g
	static bool useDraw;//Object�`�悪���񂩂ǂ����̊m�F
	static ComPtr<ID3D12PipelineState>pipelinestate;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12RootSignature>rootsignature;//���[�g�V�O�l�`��
	static ComPtr<ID3D12DescriptorHeap>descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static const int textureNum = 512;//�e�N�X�`���ő�o�^��
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
	static const int edge = 5;//��ӂ̒���
	static Vertex vertices[24];//���_�f�[�^
	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	static const UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));
	static const unsigned short indices[36];//�C���f�b�N�X�f�[�^
	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));

private://���I�����o�ϐ�
	UINT texNumber = 0;//�e�N�X�`���ԍ�
	ComPtr<ID3D12Resource> vertBuff;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView{};//���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> indexBuff;//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView{};//�C���f�b�N�X�o�b�t�@�r���[
	ComPtr<ID3D12Resource> constBuff;//�萔�o�b�t�@
	XMFLOAT2 texLeftTop = { 0,0 };//�e�N�X�`���̏㉺���W
	XMFLOAT2 texSise = { 500,500 };//�e�N�X�`���؂�o���T�C�Y
};

