#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

//�`�����N�w�b�_
struct ChunkHeader
{
	char id[4];//�`�����N����ID
	int	size;  // �`�����N�T�C�Y
};

//RIFF�w�b�_�`�����N
struct RiffHeader
{
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};

//FMT�`�����N
struct FormatChunk
{
	ChunkHeader chunk;//"fmt"
	WAVEFORMATEX fmt;//�g�`�t�H�[�}�b�g
};

//�����f�[�^
struct SoundData
{
	WAVEFORMATEX wfex;//�g�`�t�H�[�}�b�g
	BYTE* pBuffer;//�o�b�t�@�̐擪�A�h���X
	unsigned int bufferSize;//�o�b�t�@�̃T�C�Y
	IXAudio2SourceVoice* pSourceVoice;
};

class Audio
{
public:
	//�����f�[�^�ǂݍ���
	SoundData SoundLoadWave(IXAudio2* xAudio2, const char* fileName);
	
	//�����f�[�^�̉��
	void SoundUnload(SoundData* soundData);

	//�f�[�^�̐���
	void CreateSoundData(IXAudio2* xAudio2, SoundData& soundData);

	//�����Đ�
	bool SoundPlayWava(IXAudio2* xAudio2, SoundData& soundData, bool roop);

	//�T�E���h�̒�~
	void StopSound(SoundData& soundData);

private:
	XAUDIO2_BUFFER buf{};

};

