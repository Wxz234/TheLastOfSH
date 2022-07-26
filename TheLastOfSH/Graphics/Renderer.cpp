#ifdef _WIN64
#include "Renderer.h"

#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <combaseapi.h>
#include <wrl.h>
#include <winerror.h>
#include <vector>

#pragma warning(disable:4838)

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
#if defined(_DEBUG)
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}
#endif
			constexpr unsigned bufferCount = 3;
			D3D12CreateDevice(nullptr, (D3D_FEATURE_LEVEL)D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));

			D3D12_COMMAND_QUEUE_DESC queueDesc{};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pMainQueue));
			pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pDxFence));
			pDxWaitIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			mFenceValue = 1;

			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.BufferCount = bufferCount;
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

			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heapDesc.NumDescriptors = 1000000;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pTexturePool));

			D3D12_COMMAND_LIST_TYPE listDesc = D3D12_COMMAND_LIST_TYPE_DIRECT;
			for (unsigned i = 0;i < bufferCount; ++i) {
				ID3D12CommandAllocator* pAlloc;
				pDevice->CreateCommandAllocator(listDesc, IID_PPV_ARGS(&pAlloc));
				m_Allocators.push_back(pAlloc);
			}
			pDevice->CreateCommandList1(0, listDesc, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_list));

			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = bufferCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pRTVHeap));
			auto m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto rtvHandle = pRTVHeap->GetCPUDescriptorHandleForHeapStart();
			for (UINT n = 0; n < bufferCount; ++n)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets;
				pSwapchain->GetBuffer(n, IID_PPV_ARGS(&renderTargets));
				pDevice->CreateRenderTargetView(renderTargets.Get(), nullptr, rtvHandle);
				rtvHandle.ptr += m_rtvDescriptorSize;
			}

			D3D12_RESOURCE_DESC depthStencilDesc{};
			depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			depthStencilDesc.Alignment = 0;
			depthStencilDesc.Width = w;
			depthStencilDesc.Height = h;
			depthStencilDesc.DepthOrArraySize = 1;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
			depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE optClear;
			optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Stencil = 0;

			D3D12_HEAP_PROPERTIES heapProps{};
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProps.CreationNodeMask = 1;
			heapProps.VisibleNodeMask = 1;

			pDevice->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&optClear,
				IID_PPV_ARGS(&pDepth)
			);

			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pDSVHeap));
			pDevice->CreateDepthStencilView(pDepth, nullptr, pDSVHeap->GetCPUDescriptorHandleForHeapStart());

			_w = w;
			_h = h;
		}
		~MyRenderer() {
			const uint64_t fence = mFenceValue;
			pMainQueue->Signal(pDxFence, fence);
			if (pDxFence->GetCompletedValue() < fence)
			{
				pDxFence->SetEventOnCompletion(fence, pDxWaitIdleFenceEvent);
				WaitForSingleObject(pDxWaitIdleFenceEvent, INFINITE);
			}
			for (auto &alloc: m_Allocators) {
				alloc->Release();
			}
			for (auto& obj : m_obj) {
				obj->Release();
			}

			pDepth->Release();
			pDSVHeap->Release();
			pRTVHeap->Release();
			m_list->Release();
			pTexturePool->Release();
			pDxFence->Release();
			pMainQueue->Release();
			CloseHandle(pDxWaitIdleFenceEvent);

			pSwapchain->Release();
			pDevice->Release();
		}

		ID3D12Device8* GetDevice() const {
			return pDevice;
		}

		ID3D12DescriptorHeap* GetTexturePool() const {
			return pTexturePool;
		}

		ID3D12GraphicsCommandList* GetActiveCmdList() const {
			return m_list;
		}

		void BeginFrame() {
			auto frameIndex = pSwapchain->GetCurrentBackBufferIndex();
			m_Allocators[frameIndex]->Reset();
			m_list->Reset(m_Allocators[frameIndex], nullptr);
			Microsoft::WRL::ComPtr<ID3D12Resource> _res;
			pSwapchain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = _res.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			m_list->ResourceBarrier(1, &barrier);

			auto rtvHandle = pRTVHeap->GetCPUDescriptorHandleForHeapStart();
			auto m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			rtvHandle.ptr += (m_rtvDescriptorSize * frameIndex);
			m_list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

			D3D12_VIEWPORT m_viewport{ 0.f, 0.f, (float)_w, (float)_h, 0.f, 1.f };
			m_list->RSSetViewports(1, &m_viewport);
			D3D12_RECT m_scissorRect{ 0, 0, _w, _h };
			m_list->RSSetScissorRects(1, &m_scissorRect);
			m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		void EndFrame() {
			auto frameIndex = pSwapchain->GetCurrentBackBufferIndex();
			Microsoft::WRL::ComPtr<ID3D12Resource> _res;
			pSwapchain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = _res.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			m_list->ResourceBarrier(1, &barrier);
			m_list->Close();
			pMainQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_list);
			Present();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetActiveRTV() const {
			auto rtvHandle = pRTVHeap->GetCPUDescriptorHandleForHeapStart();
			auto frameIndex = pSwapchain->GetCurrentBackBufferIndex();
			auto m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			rtvHandle.ptr += (m_rtvDescriptorSize * frameIndex);
			return rtvHandle;
		}

		void AddComPtrRef(IUnknown* ptr) {
			IUnknown* temp = nullptr;
			ptr->QueryInterface(&temp);
			m_obj.push_back(temp);
		}

		void ClearRenderTarget(float x, float y, float z, float w) {
			auto rtv = GetActiveRTV();
			float color[4]{ x, y, z, w };
			m_list->ClearRenderTargetView(rtv, color, 0, nullptr);
		}

		ID3D12Device8* pDevice = nullptr;
		IDXGISwapChain4* pSwapchain = nullptr;
		ID3D12CommandQueue* pMainQueue = nullptr;
		ID3D12Fence* pDxFence = nullptr;
		HANDLE pDxWaitIdleFenceEvent = NULL;
		uint64_t mFenceValue = 0;

		ID3D12DescriptorHeap* pTexturePool = nullptr;

		ID3D12GraphicsCommandList* m_list = nullptr;
		std::vector<ID3D12CommandAllocator*> m_Allocators;

		ID3D12DescriptorHeap* pRTVHeap = nullptr;
		std::vector<IUnknown*> m_obj;

		ID3D12Resource *pDepth = nullptr;
		ID3D12DescriptorHeap* pDSVHeap = nullptr;

		UINT _w;
		UINT _h;
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
