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
	// Microsoft::WRL::を省略
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
	//深度バッファ生成
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};//深度ビュー用のデスクリプタヒープ生成
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	UINT bbIndex;

};

