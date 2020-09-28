#pragma	once
#pragma	comment(lib, "dxgi.lib")
#pragma	comment(lib, "d3d12.lib")
#pragma	comment(lib, "d3dcompiler.lib")
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <string>

using Microsoft::WRL::ComPtr;

namespace DX12U
{
	struct DX12U_VIEWPORT : public D3D12_VIEWPORT
	{
		DX12U_VIEWPORT(FLOAT width, FLOAT height)
		{
			TopLeftX = 0.0f;
			TopLeftY = 0.0f;
			Width = width;
			Height = height;
			MinDepth = 0.0f;
			MaxDepth = 1.0f;
		}
	};
	struct DX12U_RECT : public D3D12_RECT
	{
		DX12U_RECT(LONG width, LONG height)
		{
			left = 0;
			top = 0;
			right = width;
			bottom = height;
		}
	};

	struct DX12U_COMMAND_QUEUE_DESC : public D3D12_COMMAND_QUEUE_DESC
	{
		DX12U_COMMAND_QUEUE_DESC()
		{
			Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			NodeMask = 0;
		}

		DX12U_COMMAND_QUEUE_DESC(
			D3D12_COMMAND_LIST_TYPE type, 
			D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			UINT nodeMask = 0)
		{
			Type = type;
			Priority = priority;
			Flags = flags;
			NodeMask = nodeMask;
		}
	};

	struct DX12U_SWAP_CHAIN_DESC1 : public DXGI_SWAP_CHAIN_DESC1
	{
		DX12U_SWAP_CHAIN_DESC1(UINT width, UINT height)
		{
			Width = width;
			Height = height;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			Stereo = FALSE;
			SampleDesc.Count = 1;
			SampleDesc.Quality = 0;
			BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			BufferCount = 2;
			Scaling = DXGI_SCALING_STRETCH;
			AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		}
	};

	struct DX12U_DESCRIPTOR_HEAP_DESC : public D3D12_DESCRIPTOR_HEAP_DESC
	{
		DX12U_DESCRIPTOR_HEAP_DESC()
		{
			Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			NumDescriptors = 0;
			Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			NodeMask = 0;
		}

		DX12U_DESCRIPTOR_HEAP_DESC(
			D3D12_DESCRIPTOR_HEAP_TYPE type, 
			UINT numDescs = 1, 
			D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			UINT nodeMask = 0)
		{
			Type = type;
			NumDescriptors = numDescs;
			Flags = flags;
			NodeMask = nodeMask;
		}
	};

	struct DX12U_DEPTH_STENCIL_VIEW_DESC : public D3D12_DEPTH_STENCIL_VIEW_DESC
	{
		DX12U_DEPTH_STENCIL_VIEW_DESC()
		{
			Format = DXGI_FORMAT_D32_FLOAT;
			ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			Flags = D3D12_DSV_FLAG_NONE;
			Texture2D.MipSlice = 0;
		}
	};

	struct DX12U_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES
	{
		DX12U_HEAP_PROPERTIES()
		{
			Type = D3D12_HEAP_TYPE_DEFAULT;
			CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			CreationNodeMask = 1;
			VisibleNodeMask = 1;
		}
		DX12U_HEAP_PROPERTIES(
			D3D12_HEAP_TYPE type, 
			D3D12_CPU_PAGE_PROPERTY cpuPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			D3D12_MEMORY_POOL memoryPool = D3D12_MEMORY_POOL_UNKNOWN,
			UINT creationNodeMask = 1,
			UINT visibleNodeMask  = 1)
		{
			Type = type;
			CPUPageProperty = cpuPageProperty;
			MemoryPoolPreference = memoryPool;
			CreationNodeMask = creationNodeMask;
			VisibleNodeMask = visibleNodeMask;
		}
	};
	
	struct DX12U_COLOR_CLEAR_VALUE : public D3D12_CLEAR_VALUE
	{
		DX12U_COLOR_CLEAR_VALUE(DXGI_FORMAT format, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
		{
			Format = format;
			Color[0] = R;
			Color[1] = G;
			Color[2] = B;
			Color[3] = A;
		}
	};

	struct DX12U_DEPTH_STENCIL_CLEAR_VALUE : public D3D12_CLEAR_VALUE
	{
		DX12U_DEPTH_STENCIL_CLEAR_VALUE()
		{
			Format = DXGI_FORMAT_D32_FLOAT;
			DepthStencil.Depth = 1.0f;
			DepthStencil.Stencil = 0;
		}
		DX12U_DEPTH_STENCIL_CLEAR_VALUE(DXGI_FORMAT format, FLOAT depth = 1.0f, UINT8 stencil = 0)
		{
			Format = format;
			DepthStencil.Depth = depth;
			DepthStencil.Stencil = stencil;
		}
	};

