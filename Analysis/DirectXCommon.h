#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>

#include"WindowApp.h"

class DirectXCommon
{
private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void init(WindowApp* winApp);
	void createDepth(WindowApp* winApp);
	void beforeDraw(WindowApp* winApp);
	void afterDraw();

	ID3D12Device* getdev() { return dev.Get(); }
	ID3D12GraphicsCommandList* getcmdList() { return cmdList.Get(); }

private:
	ComPtr<ID3D12Device> dev;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	//�[�x�o�b�t�@����
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};//�[�x�r���[�p�̃f�X�N���v�^�q�[�v����
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	UINT bbIndex;

};

