#pragma once
#include"DirectXCommon.h"

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class Sprite
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://�ÓIconst����
	static const int textureNum = 512;

public://�\���̐錾
	//�萔�o�b�t�@�̍\����
	struct ConstBufferData {
		XMFLOAT4 color;//�F
		XMMATRIX mat;//3D�ϊ��s��
	};

	//���_�f�[�^2D
	struct VertexPosUv {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

public://�֐��錾
	//���̂ݍs������
	//�p�C�v���C������
	void Pipeline(ID3D12Device *dev);
	//�e�N�X�`���f�X�N���v�^�̐���
	void CommonCreate(WindowApp *winApp, ID3D12Device *dev);

	//���ʃe�N�X�`���ǂݍ���
	static void LoadTexture(WindowApp *winApp, ID3D12Device *dev, UINT texNum, const wchar_t *filename);

	//�X�v���C�g����
	void Create(WindowApp *winApp, ID3D12Device *dev, XMFLOAT2 anchor);

	//�ʒu�A��]���̍X�V
	void upDate(UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color,
		bool isFlipX, bool isFlipY, bool Cut, bool copy);
	//�`��O�ݒ�
	void Predraw(DirectXCommon *dXCommon);
	//�X�v���C�g�P�̒��_�o�b�t�@�̓]��
	void TransferVertexBuffer();
	//���W�Z�b�g
	void SetCoordinate(XMFLOAT2 position_C, XMFLOAT2 texLeftTop_C, XMFLOAT2 texSize_C, XMFLOAT2 size_C);
	//useDraw�𖈃t���[������������
	static void SetDraw();

	//���C���ɏ�������
	void init(WindowApp *winApp, DirectXCommon *dXCommon, XMFLOAT2 anchor);
	void draw(DirectXCommon *dXCommon, UINT texNum, XMFLOAT2 pos, float rota, XMFLOAT2 size_C, XMFLOAT4 color = { 1,1,1,1 },
		bool isFlipX = false, bool isFlipY = false, bool isCut = false, bool Invisible = false, bool copy = true);

private://�ÓI�����o�ϐ�
	static bool useInit;//Sprite���������񂩂ǂ����̊m�F
	static bool useDraw;//Sprite�`�悪���񂩂ǂ����̊m�F
	static ComPtr<ID3D12PipelineState>pipelinestate;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12RootSignature>rootsignature;//���[�g�V�O�l�`��
	static XMMATRIX matProjection;//�ˉe�s��
	static ComPtr<ID3D12DescriptorHeap>descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��

private://���I�����o�ϐ�
	ComPtr<ID3D12Resource> vertBuff;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView{};//���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> constBuff;//�萔�o�b�t�@
	float rotation = 0.0f;//z������̉�]�p
	XMFLOAT2 position = { 0,0 };//���W
	XMFLOAT4 color = { 1,1,1,1 };//�F
	UINT texNumber = 0;//�e�N�X�`���ԍ�
	XMFLOAT2 size;//�g�嗦
	XMFLOAT2 anchorpoint = { 0.0f,0.0f };//�A���J�[�|�C���g
	bool isFlip[2] = { false,false };//���E,�㉺���]
	bool isCut = false;//�؂�o����
	XMFLOAT2 texLeftTop = { 0,0 };//�e�N�X�`���̏㉺���W
	XMFLOAT2 texSize = { 500,500 };//�e�N�X�`���؂�o���T�C�Y
};

