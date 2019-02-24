#include<windows.h>
#include<d3d11.h>
#include<DirectXColors.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include"KeyboardClass.h"


using namespace DirectX;
//Vertex struct
struct SimpleVertex
{
	//-position of the vertex
	XMFLOAT3 pos;
	XMFLOAT4 Color;
};
//-constant buffer struct
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	
};


//-Global Variables

//-for input
KeyboardClass keyboard;


//-For the window
LPCSTR WndClassName = "FirstWindow";	//-window class for the window
HWND hwnd = nullptr;					//-the handle of the window
HINSTANCE m_hInstance = nullptr;
int width = 800, height = 600;			//-width and height of the window
D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;


//-For crating device and swapchain
IDXGISwapChain* SwapChain = nullptr;
ID3D11Device* deviceObj = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

//-for input layout, vertex buffer, index buffer
ID3D11InputLayout* p_vertexLayout = nullptr;
ID3D11Buffer* p_vertexBuffer = nullptr;
ID3D11PixelShader* p_pixelShader = nullptr;
ID3D11PixelShader* p_PixelShaderSolid = nullptr;
ID3D11VertexShader* p_vertexShader = nullptr;
ID3D11Buffer* p_IndexBuffer = nullptr;
ID3D11Buffer* p_ConstantBuffer = nullptr;
ID3D11Texture2D* m_depthStencil = nullptr;
ID3D11DepthStencilView* m_depthStencilView = nullptr;

//-matrices
XMMATRIX m_World1;
XMMATRIX m_World2;
XMMATRIX m_View;
XMMATRIX m_Projection;

//Functions Declaration:
bool InitializeWindow(HINSTANCE hInstance, int nCmdShow);
			//-handle of the app  //-how the window is displayed	  //-returns F if is full, T if it's minimized

//Devices Initialization
HRESULT CreateDevAndSwap();

//Devices Cleaner
void CleaneupDevice();

//message loop
int messageloop();

//-windows callback procedure functions to capture messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);	

//Render function
void Render();

//function to process the keyboard
void update();

