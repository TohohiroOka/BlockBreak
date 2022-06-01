#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

//チャンクヘッダ
struct ChunkHeader
{
	char id[4];//チャンク毎のID
	int	size;  // チャンクサイズ
};

//RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};

//FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;//"fmt"
	WAVEFORMATEX fmt;//波形フォーマット
};

//音声データ
struct SoundData
{
	WAVEFORMATEX wfex;//波形フォーマット
	BYTE* pBuffer;//バッファの先頭アドレス
	unsigned int bufferSize;//バッファのサイズ
	IXAudio2SourceVoice* pSourceVoice;
};

class Audio
{
public:
	//音声データ読み込み
	SoundData SoundLoadWave(IXAudio2* xAudio2, const char* fileName);
	
	//音声データの解放
	void SoundUnload(SoundData* soundData);

	//データの生成
	void CreateSoundData(IXAudio2* xAudio2, SoundData& soundData);

	//音声再生
	bool SoundPlayWava(IXAudio2* xAudio2, SoundData& soundData, bool roop);

	//サウンドの停止
	void StopSound(SoundData& soundData);

private:
	XAUDIO2_BUFFER buf{};

};

