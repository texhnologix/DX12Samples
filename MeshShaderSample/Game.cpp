#include "DX12U.h"

namespace DX12U
{
	Game::Game(const std::wstring& name, UINT width, UINT height)
		: m_Name(name)
		, m_Width(width)
		, m_Height(height)
		, m_Viewport(static_cast<FLOAT>(width), static_cast<FLOAT>(height))
		, m_ScissorRect(width, height)
		, m_HWND(0), m_DescSizeRTV(0), m_descSizeDSV(0)
	{
		WCHAR path[512];
		DWORD size = GetModuleFileName(nullptr, path, _countof(path));

		WCHAR* lastSlash = wcsrchr(path, L'\\');
		if (lastSlash)
		{
			*(lastSlash + 1) = L'\0';
		}
		m_BasePath = path;
	}

	Game::~Game()
	{

	}

	int Game::Run(HINSTANCE hInstance, int nShowCmd)
	{
		//  create window
		{
			WNDCLASSEXW wcex = {};

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WindowProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInstance;
			wcex.hIcon = 0;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = L"DX12UGameClass";
			wcex.hIconSm = 0;

			RegisterClassExW(&wcex);

			RECT rect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

			m_HWND = ::CreateWindowW(
				wcex.lpszClassName,
				m_Name.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rect.right - rect.left,
				rect.bottom - rect.top,
				nullptr,
				nullptr,
				hInstance,
				this);
			if (m_HWND == 0)
			{
				return -1;
			}
		}

		UINT factoryFlags = 0;

#if defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		//	create device
		{
			ComPtr<IDXGIFactory6> factory;
			auto hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));

			m_Device = CreateDevice(factory);
			//	
			if (m_Device != nullptr) {
				hr = m_Device->CreateCommandQueue(&DX12U_COMMAND_QUEUE_DESC(D3D12_COMMAND_LIST_TYPE_DIRECT), IID_PPV_ARGS(&m_CommandQueue));

				ComPtr<IDXGISwapChain1> swapChain;
				hr = factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), m_HWND, &DX12U_SWAP_CHAIN_DESC1(m_Width, m_Height), nullptr, nullptr, &swapChain);
				hr = factory->MakeWindowAssociation(m_HWND, DXGI_MWA_NO_ALT_ENTER);
				swapChain.As(&m_SwapChain);
				//	m_SwapChain->SetMaximumFrameLatency(2);

				{
					hr = m_Device->CreateDescriptorHeap(&DX12U_DESCRIPTOR_HEAP_DESC(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2), IID_PPV_ARGS(&m_DescHeapRTV));
					m_DescSizeRTV = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

					auto handleRTV = m_DescHeapRTV->GetCPUDescriptorHandleForHeapStart();
					for (UINT i = 0; i < 2; i++)
					{
						m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ResourceRTV[i]));
						m_Device->CreateRenderTargetView(m_ResourceRTV[i].Get(), nullptr, handleRTV);
						std::wstring name(L"RT");
						name += std::to_wstring(i);
						m_ResourceRTV[i]->SetName(name.c_str());
						handleRTV.ptr += m_DescSizeRTV;
					}
				}

				{
					hr = m_Device->CreateDescriptorHeap(&DX12U_DESCRIPTOR_HEAP_DESC(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1), IID_PPV_ARGS(&m_DescHeapDSV));
					m_descSizeDSV = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

					hr = m_Device->CreateCommittedResource(
						&DX12U_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
						D3D12_HEAP_FLAG_NONE,
						&DX12U_RESOURCE_DESC::Depth(m_Width, m_Height),
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						&DX12U_DEPTH_STENCIL_CLEAR_VALUE(),
						IID_PPV_ARGS(&m_ResourceDSV));
					auto handleDSV = m_DescHeapDSV->GetCPUDescriptorHandleForHeapStart();
					m_Device->CreateDepthStencilView(m_ResourceDSV.Get(), &DX12U_DEPTH_STENCIL_VIEW_DESC(), handleDSV);
					m_ResourceDSV->SetName(L"DS");
				}
			}
		}

		if (m_Device != nullptr) {
			this->OnInit();
			::ShowWindow(m_HWND, nShowCmd);

			MSG msg = {};
			while (msg.message != WM_QUIT)
			{
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			this->OnExit();

			return (0);
		} else  {
			return (-1);
		}
	}

	ComPtr<ID3DBlob> Game::LoadAsset(const std::wstring& name)
	{
		using Microsoft::WRL::Wrappers::FileHandle;

		std::wstring filename = m_BasePath + name;

		CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
		extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
		extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
		extendedParams.lpSecurityAttributes = nullptr;
		extendedParams.hTemplateFile = nullptr;

		FileHandle file(::CreateFile2(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));

		FILE_STANDARD_INFO fileInfo = {};
		if (!::GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
		{
			return nullptr;
		}

		if (fileInfo.EndOfFile.HighPart != 0)
		{
			return nullptr;
		}

		ID3DBlob* blob(nullptr);
		auto hr = ::D3DCreateBlob(fileInfo.EndOfFile.LowPart, &blob);

		if (!::ReadFile(file.Get(), blob->GetBufferPointer(), static_cast<DWORD>(blob->GetBufferSize()), nullptr, nullptr))
		{
			return nullptr;
		}

		return (blob);
	}


	LRESULT CALLBACK Game::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Game* self = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		switch (message)
		{
		case WM_CREATE:
			if (self == nullptr) {
				LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			}
			return 0;
		case WM_PAINT:
			if (self != nullptr) {
				self->OnUpdate();
				self->OnRender();
			}
			return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}