	struct DX12U_RESOURCE_DESC : public D3D12_RESOURCE_DESC
	{
		inline static DX12U_RESOURCE_DESC Buffer(UINT64 totalBytes)
		{
			DX12U_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Alignment = 0;
			desc.Width = totalBytes;
			desc.Height = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			desc.MipLevels = 1;
			desc.DepthOrArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			return desc;
		}

		inline static DX12U_RESOURCE_DESC Depth(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT)
		{
			DX12U_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Width = width;
			desc.Height = height;
			desc.Format = format;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			desc.MipLevels = 0;
			desc.DepthOrArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

			return desc;
		}

		inline static DX12U_RESOURCE_DESC Tex2D(
			UINT width, 
			UINT height, 
			DXGI_FORMAT format,
			UINT16 mipLevel = 0,
			UINT16 arraySize = 1,
			UINT sampleCount = 1,
			UINT sampleQuality = 0,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
			D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN)
		{
			DX12U_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Width = width;
			desc.Height = height;
			desc.Format = format;
			desc.Flags = flags;
			desc.MipLevels = mipLevel;
			desc.DepthOrArraySize = arraySize;
			desc.SampleDesc.Count = sampleCount;
			desc.SampleDesc.Quality = sampleQuality;
			desc.Layout = layout;

			return desc;
		}
	};


	struct DX12U_SHADER_RESOURCE_VIEW_DESC : public D3D12_SHADER_RESOURCE_VIEW_DESC
	{
		static DX12U_SHADER_RESOURCE_VIEW_DESC Tex2D(DXGI_FORMAT format)
		{
			DX12U_SHADER_RESOURCE_VIEW_DESC desc;
			D3D12_SHADER_RESOURCE_VIEW_DESC& base = desc;

			base.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			base.Format = format;
			base.Texture2D.MipLevels = 1;
			base.Texture2D.MostDetailedMip = 0;
			base.Texture2D.PlaneSlice = 0;
			base.Texture2D.ResourceMinLODClamp = 0.0f;
			base.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			return (desc);
		}
		static DX12U_SHADER_RESOURCE_VIEW_DESC Buffer(UINT numElement, UINT elementSize)
		{
			DX12U_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
			D3D12_SHADER_RESOURCE_VIEW_DESC& base = viewDesc;

			base.Format = DXGI_FORMAT_UNKNOWN;
			base.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			base.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			base.Buffer.FirstElement = 0;
			base.Buffer.NumElements = numElement;
			base.Buffer.StructureByteStride = elementSize;
			base.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			return (viewDesc);
		}
	};

	struct DX12U_SHADER_BYTECODE : public D3D12_SHADER_BYTECODE
	{
		DX12U_SHADER_BYTECODE()
		{
			pShaderBytecode = nullptr;
			BytecodeLength = 0;
		};

		DX12U_SHADER_BYTECODE(ID3D10Blob* blob)
		{
			pShaderBytecode = blob->GetBufferPointer();
			BytecodeLength = blob->GetBufferSize();
		}

	};

	struct DX12U_RENDER_TARGET_BLEND_DESC : public D3D12_RENDER_TARGET_BLEND_DESC
	{
		DX12U_RENDER_TARGET_BLEND_DESC()
		{
			BlendEnable = FALSE;
			LogicOpEnable = FALSE;
			SrcBlend = D3D12_BLEND_ONE;
			DestBlend = D3D12_BLEND_ZERO;
			BlendOp = D3D12_BLEND_OP_ADD;
			SrcBlendAlpha = D3D12_BLEND_ONE;
			DestBlendAlpha = D3D12_BLEND_ZERO;
			BlendOpAlpha = D3D12_BLEND_OP_ADD;
			LogicOp = D3D12_LOGIC_OP_NOOP;
			RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}
	};

	struct DX12U_BLEND_DESC : D3D12_BLEND_DESC
	{
		DX12U_BLEND_DESC()
		{
			AlphaToCoverageEnable = FALSE;
			IndependentBlendEnable = FALSE;
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			{
				RenderTarget[i] = DX12U_RENDER_TARGET_BLEND_DESC();
			}
		}
	};

	struct DX12U_DEPTH_STENCILOP_DESC : D3D12_DEPTH_STENCILOP_DESC
	{
		DX12U_DEPTH_STENCILOP_DESC()
		{
			StencilFailOp = D3D12_STENCIL_OP_KEEP;
			StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			StencilPassOp = D3D12_STENCIL_OP_KEEP;
			StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		}
	};

