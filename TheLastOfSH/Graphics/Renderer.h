#pragma once
#ifdef _WIN64
#include <Windows.h>
#include <d3d12.h>

namespace TheLastOfSH {

	struct Renderer {
		virtual void Present() = 0;
		virtual ID3D12Device8* GetDevice() const = 0;
		virtual ID3D12DescriptorHeap* GetTexturePool() const = 0;
		virtual ID3D12GraphicsCommandList* GetActiveCmdList() const = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ClearRenderTarget(float x, float y, float z, float w) = 0;
		virtual void AddComPtrRef(IUnknown* ptr) = 0;
	};

	Renderer* CreateRenderer(HWND hwnd, UINT w, UINT h);
	void RemoveRenderer(Renderer* pRenderer);
}
#endif
