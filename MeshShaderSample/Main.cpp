#include "DX12U.h"
using namespace DX12U;
using namespace DirectX;
using Microsoft::WRL::Wrappers::Event;


class MeshShaderSample : public Game
{
private:
  struct Meshlet
  {
    UINT  VertsCount;
    UINT  VertsOffset;
    UINT  PrimsCount;
    UINT  PrimsOffset;
  };

  struct Vertex
  {
    XMFLOAT3  Position;
    XMFLOAT2  Texcoord;
  };

  struct PrimIndex
  {
    UINT8   i0;
    UINT8   i1;
    UINT8   i2;
    UINT8   i3;
  };

  UINT		m_FrameIndex;
  UINT64	m_FenceValues[2];

  ComPtr<ID3D12RootSignature>     m_RootSignature;
  ComPtr<ID3D12PipelineState>     m_PipelineState;
  ComPtr<ID3D12CommandAllocator>  m_CommandAllocator[2];
  ComPtr<ID3D12GraphicsCommandList6>  m_CommandList;
  ComPtr<ID3D12Fence1>  m_Fence;
  ComPtr<ID3D12Resource2> m_MeshletBuffer;
  ComPtr<ID3D12Resource2> m_VertexBuffer;
  ComPtr<ID3D12Resource2> m_VertsIndices;
  ComPtr<ID3D12Resource2> m_PrimsIndices;
  ComPtr<ID3D12Resource>  m_Texture[2];
  ComPtr<ID3D12DescriptorHeap> m_DescHeapSRV;
  D3D12_GPU_DESCRIPTOR_HANDLE m_HandleGPU;
  D3D12_CPU_DESCRIPTOR_HANDLE m_HandleCPU;
  UINT m_HandleIncSize;
  Event m_Event;


public:
  MeshShaderSample(UINT width = 1280, UINT height = 720) 
    : Game(L"MeshShaderSample", width, height)
    , m_FrameIndex(0)
    , m_FenceValues{}
    , m_HandleCPU{}
    , m_HandleGPU{}
    , m_HandleIncSize(0)
  {

  }