	struct DX12U_DEPTH_STENCIL_DESC : D3D12_DEPTH_STENCIL_DESC
	{
		DX12U_DEPTH_STENCIL_DESC()
		{
			DepthEnable = TRUE;
			DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			StencilEnable = FALSE;
			StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			FrontFace = DX12U_DEPTH_STENCILOP_DESC();
			BackFace = DX12U_DEPTH_STENCILOP_DESC();
		}
	};
	struct DX12U_DEPTH_FORMAT
	{
		DXGI_FORMAT	Format;
		
		DX12U_DEPTH_FORMAT()
		{
			Format = DXGI_FORMAT_D32_FLOAT;
		}
	};

	struct DX12U_RASTERIZER_DESC : D3D12_RASTERIZER_DESC
	{
		DX12U_RASTERIZER_DESC()
		{
			FillMode = D3D12_FILL_MODE_SOLID;
			CullMode = D3D12_CULL_MODE_FRONT;
			FrontCounterClockwise = FALSE;
			DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			DepthClipEnable = TRUE;
			MultisampleEnable = FALSE;
			AntialiasedLineEnable = FALSE;
			ForcedSampleCount = 0;
			ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		}
	};

	struct DX12U_RT_FORMAT_ARRAY : D3D12_RT_FORMAT_ARRAY
	{
		DX12U_RT_FORMAT_ARRAY()
		{
			NumRenderTargets = 1;
			RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			RTFormats[1] = DXGI_FORMAT_UNKNOWN;
			RTFormats[2] = DXGI_FORMAT_UNKNOWN;
			RTFormats[3] = DXGI_FORMAT_UNKNOWN;
			RTFormats[4] = DXGI_FORMAT_UNKNOWN;
			RTFormats[5] = DXGI_FORMAT_UNKNOWN;
			RTFormats[6] = DXGI_FORMAT_UNKNOWN;
			RTFormats[7] = DXGI_FORMAT_UNKNOWN;
		}
	};

	struct DX12U_SAMPLE_DESC : DXGI_SAMPLE_DESC
	{
		DX12U_SAMPLE_DESC()
		{
			Count = 1;
			Quality = 0;
		}
	};
	struct DX12U_SAMPLE_MASK
	{
		UINT Mask;
		DX12U_SAMPLE_MASK()
		{
			Mask = UINT_MAX;
		}
		DX12U_SAMPLE_MASK(UINT mask)
		{
			Mask = mask;
		}
	};

	template <typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename DefaultArg = ValueType>
	class alignas(void*) DX12U_PIPELINE_STATE_STREAM_SUBOBJECT
	{
	private:
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE m_Type;
		ValueType m_Value;
	public:
		DX12U_PIPELINE_STATE_STREAM_SUBOBJECT() noexcept : m_Type(Type), m_Value(DefaultArg()) {}
		DX12U_PIPELINE_STATE_STREAM_SUBOBJECT(ValueType const& i) noexcept : m_Type(Type), m_Value(i) {}
		DX12U_PIPELINE_STATE_STREAM_SUBOBJECT& operator=(ValueType const& i) noexcept { m_Type = Type; m_Value = i; return *this; }
		operator ValueType const& () const noexcept { return m_Value; }
		operator ValueType& () noexcept { return m_Value; }
		ValueType* operator&() noexcept { return &m_Value; }
		ValueType const* operator&() const noexcept { return &m_Value; }
	};

	using DX12U_PIPELINE_STATE_STREAM_ROOT_SIGNATURE = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE>;
	using DX12U_PIPELINE_STATE_STREAM_VS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS>;
	using DX12U_PIPELINE_STATE_STREAM_PS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS>;
	using DX12U_PIPELINE_STATE_STREAM_AS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS>;
	using DX12U_PIPELINE_STATE_STREAM_MS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS>;
	using DX12U_PIPELINE_STATE_STREAM_CS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS>;
	using DX12U_PIPELINE_STATE_STREAM_BLEND = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND>;
	using DX12U_PIPELINE_STATE_STREAM_DEPTH_STENCIL = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_DEPTH_STENCIL_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL>;
	using DX12U_PIPELINE_STATE_STREAM_DS_FORMAT = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_DEPTH_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT>;
	using DX12U_PIPELINE_STATE_STREAM_RASTERIZER = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER>;
	using DX12U_PIPELINE_STATE_STREAM_RT_FORMATS = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS>;
	using DX12U_PIPELINE_STATE_STREAM_SAMPLE_DESC = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC>;
	using DX12U_PIPELINE_STATE_STREAM_SAMPLE_MASK = DX12U_PIPELINE_STATE_STREAM_SUBOBJECT<DX12U_SAMPLE_MASK, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK>;