//-main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)			
{
	
	if (FAILED(InitializeWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(CreateDevAndSwap()))
	{
		CleaneupDevice();
		return 0;
	}
	//-call the messageloop function
	messageloop();

	CleaneupDevice();
	return 0;
}
bool InitializeWindow(HINSTANCE hInstance, int nCmdShow)
{
	//-Start creating the window

	//-create the window class structure and define it
	WNDCLASSEX wc;													//-creating a new extended window class
	wc.cbSize = sizeof(WNDCLASSEX);									//-size of the class
	wc.style = CS_HREDRAW | CS_VREDRAW;								//-style of the window (see documentation)
	wc.lpfnWndProc = WndProc;										//-pointer to the function we want to process the messages for
	wc.cbClsExtra = NULL;											//-number of extra bytes allocated after WNDCLASSEX
	wc.cbWndExtra = NULL;											//-Extra bytes after our windows instance
	wc.hInstance = hInstance;										//-handle to our current application
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);							//-icon of the window
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);						//-cursor of the window
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW -2 );					//-color of the window 
	wc.lpszClassName = WndClassName;								//-name of the window class 
	wc.lpszMenuName = NULL;											//-name of the menu attached to the window. We don't have one yet.
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);					//-icon of the taskbar

																	//-register the class. If it fails, get an error, if not, continue to creating the window...
	if (!RegisterClassEx(&wc))
	{
		MessageBox(0, "Window Class could not be registered...", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//-creating the window
	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		"DirectX11 - Tutorial",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//-check if the window was created
	if (!hwnd)
	{
		MessageBox(NULL, "The window couldn't be created...", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//-show the window
	ShowWindow(hwnd, nCmdShow);


	//-return true if there weren't any errors...
	return true;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

//-Create Device and SwapChain
HRESULT CreateDevAndSwap()
{
	HRESULT hr;

	//-create DXGI_SWAPCHAIN_DESC structure
	DXGI_SWAP_CHAIN_DESC sd;
	
	//-clear memory
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//-define "sd"
	sd.BufferCount = 1;												//-number of buffers
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//-format of the display
	sd.BufferDesc.Height = height;									//-height of the display
	sd.BufferDesc.Width = width;									//-width of the display
	sd.BufferDesc.RefreshRate.Numerator = 60;						
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;				//-flag that tells the application how the back buffer will be used
	sd.OutputWindow = hwnd;											//-the window that the swap chain will use to present images on the screen
	sd.SampleDesc.Count = 1;										//-SampleDesc is used to enable multi-sampling
	sd.SampleDesc.Quality = 0;										//-Since multi-sampling is not used, set count=1 and quality=0 to disable multi-sampling
	sd.Windowed = TRUE;												//-This either true or false depending on if we want windowed or full-screen

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL,
		D3D11_SDK_VERSION, &sd, &SwapChain, &deviceObj, NULL, &immediateContext)))
	{
		MessageBox(0, "The SwapChain initialization failed...", "Error", MB_OK);
		return FALSE;
	}

	//-create Render Target View for binding the back buffer of our swap chain as a render target

	//-create the backBuffer
	ID3D11Texture2D* pBackBuffer;
	hr = SwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),	//-a reference ID to the type of interface to change the back buffer. We use a 2d texture (ID3D11Texture2D).
		reinterpret_cast<void**>(&pBackBuffer));		//-a pointer to the BackBuffer we created above, which is the surface we will render to.
	if (FAILED(hr))
	{
		MessageBox(0, "The BackBuffer initialization failed...", "Error", MB_OK);
		return hr;
	}
	
	//-Create Render Target View..
	hr = deviceObj->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);

	//-release the backbuffer
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(0, "The Render Target View initialization failed ", "Error", MB_OK);
		return hr;
	}

	//-define the depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	
	hr = deviceObj->CreateTexture2D(&descDepth, nullptr, &m_depthStencil);
	if (FAILED(hr))
	{
		MessageBox(0, "Depth Stencil failed to create...", "ERROR", MB_OK);
		return hr;
	}

	//-create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = deviceObj->CreateDepthStencilView(m_depthStencil, &descDSV, &m_depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(0, "Depth Stencil View failed to create...", "ERROR", MB_OK);
		return hr;
	}

	//-bind the render target view to the pipeline
	immediateContext->OMSetRenderTargets(1, &renderTargetView, m_depthStencilView);

	//-initialize the viewport structure
	D3D11_VIEWPORT vp;
	vp.Height = (float)height;
	vp.Width = (float)width;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	
	//-set the viewport
	immediateContext->RSSetViewports(1, &vp);

	//-compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"Effect.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(0, "Vertex shader initialization failed... ", "Error", MB_OK);
		return hr;
	}

	//-create the vertex shader
	hr = deviceObj->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &p_vertexShader);
	if (FAILED(hr))
	{
		MessageBox(0, "Vertex shader creation failed... ", "Error", MB_OK);
		pVSBlob->Release();
		return hr;
	}
	//-define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[]=
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	//-create the input layout
	hr = deviceObj->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &p_vertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
	{
		MessageBox(0, "The Input Layout initialization failed ", "Error", MB_OK);
		return hr;
	}

	//-set the input layout
	immediateContext->IASetInputLayout(p_vertexLayout);

	//-compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"Effect.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(0, "The Pixel Shader initialization failed ", "Error", MB_OK);
		return hr;
	}

	//-create the pixel shader
	hr = deviceObj->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &p_pixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
	{
		MessageBox(0, "The Pixel Shader creation failed ", "Error", MB_OK);
		return hr;
	}

	//-create vertex buffer(Define buffer desc and subbresource data)
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		

	};

	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = deviceObj->CreateBuffer(&bd, &InitData, &p_vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, "Vertex Buffer initialization failed... ", "Error", MB_OK);
		return hr;
	}
	//-Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &p_vertexBuffer, &stride, &offset);

	//-Create Index Buffer
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = deviceObj->CreateBuffer(&bd, &InitData, &p_IndexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, "Index buffer creation failed... ", "Error", MB_OK);
		return hr;
	}
	//-set index buffer
	immediateContext->IASetIndexBuffer(p_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		
	//-set primitive topology
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//-Create Constant Buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = deviceObj->CreateBuffer(&bd, nullptr, &p_ConstantBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, "Constant buffer creation failed... ", "Error", MB_OK);
		return hr;
	}


	//-Initialize the world matrix
	m_World1 = XMMatrixIdentity();
	m_World2 = XMMatrixIdentity();


	//-Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	//-Initialize the projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

	return S_OK;
}
int messageloop()
{
	//-make an instance of the msg structure
	MSG msg = {0};

	//-clean the msg structure
	ZeroMemory(&msg, sizeof(MSG));

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			update();
		}
		else
		{
			Render();
		}
	}
	return (int)msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Check message for events
	switch (msg)    
	{
	//-process the key presses and key releases
	case WM_KEYDOWN:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat())
		{
			keyboard.OnKeyPressed(keycode);
		}
		else
		{
			// check to see if a key is auto repeat
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				//dispatch the char
				keyboard.OnKeyPressed(keycode);
			}
		}
		return 0;
	}

	case WM_KEYUP:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR:
	{
		//-a pressed char can be get by accesing the wParam
		unsigned char ch = static_cast<unsigned char>(wParam);
		//-check to see if auto repeats 
		if (keyboard.IsCharsAutoRepeat())
		{
			//call the onChar and pass the msg
			keyboard.OnChar(ch);
		}
		else
		{
			//check to see if a key is auto repeat
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				//dispatch the char
				keyboard.OnChar(ch);
			}
		}
		return 0;
	}
	case WM_DESTROY:    //if x button in top right was pressed
		PostQuitMessage(0);
		return 0;
	}
	//return the message for windows to handle it
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}
void Render()
{
	
	// Update our time
	static float t = 0.0f;
	if (driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else;
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	// 1st Cube: Rotate around the origin
	m_World1 = XMMatrixRotationY(t);
	if (keyboard.KeyIsPressed('W'))
	{
		XMMATRIX mScale = XMMatrixTranslation(0, 0.5, 0.5);
		m_World1 = mScale*m_World1;
	}

	// 2nd Cube:  Rotate around origin
	XMMATRIX mSpin = XMMatrixRotationZ(-t);
	XMMATRIX mOrbit = XMMatrixRotationY(-t * 2.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	XMMATRIX mbScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	m_World2 = mScale * mSpin * mTranslate * mOrbit;
	if (keyboard.KeyIsPressed('S'))
	{
		m_World2 = mbScale * mTranslate *mOrbit;
	}

	//
	// Clear the back buffer
	//
	immediateContext->ClearRenderTargetView(renderTargetView, Colors::MidnightBlue);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//
	// Update variables for the first cube
	//
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(m_World1);
	cb1.mView = XMMatrixTranspose(m_View);
	cb1.mProjection = XMMatrixTranspose(m_Projection);
	immediateContext->UpdateSubresource(p_ConstantBuffer, 0, nullptr, &cb1, 0, 0);

	//
	// Render the first cube
	//
	immediateContext->VSSetShader(p_vertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &p_ConstantBuffer);
	immediateContext->PSSetShader(p_pixelShader, nullptr, 0);
	immediateContext->DrawIndexed(36, 0, 0);
	
	//
	// Update variables for the second cube
	//
	ConstantBuffer cb2;
	cb2.mWorld = XMMatrixTranspose(m_World2);
	cb2.mView = XMMatrixTranspose(m_View);
	cb2.mProjection = XMMatrixTranspose(m_Projection);
	immediateContext->UpdateSubresource(p_ConstantBuffer, 0, nullptr, &cb2, 0, 0);

	//
	// Render the second cube
	//
	immediateContext->DrawIndexed(36, 0, 0);

	//
	// Present our back buffer to our front buffer
	//
	SwapChain->Present(0, 0);
	
}
void CleaneupDevice()
{
	//-clean all the devices created
	if (immediateContext) immediateContext->ClearState();

	if (m_depthStencil) m_depthStencil->Release();
	if (m_depthStencilView) m_depthStencilView->Release();
	if (p_ConstantBuffer) p_ConstantBuffer->Release();
	if (p_IndexBuffer) p_IndexBuffer->Release();
	if (p_vertexBuffer) p_vertexBuffer->Release();
	if (p_vertexLayout) p_vertexLayout->Release();
	if (p_vertexShader) p_vertexShader->Release();
	if (p_pixelShader) p_pixelShader->Release();
	if (p_PixelShaderSolid) p_PixelShaderSolid->Release();
	if (renderTargetView) renderTargetView->Release();
	if (SwapChain) SwapChain->Release();
	if (immediateContext) immediateContext->Release();
	if (deviceObj) deviceObj->Release();
}
void update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		//get a char
		unsigned char ch = keyboard.ReadChar();
		
		//output the char in the debug string
		std::string outmsg = "Char: ";
		outmsg += ch;
		outmsg += "\n";
		OutputDebugStringA(outmsg.c_str());
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		//-get a key
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
		std::string outmsg = "Keycode: ";
		outmsg += keycode;
		outmsg += "\n";
		OutputDebugStringA(outmsg.c_str());


	}
}

