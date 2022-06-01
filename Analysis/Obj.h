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
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://�\���̐錾
		//���_�f�[�^3D
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

	//�}�e���A���p
	struct Material
	{
		std::string name;//�}�e���A����
		XMFLOAT3 ambient;//�A���r�G���g�e���x
		XMFLOAT3 diffuse;//�f�B�t���[�Y�e���x
		XMFLOAT3 specular;//�X�y�L�����e���x
		float alpha;//�A���t�@
		std::string textureFilename;//�e�N�X�`���t�@�C����
		//�R���X�g���N�^
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.3f,0.3f,0.3f };
			specular = { 0.3f,0.3f,0.3f };
			alpha = 1.0f;
		}
	};

	//�萔�o�b�t�@�p�f�[�^B0
	struct ConstBufferDataB0
	{
		XMMATRIX mat;
		XMFLOAT4 color;
	};

	//�萔�o�b�t�@�p�f�[�^B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;//�A���r�G���g�e���x
		float pad1;
		XMFLOAT3 diffuse;//�f�B�t���[�Y�e���x
		float pad2;
		XMFLOAT3 specular;//�X�y�L�����e���x
		float alpha;
	};

	//Obj�f�[�^
	struct Data
	{
		Material material;
		std::vector<Vertex>vertices;
		std::vector<unsigned short>indices;
	};

private://�֐��錾
	//���̂ݍs������
	//Obj�p�p�C�v���C��
	static void Pipeline();
	//Obj���ʃf�[�^����
	static void CommonCreate();

	//Obj�}�e���A���ǂݍ���
	static void LoadMaterial(UINT objNumber, const std::string& directoryPath, const std::string& filename);
	//Obj�̋��ʃe�N�X�`���ǂݍ���
	static void CommonLoadTexture(UINT objNumber, const std::string& directoryPath, const std::string& filename);
	//Obj�t�@�C���̓ǂݍ���
	static void LoadFile(UINT objNum, const std::string modelname);

	//Obj����
	virtual void Create();
	//�`��O�ݒ�
	virtual void Predraw();
	//Obj�P�̍X�V
	virtual void Update();

public://�֐��錾
		/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Obj() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Obj();

	//���C���ɏ���
	//������
	static void Baseinit(DirectXCommon* dXCommon);
	//Obj�t�@�C���̓ǂݍ���
	static void LoadObj(UINT objNumber, const std::string modelname);
	//�ǂݍ���obj���w�肵�ĕ`��ɕK�v�ȃf�[�^���쐬����
	virtual void CreateModel(UINT objNumber);
	//Obj�P�̕`��
	virtual void draw(XMFLOAT3 pos, XMFLOAT3 rota, float size, XMFLOAT4 color, bool isInvisible);
	//useDraw�𖈃t���[������������
	static void SetDraw();

	//���[���h�s��̎擾
	const XMMATRIX& GetMatWorld() { return matWorld; }

	//�R���C�_�[�Z�b�g
	void SetCollider(BaseCollider* collider);

	//�Փˎ��R�[���o�b�N�֐�
	virtual void OnCollision(const CollisionInfo& info) {}


private://�ÓI�����o�ϐ�
	static ID3D12Device* device;//�f�o�C�X
	static ID3D12GraphicsCommandList* cmdList;//�R�}���h���X�g
	static std::vector<Data> data;
	static bool useDraw;//Object�`�悪���񂩂ǂ����̊m�F
	static ComPtr<ID3D12PipelineState>pipelinestate;//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12RootSignature>rootsignature;//���[�g�V�O�l�`��
	static ComPtr<ID3D12DescriptorHeap>descHeap;//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	static const int textureNum = 512;//�e�N�X�`���ő�o�^��
	static ComPtr<ID3D12Resource>texBuffer[textureNum];//�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
	static int vecSize;//���݂̔z�񐔊m�F

private://���I�����o�ϐ�
	UINT texNumber = 0;//�e�N�X�`���ԍ�
	ComPtr<ID3D12Resource> constBuffB0;//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;//�萔�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView;//���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> indexBuff;//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView;//�C���f�b�N�X�o�b�t�@�r���[
	XMMATRIX matWorld;// ���[�J�����[���h�ϊ��s��
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	float scale = 0.0f;
	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

protected:
	//�N���X��
	const char* name = nullptr;
	//�R���C�_�[
	BaseCollider* collider = nullptr;

};