	struct DX12U_MESH_SHADER_PIPEPLINE_STATE_STREAM
	{
		DX12U_PIPELINE_STATE_STREAM_ROOT_SIGNATURE  RootSignature;
		DX12U_PIPELINE_STATE_STREAM_MS              MS;
		DX12U_PIPELINE_STATE_STREAM_AS              AS;
		DX12U_PIPELINE_STATE_STREAM_PS              PS;
		DX12U_PIPELINE_STATE_STREAM_BLEND           Blend;
		DX12U_PIPELINE_STATE_STREAM_DEPTH_STENCIL   DepthStencil;
		DX12U_PIPELINE_STATE_STREAM_DS_FORMAT       DepthFormat;
		DX12U_PIPELINE_STATE_STREAM_RASTERIZER      Rasterizer;
		DX12U_PIPELINE_STATE_STREAM_RT_FORMATS      RTFormats;
		DX12U_PIPELINE_STATE_STREAM_SAMPLE_DESC     Sample;
		DX12U_PIPELINE_STATE_STREAM_SAMPLE_MASK     SampleMask;
	};

	struct DX12U_RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER
	{
		DX12U_RESOURCE_BARRIER()
		{
		}

		static inline DX12U_RESOURCE_BARRIER Transition(
			ID3D12Resource* pResource,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter,
			UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
		{
			DX12U_RESOURCE_BARRIER	self;
			D3D12_RESOURCE_BARRIER& base = self;
			base.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			base.Flags = flags;
			base.Transition.pResource = pResource;
			base.Transition.StateBefore = stateBefore;
			base.Transition.StateAfter = stateAfter;
			base.Transition.Subresource = subresource;

			return (self);
		}
	};

	struct DX12U_ROOT_SIGNATURE_DESC : public D3D12_ROOT_SIGNATURE_DESC1
	{
		DX12U_ROOT_SIGNATURE_DESC()
		{
			NumParameters = 0;
			pParameters = nullptr;
			NumStaticSamplers = 0;
			pStaticSamplers = nullptr;
			Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		}
	};

	struct DX12U_STATIC_SAMPLER_DESC : public D3D12_STATIC_SAMPLER_DESC
	{
		inline static DX12U_STATIC_SAMPLER_DESC Default(
			UINT shaderRegister,
			D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			DX12U_STATIC_SAMPLER_DESC desc;

			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.MipLODBias = 0;
			desc.MaxAnisotropy = 0;
			desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			desc.MinLOD = 0.0f;
			desc.MaxLOD = D3D12_FLOAT32_MAX;
			desc.ShaderRegister = shaderRegister;
			desc.RegisterSpace = 0;
			desc.ShaderVisibility = shaderVisibility;

			return (desc);
		}

	};

	struct DX12U_DESCRIPTOR_RANGE : public D3D12_DESCRIPTOR_RANGE1
	{
		DX12U_DESCRIPTOR_RANGE() noexcept = default;

		inline static DX12U_DESCRIPTOR_RANGE CBV(UINT baseReg, UINT numRegs, UINT regSpace = 0)
		{
			DX12U_DESCRIPTOR_RANGE self = {};
			self.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			self.BaseShaderRegister = baseReg;
			self.NumDescriptors = numRegs;
			self.RegisterSpace = regSpace;
			self.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			self.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			return (self);
		}

		inline static DX12U_DESCRIPTOR_RANGE SRV(UINT baseReg, UINT numRegs, UINT regSpace = 0)
		{
			DX12U_DESCRIPTOR_RANGE self = {};
			self.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			self.BaseShaderRegister = baseReg;
			self.NumDescriptors = numRegs;
			self.RegisterSpace = regSpace;
			self.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			self.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			return (self);
		}
	};

	struct DX12U_ROOT_PARAMETER : public D3D12_ROOT_PARAMETER1
	{
		DX12U_ROOT_PARAMETER() noexcept = default;

		inline static DX12U_ROOT_PARAMETER CBV(
			UINT baseReg, 
			UINT regSpace = 0,
			D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			DX12U_ROOT_PARAMETER self;
			self.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			self.ShaderVisibility = shaderVisibility;
			self.Descriptor.ShaderRegister = baseReg;
			self.Descriptor.RegisterSpace = regSpace;
			self.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

			return (self);
		}

