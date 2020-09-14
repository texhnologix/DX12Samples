#include "DX12U.h"

namespace DX12U
{
	ComPtr<ID3D12Device8> CreateDevice(const ComPtr<IDXGIFactory6>& factory)
	{
		HRESULT hr(S_OK);

		ComPtr<IDXGIAdapter1> adapter;
		for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); i++)
		{
			DXGI_ADAPTER_DESC1 desc = {};
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

			ComPtr<ID3D12Device8> device;
			hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
			if (SUCCEEDED(hr)) {
				{
					D3D12_FEATURE_DATA_ROOT_SIGNATURE	signature = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
					hr = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signature, sizeof(signature));
					if (signature.HighestVersion < D3D_ROOT_SIGNATURE_VERSION_1_1)
					{
						continue;
					}
				}

				{
					D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
					hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
					if (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5)
					{
						continue;
					}
				}
				{
					D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
					hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
					if (options.ResourceBindingTier < D3D12_RESOURCE_BINDING_TIER_3) {
						continue;
					}

					if (options.TiledResourcesTier < D3D12_TILED_RESOURCES_TIER_3) {

					}
				}
				{
					D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
					hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
					if (options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_1) {
						//	continue;
					}
				}

				{
					D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
					hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6));
					if (options6.VariableShadingRateTier < D3D12_VARIABLE_SHADING_RATE_TIER_2) {
						continue;
					}
				}

				{
					D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
					hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));
					if (options7.MeshShaderTier < D3D12_MESH_SHADER_TIER_1)
					{
						continue;
					}
					if (options7.SamplerFeedbackTier < D3D12_SAMPLER_FEEDBACK_TIER_0_9)
					{
						continue;
					}
				}



				return (device);
			}
		}
		return (nullptr);
	}


	void CopyResource(const ComPtr<ID3D12GraphicsCommandList6>& commandList, const ComPtr<ID3D12Resource2>& resource, const void* buffer)
	{
		HRESULT hr(S_OK);

		ComPtr<ID3D12Device8> device;
		
		hr = commandList->GetDevice(IID_PPV_ARGS(&device));

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT64 uploadSize, rowSizeInBytes;
		const D3D12_RESOURCE_DESC destDesc = resource->GetDesc();

		device->GetCopyableFootprints(&destDesc, 0, 1, 0, &footprint, nullptr, &rowSizeInBytes, &uploadSize);

		ComPtr<ID3D12Resource2> uploadBuffer;
		hr = device->CreateCommittedResource(
			&DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&DX12U_RESOURCE_DESC::Buffer(uploadSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));

		{
			void* pOut;
			D3D12_RANGE readRange = {};
			hr = uploadBuffer->Map(0, &readRange, &pOut);
			::memcpy(pOut, buffer, uploadSize);
			D3D12_RANGE wroteRange = { 0, (SIZE_T)uploadSize };
			uploadBuffer->Unmap(0, &readRange);
		}


	}
}
