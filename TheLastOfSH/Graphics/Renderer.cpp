#ifdef _WIN64
#include "Renderer.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <combaseapi.h>
#include <wrl.h>
#include <vector>

namespace TheLastOfSH {

	struct MyRenderer : public Renderer {
		void Present() {
			static std::vector<uint64_t> fenceValue(3, 0);
			auto frameIndex = pSwapchain->GetCurrentBackBufferIndex();
			fenceValue[frameIndex] = mFenceValue++;
			pSwapchain->Present(1, 0);

			const uint64_t currentFenceValue = fenceValue[frameIndex];
			pMainQueue->Signal(pDxFence, currentFenceValue);

			frameIndex = pSwapchain->GetCurrentBackBufferIndex();

			if (pDxFence->GetCompletedValue() < fenceValue[frameIndex])
			{
				pDxFence->SetEventOnCompletion(fenceValue[frameIndex], pDxWaitIdleFenceEvent);
				WaitForSingleObjectEx(pDxWaitIdleFenceEvent, INFINITE, FALSE);
			}
		}

		MyRenderer(HWND hwnd, UINT w, UINT h) {
			D3D12CreateDevice(nullptr, (D3D_FEATURE_LEVEL)0xc200, IID_PPV_ARGS(&pDevice));

			D3D12_COMMAND_QUEUE_DESC queueDesc{};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pMainQueue));
			pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pDxFence));
			pDxWaitIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			mFenceValue = 1;

			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.BufferCount = 3;
			scDesc.Width = w;
			scDesc.Height = h;
			scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.SampleDesc.Count = 1;
			scDesc.SampleDesc.Quality = 0;
			scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			scDesc.Scaling = DXGI_SCALING_STRETCH;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
			fsSwapChainDesc.Windowed = TRUE;

			Microsoft::WRL::ComPtr<IDXGIFactory7> temp_factory;
#ifdef _DEBUG
			CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&temp_factory));
#else
			CreateDXGIFactory2(0, IID_PPV_ARGS(&temp_factory));
#endif
			Microsoft::WRL::ComPtr<IDXGISwapChain1> temp_sc;
			temp_factory->CreateSwapChainForHwnd(pMainQueue, hwnd, &scDesc, &fsSwapChainDesc, nullptr, &temp_sc);
			temp_sc->QueryInterface(&pSwapchain);
			temp_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

		}
		~MyRenderer() {
			const uint64_t fence = mFenceValue;
			pMainQueue->Signal(pDxFence, fence);
			if (pDxFence->GetCompletedValue() < fence)
			{
				pDxFence->SetEventOnCompletion(fence, pDxWaitIdleFenceEvent);
				WaitForSingleObject(pDxWaitIdleFenceEvent, INFINITE);
			}
			pDxFence->Release();
			pMainQueue->Release();
			CloseHandle(pDxWaitIdleFenceEvent);

			pSwapchain->Release();
			pDevice->Release();
		}

		ID3D12DescriptorHeap* GetTexturePool() const {
			return pTexturePool;
		}

		ID3D12Device8* pDevice = nullptr;
		IDXGISwapChain4* pSwapchain = nullptr;
		ID3D12CommandQueue* pMainQueue = nullptr;
		ID3D12Fence* pDxFence = nullptr;
		HANDLE pDxWaitIdleFenceEvent = NULL;
		uint64_t mFenceValue = 0;

		ID3D12DescriptorHeap* pTexturePool = nullptr;
	};

	Renderer* CreateRenderer(HWND hwnd, UINT w, UINT h) {
		return new MyRenderer(hwnd, w, h);
	}
	void RemoveRenderer(Renderer* r) {
		auto temp = dynamic_cast<MyRenderer*>(r);
		delete temp;
	}
}

#endif