  ~MeshShaderSample()
  {

  }


protected:
  void InitMS()
  {
    auto& device = GetDevice();

    HRESULT hr(S_OK);
    {
      Meshlet meshlet = { 4, 0, 1, 0 };

      hr = device->CreateCommittedResource(
        &DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &DX12U_RESOURCE_DESC::Buffer(sizeof(meshlet)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_MeshletBuffer));
      {
        void* pOut;
        D3D12_RANGE readRange = {};
        hr = m_MeshletBuffer->Map(0, &readRange, &pOut);
        ::memcpy(pOut, &meshlet, sizeof(meshlet));
        D3D12_RANGE wroteRange = { 0, sizeof(meshlet) };
        m_MeshletBuffer->Unmap(0, &readRange);
      }
    }
    
    {
      Vertex vertices[] = {
        { XMFLOAT3(-0.5f, +0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(+0.5f, +0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(+0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
      };

      hr = device->CreateCommittedResource(
        &DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &DX12U_RESOURCE_DESC::Buffer(sizeof(vertices)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer));
      m_VertexBuffer->SetName(L"VB");
      {
        void* pOut;
        D3D12_RANGE readRange = {};
        hr = m_VertexBuffer->Map(0, &readRange, &pOut);
        ::memcpy(pOut, vertices, sizeof(vertices));
        D3D12_RANGE wroteRange = { 0, sizeof(vertices) };
        m_VertexBuffer->Unmap(0, &readRange);
      }
    }
    
    {
      UINT16 vertexIndices[] = {
        0,1,2,3
      };

      hr = device->CreateCommittedResource(
        &DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &DX12U_RESOURCE_DESC::Buffer(sizeof(vertexIndices)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertsIndices));
      m_VertsIndices->SetName(L"VI");
      {
        void* pOut;
        D3D12_RANGE readRange = {};
        hr = m_VertsIndices->Map(0, &readRange, &pOut);
        ::memcpy(pOut, vertexIndices, sizeof(vertexIndices));
        D3D12_RANGE wroteRange = { 0, sizeof(vertexIndices) };
        m_VertsIndices->Unmap(0, &readRange);
      }
    }

    {
      PrimIndex primsIndices = { 0,1,2,3 };

      hr = device->CreateCommittedResource(
        &DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &DX12U_RESOURCE_DESC::Buffer(sizeof(primsIndices)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_PrimsIndices));
      m_PrimsIndices->SetName(L"PI");

      {
        void* pOut;
        D3D12_RANGE readRange = {};
        hr = m_PrimsIndices->Map(0, &readRange, &pOut);
        ::memcpy(pOut, &primsIndices, sizeof(primsIndices));
        D3D12_RANGE wroteRange = { 0, sizeof(primsIndices) };
        m_PrimsIndices->Unmap(0, &readRange);
      }

    }
  }

  void UploadTex2D(const ComPtr<ID3D12Resource>& tex2d, const void* data)
  {
    HRESULT hr(S_OK);
    auto& device = GetDevice();

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
    UINT64 uploadSize, rowSizeInBytes;
    const D3D12_RESOURCE_DESC destDesc = tex2d->GetDesc();
    
    device->GetCopyableFootprints(&destDesc, 0, 1, 0, &footprint, nullptr, &rowSizeInBytes, &uploadSize);

    ComPtr<ID3D12Resource> uploadBuffer;
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
      ::memcpy(pOut, data, uploadSize);
      D3D12_RANGE wroteRange = { 0, (SIZE_T)uploadSize };
      uploadBuffer->Unmap(0, &readRange);
    }

    D3D12_TEXTURE_COPY_LOCATION uploadBufLocation = { uploadBuffer.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, footprint };
    D3D12_TEXTURE_COPY_LOCATION nativeBufLocation = { tex2d.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };

    hr = m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), nullptr);// m_PipelineState.Get());
    m_CommandList->ResourceBarrier(1, &DX12U_RESOURCE_BARRIER::Transition(tex2d.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
    m_CommandList->CopyTextureRegion(&nativeBufLocation, 0, 0, 0, &uploadBufLocation, nullptr);
    D3D12_RESOURCE_BARRIER transition2 = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { tex2d.Get(), 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };

    m_CommandList->ResourceBarrier(1, &transition2);
    m_CommandList->Close();
    this->ExecuteCommandList(m_CommandList);
    this->WaitForGPU();

  }

  void InitPS()
  {
    HRESULT hr(S_OK);
    auto& device = GetDevice();

    //  create textures
    {
      UINT8* pData = new UINT8[256 * 256 * 4];
      memset(pData, 0, 256 * 256 * 4);

      for (UINT i = 0; i < 2; i++) {
        hr = device->CreateCommittedResource(
          &DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
          D3D12_HEAP_FLAG_NONE,
          &DX12U_RESOURCE_DESC::Tex2D(256, 256, DXGI_FORMAT_R8G8B8A8_UNORM),
          D3D12_RESOURCE_STATE_COPY_DEST,
          nullptr,
          IID_PPV_ARGS(&m_Texture[i]));
      }

      for (UINT y = 0; y < 256; y++)
      {
        for (UINT x = 0; x < 256; x++)
        {
          pData[y * 1024 + x * 4 + 0] = x;
          pData[y * 1024 + x * 4 + 1] = y;
          pData[y * 1024 + x * 4 + 2] = 0xFF - y;
          pData[y * 1024 + x * 4 + 3] = 0xFF;

        }
      }

      for (UINT i = 0; i < 2; i++) {
        this->UploadTex2D(m_Texture[i], pData);
      }


      delete[] pData;
    }
  }



  void OnInit()
  {
    HRESULT hr(S_OK);
    auto& device = GetDevice();

    {
      DX12U_ROOT_PARAMETER    rootParams[3];
      DX12U_DESCRIPTOR_RANGE  descRanges[3];
      //  for PS
      descRanges[0] = DX12U_DESCRIPTOR_RANGE::SRV(0, 2);
      rootParams[0] = DX12U_ROOT_PARAMETER::DescTable(1, &descRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);
      //  for MS
      descRanges[1] = DX12U_DESCRIPTOR_RANGE::SRV(0, 4);
      rootParams[1] = DX12U_ROOT_PARAMETER::DescTable(1, &descRanges[1], D3D12_SHADER_VISIBILITY_MESH);
      //  for AS
      descRanges[2] = DX12U_DESCRIPTOR_RANGE::SRV(0, 1);
      rootParams[2] = DX12U_ROOT_PARAMETER::DescTable(1, &descRanges[2], D3D12_SHADER_VISIBILITY_AMPLIFICATION);

      DX12U_STATIC_SAMPLER_DESC sampler[1];
      sampler[0] = DX12U_STATIC_SAMPLER_DESC::Default(0);

      DX12U_VERSIONED_ROOT_SIGNATURE_DESC rootDesc(3, rootParams, 1, sampler);

      ComPtr<ID3D10Blob> signature;
      ComPtr<ID3D10Blob> errorblob;
      hr = D3D12SerializeVersionedRootSignature(&rootDesc, &signature, &errorblob);
      hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
    }

    ComPtr<ID3DBlob> bytecodeAS = LoadAsset(L"AmpShader.cso");
    ComPtr<ID3DBlob> bytecodeMS = LoadAsset(L"MeshShader.cso");
    ComPtr<ID3DBlob> bytecodePS = LoadAsset(L"PixelShader.cso");

    DX12U_MESH_SHADER_PIPEPLINE_STATE_STREAM pso;
    pso.RootSignature = m_RootSignature.Get();
    pso.AS = DX12U_SHADER_BYTECODE(bytecodeAS.Get());
    pso.MS = DX12U_SHADER_BYTECODE(bytecodeMS.Get());
    pso.PS = DX12U_SHADER_BYTECODE(bytecodePS.Get());

    D3D12_PIPELINE_STATE_STREAM_DESC  streamDesc;
    streamDesc.pPipelineStateSubobjectStream = &pso;
    streamDesc.SizeInBytes = sizeof(pso);
    hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&m_PipelineState));
    //  
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[0]));
    hr = m_CommandAllocator[0]->Reset();
    //  
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[1]));
    hr = m_CommandAllocator[1]->Reset();
    //  
    m_FrameIndex = this->GetFrameIndex();
    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[m_FrameIndex].Get(), m_PipelineState.Get(), IID_PPV_ARGS(&m_CommandList));
    hr = m_CommandList->Close();

    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
    m_Event = Event(::CreateEvent(nullptr, FALSE, FALSE, nullptr));

    hr = device->CreateDescriptorHeap(
      &DX12U_DESCRIPTOR_HEAP_DESC(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE),
      IID_PPV_ARGS(&m_DescHeapSRV));
      m_HandleGPU = m_DescHeapSRV->GetGPUDescriptorHandleForHeapStart();
      m_HandleCPU = m_DescHeapSRV->GetCPUDescriptorHandleForHeapStart();
      m_HandleIncSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    InitPS();
    InitMS();

    WaitForGPU();
  }

  void OnExit()
  {

  }
  void OnUpdate()
  {

  }
  void OnRender()
  {
    auto& device = this->GetDevice();

    HRESULT hr(S_OK);

    hr = m_CommandAllocator[m_FrameIndex]->Reset();
    hr = m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), m_PipelineState.Get());

    m_CommandList->RSSetViewports(1, &this->GetViewport());
    m_CommandList->RSSetScissorRects(1, &this->GetScissorRect());

    {
      m_CommandList->ResourceBarrier(1, &this->GetResourceBarrierRTV(m_FrameIndex, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

      auto handleRTV = this->GetCpuDescriptorHandleRTV(m_FrameIndex);
      auto handleDSV = this->GetCpuDescriptorHandleDSV();

      m_CommandList->OMSetRenderTargets(1, &handleRTV, false, &handleDSV);

      const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
      m_CommandList->ClearRenderTargetView(handleRTV, clearColor, 0, nullptr);
      m_CommandList->ClearDepthStencilView(handleDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
      {
        // setup descriptor table
        auto handleGPU = m_HandleGPU;
        auto handleCPU = m_HandleCPU;

        // for PS
        auto handlePS = handleGPU;
        {
          for (size_t i = 0; i < 2; i++) {
            device->CreateShaderResourceView(
              m_Texture[i].Get(),
              &DX12U_SHADER_RESOURCE_VIEW_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM),
              handleCPU);
            handleCPU.ptr += m_HandleIncSize;
            handleGPU.ptr += m_HandleIncSize;
          }
        }
        // for MS
        auto handleMS = handleGPU;
        {
          //  #t0
          device->CreateShaderResourceView(
            m_MeshletBuffer.Get(),
            &DX12U_SHADER_RESOURCE_VIEW_DESC::Buffer(1, sizeof(Meshlet)),
            handleCPU);
          handleCPU.ptr += m_HandleIncSize;
          handleGPU.ptr += m_HandleIncSize;
          //  #t1
          device->CreateShaderResourceView(
            m_VertexBuffer.Get(),
            &DX12U_SHADER_RESOURCE_VIEW_DESC::Buffer(4, sizeof(Vertex)),
            handleCPU);
          handleCPU.ptr += m_HandleIncSize;
          handleGPU.ptr += m_HandleIncSize;

          device->CreateShaderResourceView(
            m_VertsIndices.Get(),
            &DX12U_SHADER_RESOURCE_VIEW_DESC::Buffer(4, sizeof(UINT16)),
            handleCPU);
          handleCPU.ptr += m_HandleIncSize;
          handleGPU.ptr += m_HandleIncSize;

          device->CreateShaderResourceView(
            m_PrimsIndices.Get(),
            &DX12U_SHADER_RESOURCE_VIEW_DESC::Buffer(1, sizeof(PrimIndex)),
            handleCPU);
          handleCPU.ptr += m_HandleIncSize;
          handleGPU.ptr += m_HandleIncSize;
        }
        // for AS
        auto handleAS = handleGPU;
        {
          device->CreateShaderResourceView(
            m_MeshletBuffer.Get(),
            &DX12U_SHADER_RESOURCE_VIEW_DESC::Buffer(1, sizeof(Meshlet)),
            handleCPU);
          handleCPU.ptr += m_HandleIncSize;
          handleGPU.ptr += m_HandleIncSize;
        }

        {
          m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

          ID3D12DescriptorHeap* heaps[] = { m_DescHeapSRV.Get() };
          
          m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
          m_CommandList->SetGraphicsRootDescriptorTable(0, handlePS);
          m_CommandList->SetGraphicsRootDescriptorTable(1, handleMS);
          m_CommandList->SetGraphicsRootDescriptorTable(2, handleAS);

          m_CommandList->DispatchMesh(1, 1, 1);
        }
      }
      m_CommandList->ResourceBarrier(1, &this->GetResourceBarrierRTV(m_FrameIndex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    }
    m_CommandList->Close();


    this->ExecuteCommandList(m_CommandList);
    this->Present();
    this->MoveToNextFrame();
  }


  void WaitForGPU()
  {
    HRESULT hr(S_OK);

    this->Signal(m_Fence, m_FenceValues[m_FrameIndex]);

    hr = m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_Event.Get());
    ::WaitForSingleObjectEx(m_Event.Get(), INFINITE, FALSE);

    m_FenceValues[m_FrameIndex]++;
  }

  void MoveToNextFrame()
  {
    const UINT64 value = m_FenceValues[m_FrameIndex];
    
    m_FrameIndex = this->GetFrameIndex();

    this->Signal(m_Fence, m_FenceValues[m_FrameIndex]);
    if (m_Fence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
    {
      auto hr = m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_Event.Get());
      ::WaitForSingleObjectEx(m_Event.Get(), INFINITE, FALSE);
    }
    m_FenceValues[m_FrameIndex] = (value + 1);
  }

private:

private:

};

int WINAPI wWinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR lpCmdLine,
  _In_ int nShowCmd)
{
	return (MeshShaderSample().Run(hInstance, nShowCmd));
}
