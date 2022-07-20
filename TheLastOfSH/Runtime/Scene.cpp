#ifdef _WIN64
#include "Scene.h"

#include <wrl.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>


namespace 
{
    std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name)
    {
        std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        if (!inFile)
        {
            wchar_t moduleName[_MAX_PATH] = {};
            if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
                throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");

            wchar_t drive[_MAX_DRIVE];
            wchar_t path[_MAX_PATH];

            if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
                throw std::runtime_error("_wsplitpath_s");

            wchar_t filename[_MAX_PATH];
            if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
                throw std::runtime_error("_wmakepath_s");

            inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        }
#endif

        if (!inFile)
            throw std::runtime_error("ReadData");

        const std::streampos len = inFile.tellg();
        if (!inFile)
            throw std::runtime_error("ReadData");

        std::vector<uint8_t> blob;
        blob.resize(size_t(len));

        inFile.seekg(0, std::ios::beg);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.read(reinterpret_cast<char*>(blob.data()), len);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.close();

        return blob;
    }

	inline D3D12_BLEND_DESC GetDefaultBlendState() {
		D3D12_BLEND_DESC temp{};
		temp.AlphaToCoverageEnable = FALSE;
		temp.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			temp.RenderTarget[i] = defaultRenderTargetBlendDesc;

		return temp;
	}
	inline D3D12_RASTERIZER_DESC GetDefaultRasterizerState() {
		D3D12_RASTERIZER_DESC temp{};
		temp.FillMode = D3D12_FILL_MODE_SOLID;
		temp.CullMode = D3D12_CULL_MODE_BACK;
		temp.FrontCounterClockwise = FALSE;
		temp.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		temp.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		temp.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		temp.DepthClipEnable = TRUE;
		temp.MultisampleEnable = FALSE;
		temp.AntialiasedLineEnable = FALSE;
		temp.ForcedSampleCount = 0;
		temp.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		return temp;
	}
}

namespace TheLastOfSH {
	struct MyScene : public Scene {
		MyScene(Renderer* r):pRenderer(r) {
			auto device = pRenderer->GetDevice();
			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{ 0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };
			Microsoft::WRL::ComPtr<ID3DBlob> signature;
			Microsoft::WRL::ComPtr<ID3DBlob> error;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

            auto vsBlob = ReadData(L"VertexShader.cso");
            auto psBlob = ReadData(L"PixelShader.cso");

            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            psoDesc.InputLayout = { 0, 0 };
            psoDesc.pRootSignature = m_rootSignature;
            psoDesc.VS = { vsBlob.data(), vsBlob.size() };
            psoDesc.PS = { psBlob.data(), psBlob.size() };
            psoDesc.RasterizerState = GetDefaultRasterizerState();
            psoDesc.BlendState = GetDefaultBlendState();
            psoDesc.SampleMask = 0xffffffff;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;

			device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

		}
		~MyScene() {
			m_rootSignature->Release();
			m_pipelineState->Release();
		}

		void Draw() {
			pRenderer->Present();
		}
		void LoadModel(Model* pModel) {}
		Renderer* pRenderer = nullptr;
		ID3D12RootSignature* m_rootSignature = nullptr;
		ID3D12PipelineState* m_pipelineState = nullptr;

	};

	Scene* CreateScene(Renderer* pRenderer) {
		MyScene* scene = new MyScene(pRenderer);
		return scene;
	}

	void RemoveScene(Scene* pScene) {
		auto temp = dynamic_cast<MyScene*>(pScene);
		delete temp;
	}
}

#endif
