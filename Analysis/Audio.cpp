#include "Audio.h"
#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

//音声データ読み込み
SoundData Audio::SoundLoadWave(IXAudio2* xAudio2, const char* fileName) {
	SoundData soundData;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);

	//ファイルオープン失敗を検知
	assert(file.is_open());

	///wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//ファイルがRIFFがチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}

	//タイプがWAVEがチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};

	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0 ||
		strncmp(data.id, "LIST", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);

		//再度読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	///ファイルクローズ
	//Waveファイルを閉じる
	file.close();

	///読み込んだ音声データを返す
	//returnするための音声データ
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	CreateSoundData(xAudio2, soundData);
	float SourceVoiceChannelVolumes[1] = { 0.0 };
	soundData.pSourceVoice->SetChannelVolumes(1, SourceVoiceChannelVolumes);

	return soundData;
}

//音声データの解放
void Audio::SoundUnload(SoundData* soundData) {
	//バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//データの生成
void Audio::CreateSoundData(IXAudio2* xAudio2, SoundData& soundData) {
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&soundData.pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

}

//音声再生
bool Audio::SoundPlayWava(IXAudio2* xAudio2, SoundData& soundData, bool roop) {
	HRESULT result;

	XAUDIO2_VOICE_STATE xa3state;
	soundData.pSourceVoice->GetState(&xa3state);
	//0でないなら音楽が再生中になるためスルーする
	if (xa3state.BuffersQueued != 0)
	{
		return false;
	}

	//データ生成
	CreateSoundData(xAudio2, soundData);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (roop == true)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//波形データの再生
	result = soundData.pSourceVoice->SubmitSourceBuffer(&buf);
	result = soundData.pSourceVoice->Start();

	buf = { NULL };

	return true;
}

//音楽の停止
void Audio::StopSound(SoundData& soundData)
{
	soundData.pSourceVoice->Stop(0);
	soundData.pSourceVoice->FlushSourceBuffers();
	soundData.pSourceVoice->SubmitSourceBuffer(&buf);
}