		inline static DX12U_ROOT_PARAMETER Constant(
			UINT baseReg,
			UINT num32BitValues,
			UINT regSpace = 0,
			D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			DX12U_ROOT_PARAMETER self;
			self.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			self.ShaderVisibility = shaderVisibility;
			self.Constants.Num32BitValues = num32BitValues;
			self.Constants.ShaderRegister = baseReg;
			self.Constants.RegisterSpace = regSpace;
			return (self);
		}

		inline static DX12U_ROOT_PARAMETER Table(
			UINT numRange, 
			const DX12U_DESCRIPTOR_RANGE* pRanges, 
			D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			DX12U_ROOT_PARAMETER self;
			self.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			self.ShaderVisibility = shaderVisibility;
			self.DescriptorTable.NumDescriptorRanges = numRange;
			self.DescriptorTable.pDescriptorRanges = pRanges;

			return (self);
		}
	};

	struct DX12U_VERSIONED_ROOT_SIGNATURE_DESC : public D3D12_VERSIONED_ROOT_SIGNATURE_DESC
	{
		DX12U_VERSIONED_ROOT_SIGNATURE_DESC(
			UINT numParams,
			DX12U_ROOT_PARAMETER* pParams,
			UINT numSampler,
			DX12U_STATIC_SAMPLER_DESC* pSampler)
		{
			Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			Desc_1_1.NumParameters = numParams;
			Desc_1_1.pParameters = pParams;
			Desc_1_1.NumStaticSamplers = numSampler;
			Desc_1_1.pStaticSamplers = pSampler;
			Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		}
	};



	ComPtr<ID3D12Device8> CreateDevice(const ComPtr<IDXGIFactory6>& factory);

	class Game
	{
	private:
		std::wstring	m_Name;
		UINT					m_Width;
		UINT					m_Height;
		DX12U_VIEWPORT	m_Viewport;
		DX12U_RECT      m_ScissorRect;
		HWND					m_HWND;
		std::wstring	m_BasePath;
		//	
		ComPtr<ID3D12Device8>					m_Device;
		ComPtr<ID3D12CommandQueue>		m_CommandQueue;
		ComPtr<IDXGISwapChain3>				m_SwapChain;
		ComPtr<ID3D12DescriptorHeap>	m_DescHeapRTV;
		ComPtr<ID3D12DescriptorHeap>	m_DescHeapDSV;
		ComPtr<ID3D12Resource2>				m_ResourceRTV[2];
		ComPtr<ID3D12Resource2>				m_ResourceDSV;
		UINT	m_DescSizeRTV;
		UINT	m_descSizeDSV;
	public:
		Game(const std::wstring& name, UINT width, UINT height);
		virtual ~Game();

		int Run(HINSTANCE hInstance, int nShowCmd);

	protected:
		inline ComPtr<ID3D12Device8>& GetDevice() { return m_Device; }
		inline const DX12U_VIEWPORT& GetViewport() { return m_Viewport; }
		inline const DX12U_RECT& GetScissorRect() { return m_ScissorRect; }
		inline UINT GetFrameIndex() { return (m_SwapChain->GetCurrentBackBufferIndex()); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleRTV(UINT frameIndex)
		{
			auto handle = m_DescHeapRTV->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += (SIZE_T(frameIndex) * m_DescSizeRTV);
			return (handle);
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleDSV()
		{
			auto handle = m_DescHeapDSV->GetCPUDescriptorHandleForHeapStart();
			return (handle);
		}

		inline DX12U_RESOURCE_BARRIER GetResourceBarrierRTV(UINT frameIndex, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES affterState)
		{
			return (DX12U_RESOURCE_BARRIER::Transition(m_ResourceRTV[frameIndex].Get(), beforeState, affterState));
		}

		inline HRESULT Present()
		{
			return (m_SwapChain->Present(1, 0));
		}

		inline void ExecuteCommandList(const ComPtr<ID3D12GraphicsCommandList6>& commandList)
		{
			ID3D12CommandList* ppCommandLists[] = { commandList.Get() };

			return (m_CommandQueue->ExecuteCommandLists(1, ppCommandLists));
		}

		inline HRESULT Signal(const ComPtr<ID3D12Fence1>& fence, UINT64 value)
		{
			return m_CommandQueue->Signal(fence.Get(), value);
		}


		//	
		ComPtr<ID3DBlob> LoadAsset(const std::wstring& name);
	protected:
		virtual void OnInit() = 0;
		virtual void OnExit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
