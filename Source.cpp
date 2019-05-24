#include<windows.h>
#include<d3d11.h>
#include<DirectXColors.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include"KeyboardClass.h"
#include<stdlib.h>
#include<iostream>
#include<SpriteBatch.h>
#include<SpriteFont.h>
#include"Includes\SpriteBatch.h"
#include"Includes\SpriteFont.h"
#include<chrono>
#include<string>
#include "DXUT.h"
#include"Includes\CommonStates.h"
#include"Includes\Effects.h"
#include"Includes\PrimitiveBatch.h"
#include"Includes\VertexTypes.h"
#include<WICTextureLoader.h>
#include"ImGui\imgui.h"
#include"ImGui\imgui_impl_dx11.h"
#include"ImGui\imgui_impl_win32.h"
#include<cmath>
#pragma comment(lib, "DirectXTK.lib")



#ifdef USE_DIRECTX_SDK
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\include\xinput.h>
#pragma comment(lib,"xinput.lib")
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

//						X			Y	  TO	 X		   Y   
//floor coord : UP: -6.169929 , 5.946938 --->  7.120153 ,  5.576833	(Stanga-Dreapta)
//			  DOWN:	-7.136094 ,-4.479735 --->  7.145041 , -4.479735			
//			  Left: -7.146216 , 5.932014 ---> -7.146216 , -3.176147	(Sus-Jos)
//			 Right:  7.142135 , 5.906685 --->  7.142135 , -3.180861


using namespace DirectX;


//Vertex struct
struct SimpleVertex
{
	//-position of the vertex
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};


//constant buffer struct
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

};

//Controller State
struct CONTROLLER_STATE
{
	//-default constructor
	
	enum EventType
	{
		Press,
		Released,
		Invalid
	};
	XINPUT_STATE state;
	bool bConnected;
	bool isPressed()const
	{
		return type == Press;
	}
	bool isReleased()
	{
		return type == Released;
	}

private:
	EventType type;
};

//Timer Class
class Timer
{
public:

	bool isrunning = false;

	//start and stop variables
#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif

	//default constructor
	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
		stop = std::chrono::high_resolution_clock::now();
	}

	//function to get the miliseconds
	double GetMilisecondsElapsed()
	{
		if (isrunning)
		{
			auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
			return elapsed.count();
		}
		else
		{
			auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
			return elapsed.count();
		}
	}

	void Restart()
	{
		isrunning = true;
		start = std::chrono::high_resolution_clock::now();
	}

	bool Stop()
	{
		if (!isrunning)
			return false;
		else
		{
			stop = std::chrono::high_resolution_clock::now();
			isrunning = false;
			return true;
		}
	}

	bool Start()
	{
		if (isrunning)
		{
			return false;
		}
		else
		{
			start = std::chrono::high_resolution_clock::now();
			isrunning = true;
			return true;
		}
	}
};
Timer timer;
Timer fpsTimer;

//game object class
class Square
{
public:
	float decayRate = 65.0f;		//the friction rate

	//Collision edges functions
	bool isCollidingEdgeL()
	{

		bool leftCol = false;

		//get the dimensions of the player
		float myMinX = -0.5f * width + x;
		float myMaxX = 0.5 * width + x;
		float myMinY = -0.5 * height + y;
		float myMaxY = 0.5 * height + y;

		//wall coordinates
		float leftX = -6.646216f;
		float upY = 5.976833f;
		float rightX = 6.642135f;
		float downY = -4.479734f;

		if (myMinX < leftX)
		{
			leftCol = true;
		}
		return leftCol;
	}
	bool isCollidingEdgeR()
	{
		bool rightCol = false;

		//similar as above
		float myMinX = -0.5 * width + x;
		float myMaxX = 0.5 * width + x;
		float myMinY = -0.5 * height + y;
		float myMaxY = 0.5 * height + y;

		float rightX = 6.642135f;

		if (myMaxX > rightX)
		{
			rightCol = true;
		}
		return rightCol;
	}
	bool isCollidingEdgeD()
	{
		bool downCol = false;
		
		//analog as above
		float myMinX = -0.5 * width + x;
		float myMaxX = 0.5 * width + x;
		float myMinY = -0.5 * height + y;
		float myMaxY = 0.5 * height + y;

		
		float downY = -4.479734f;

		if (myMinY < downY)
		{
			downCol = true;
		}
		return downCol;
	}
	bool isCollidingEdgeU()
	{
		bool upCol = false;

		//analog
		float myMinX = -0.5 * 1 + x;
		float myMaxX = 0.5 * 1 + x;
		float myMinY = -0.5 * 1 + y;
		float myMaxY = 0.5 * 1 + y;

		float upY = 5.976833f;
		if (myMaxY > upY)
		{
			upCol = true;
		}
		return upCol;

	}

	bool IsColliding(Square & other)
	{
		bool xCollision = false;
		bool yCollision = false;

		//get both squares dimensions
		float myMinX = -0.5 * width + x;
		float myMaxX = 0.5 * width + x;
		float myMinY = -0.5 * height + y;
		float myMaxY = 0.5 * height + y;

		float otherMinX = -0.5 * other.GetWidth() + other.GetX();
		float otherMaxX = 0.5 * other.GetWidth() + other.GetX();
		float otherMinY = -0.5 * other.GetHeight() + other.GetY();
		float otherMaxY = 0.5 * other.GetHeight() + other.GetY();

		//Check the collision with extremities
		if (myMaxX >= otherMinX && myMinX <= otherMaxX)
		{
			xCollision = true;
		}
		if (myMaxY >= otherMinY && myMinY <= otherMaxY)
		{
			yCollision = true;
		}

		if (xCollision == true && yCollision == true)
			return true;
		else
			return false;
	}
	bool IsCollidingObs(Square & other)
	{
		bool xCollision = false;
		bool yCollision = false;

		//get both squares dimensions
		float myMinX = -0.5*width+ x;
		float myMaxX = 0.5*width+ x;
		float myMinY = -0.5*height+ y;
		float myMaxY = 0.5*height+ y;


		float otherMinX = -0.5 * other.GetWidth() + other.GetX();
		float otherMaxX = 0.5 * other.GetWidth() + other.GetX();
		float otherMinY = -0.5 * other.GetHeight() + other.GetY();
		float otherMaxY = 0.5 * other.GetHeight() + other.GetY();

		//Check the collision with extremities
		if (myMaxX >= otherMinX && myMinX <= otherMaxX)
		{
			xCollision = true;
		}
		if (myMaxY >= otherMinY && myMinY <= otherMaxY)
		{
			yCollision = true;
		}

		if (xCollision == true && yCollision == true)
			return true;
		else
			return false;
		powerUpMat = XMMatrixScaling(Scale.x, Scale.y, Scale.y)* XMMatrixRotationZ(x);
	}
	void UpdatePowerUpMat(float dt, Square &other, Square &other2)
	{
		bool xCollision = false;
		bool yCollision = false;
		static float t = 0.0f;
		
			static ULONGLONG timeStart = 0;
			ULONGLONG timeCur = GetTickCount64();
			if (timeStart == 0)
				timeStart = timeCur;
			t = (timeCur - timeStart) / 1000.0f;
			
			Rot.x =- t*dt;
			x += Rot.x;

			XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(Rot.x, 0.f, 0.0f, 0.0f));
			x+=XMVectorGetX(tempVect)*dt;
			if (IsCollidingObs(other))
			{
				OutputDebugStringA("merge\n)");
			}
			powerUpMat = XMMatrixScaling(Scale.x, Scale.y, Scale.y)* XMMatrixRotationZ(x)* XMMatrixTranslation(0, 0, 0);
	
			
	}
	//Set position as floats
	void SetPosition(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	//Set position as a vector 
	void SetPoisitionVect(XMVECTOR pos)
	{
		this->posVect += pos;
	}
	//Set Dimensions as floats
	void SetDimensions(float width, float height)
	{
		this->width = width;
		this->height = height;

	}
	//Set the movement speed
	void SetVelocity(float vel)
	{
		this->velocity = vel;
	}
	//take care of the physics of the bullet 
	void UpdateMatrix(float dt)
	{
		x += Dir.x*dt;
		y += Dir.y*dt;
		bool upCol = false;
		bool leftCol = false;
		bool rightCol = false;
		bool downCol = false;
		//square coord
		float myMinX = -0.5 * 1 + x;
		float myMaxX = 0.5 * 1 + x;
		float myMinY = -0.5 * 1 + y;
		float myMaxY = 0.5 * 1 + y;

		//wall coord
		float leftX = -6.646216f;
		float upY = 5.976833f;
		float rightX = 6.642135f;
		float downY = -4.479734f;

		//collision with the up wall
		if (myMaxY > upY)
		{
			Dir.y -= 1.0f;
			y -= (Dir.y*dt) / 2;
		}
		//collision with the down wall
		if (myMinY < downY)
		{
			Dir.y += 1.0f;
			y += (Dir.y*dt) / 2;
		}
		//Collision with the left wall
		if (myMinX < leftX)
		{
			Dir.x += 1.0f;
			x += (Dir.x*dt) / 2;
		}
		//Collision with the right wall
		if (myMaxX > rightX)
		{
			Dir.x -= 1.0f;
			x -= (Dir.y*dt) / 2;
		}

		//Applying friction

		//Dir.x /= 1.01f;
		//Dir.y /= 1.01f;
		if (Dir.x > 0.0f)
			Dir.x -= decayRate*dt;
		else if (Dir.x < 0.0f)
			Dir.x += decayRate*dt;

		if (Dir.y > 0.0f)
			Dir.y -= decayRate*dt;
		else if (Dir.y < 0.0f)
			Dir.y += decayRate*dt;

		playerMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.y)*XMMatrixTranslation(x, y, 0);
	}
	
	
	float GetX() { return x; }					//get the x pos
	float GetY() { return y; }					//get the y pos
	float GetWidth() { return width; }			//get the width
	float GetHeight() { return height; }		//get the height
	float GetVelocity() { return velocity; }	//get the speed
	XMVECTOR GetPosVect() { return posVect; }	//get the pos vect
	ConstantBuffer squareBuffer;
	XMFLOAT2 Scale;
	XMFLOAT2 Force;
	XMFLOAT2 Acc;
	XMFLOAT2 Dir;
	XMFLOAT2 Rot;
	XMMATRIX playerMatrix;
	XMMATRIX powerUpMat;
	float mass;

private:
	float x = 0.0;
	float y = 0.0;
	float width = 1.0;
	float height = 1.0;
	float velocity = 0.0f;
	XMVECTOR posVect = XMVectorSet(0, 0, 0, 0);
};

float vel = 40.0f;	//power of the shot

//converter to wide string
class StringConverter
{
public:
	std::wstring StringConverter::StringToWide(std::string str)
	{
		std::wstring wide_string(str.begin(), str.end());
		return wide_string;
	}
};
//-Global Variables

//-for input
KeyboardClass keyboard;



//-For the window
LPCTSTR WndClassName = L"FirstWindow";	//-window class for the window
HWND hwnd = nullptr;					//-the handle of the window
HINSTANCE m_hInstance = nullptr;
int width = 800, height = 600;			//-width and height of the window
D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;

bool end1, end2, end3, end4 = false;	//to check if the game is over

//-For crating device and swapchain
IDXGISwapChain* SwapChain = nullptr;
ID3D11Device* deviceObj = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

//-for input layout, vertex buffer, index buffer
ID3D11InputLayout* p_vertexLayout = nullptr;
ID3D11Buffer* p_vertexBuffer1 = nullptr;	//player 1
ID3D11Buffer* p_vertexBuffer2 = nullptr;	//player 2
ID3D11Buffer* p_vertexBuffer3 = nullptr;	//ball
ID3D11Buffer* p_vertexBuffer4 = nullptr;	//power ups (yet to come)
ID3D11PixelShader* p_pixelShader = nullptr;
ID3D11PixelShader* p_PixelShaderSolid = nullptr;
ID3D11VertexShader* p_vertexShader = nullptr;
ID3D11Buffer* p_IndexBuffer = nullptr;		//player 1 index buffer
ID3D11Buffer* p_IndexBuffer2 = nullptr;		//player 2 index buffer
ID3D11Buffer* p_IndexBuffer3 = nullptr;		//ball index buffer
ID3D11Buffer* p_IndexBuffer4 = nullptr;		//obstacle index buffer
ID3D11Buffer* p_ConstantBuffer = nullptr;
ID3D11Texture2D* m_depthStencil = nullptr;
ID3D11DepthStencilView* m_depthStencilView = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;
ID3D11SamplerState* samplerState = nullptr;
ID3D11ShaderResourceView* myTexture1 = nullptr;
ID3D11ShaderResourceView* myTexture2 = nullptr;
ID3D11ShaderResourceView* myTexture3 = nullptr;

//Controller
CONTROLLER_STATE g_Controllers[MAX_CONTROLLERS];
bool    g_bDeadZoneOn = true;


//-Fonts
std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
std::unique_ptr<DirectX::SpriteFont> spriteFont;


//-matrices
XMMATRIX m_World2;
XMMATRIX squareMatrix;
XMMATRIX m_View;
XMMATRIX m_Projection;
XMMATRIX projMatrix;
const int squareNr = 4;
const int powerUpNr = 2;
XMMATRIX fillSquareMat[squareNr];
XMMATRIX powerUpMat[powerUpNr];

Square CornerSquares[squareNr];
Square PowerUps[powerUpNr];
Square s1, s2, s3;


//Functions Declaration:
bool InitializeWindow(HINSTANCE hInstance, int nCmdShow);
//-handle of the app  //-how the window is displayed	  //-returns F if is full, T if it's minimized

//Devices Initialization
HRESULT CreateDevAndSwap();

//Devices Cleaner
void CleaneupDevice();

//message loop
int messageloop();

void DrawCube(CXMMATRIX mWorld, FXMVECTOR color);



//-windows callback procedure functions to capture messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Render function
void Render();

//function to process the keyboard
void Controller();
void update();
void shoot();
HRESULT UpdateControllerState();
//-main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	timer.Start();
	if (FAILED(InitializeWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(CreateDevAndSwap()))
	{
		CleaneupDevice();
		return 0;
	}


	// Init state
	ZeroMemory(g_Controllers, sizeof(CONTROLLER_STATE) * MAX_CONTROLLERS);

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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW - 2);					//-color of the window 
	wc.lpszClassName = WndClassName;								//-name of the window class 
	wc.lpszMenuName = NULL;											//-name of the menu attached to the window. We don't have one yet.
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);					//-icon of the taskbar

																	//-register the class. If it fails, get an error, if not, continue to creating the window...
	if (!RegisterClassEx(&wc))
	{
		MessageBox(0, L"Window Class could not be registered...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//-creating the window
	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"DirectX11 - Tutorial",
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
		MessageBox(NULL, L"The window couldn't be created...", L"Error", MB_OK | MB_ICONERROR);
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
	fpsTimer.Start();

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
		MessageBox(0, L"The SwapChain initialization failed...", L"Error", MB_OK);
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
		MessageBox(0, L"The BackBuffer initialization failed...", L"Error", MB_OK);
		return hr;
	}

	//-Create Render Target View..
	hr = deviceObj->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);

	//-release the backbuffer
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(0, L"The Render Target View initialization failed ", L"Error", MB_OK);
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
		MessageBox(0, L"Depth Stencil failed to create...", L"ERROR", MB_OK);
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
		MessageBox(0, L"Depth Stencil View failed to create...", L"ERROR", MB_OK);
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

	//create rasterizer desc
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = deviceObj->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	if (FAILED(hr))
	{
		MessageBox(0, L"Rasterizer State initialization failed...", L"Error", MB_OK);
		return hr;
	}

	//Initialize Sampler State for Texture
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = deviceObj->CreateSamplerState(&sampDesc, &samplerState); //Create sampler state
	if (FAILED(hr))
	{
		MessageBox(0, L"Sampler State initialization failed...", L"Error", MB_OK);
		return hr;
	}

	//-Initialize the fonts
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(immediateContext);
	spriteFont = std::make_unique<DirectX::SpriteFont>(deviceObj, L"Data\\Fonts\\comic_sans_ms_16.spritefont");


	//-compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"Effect.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex shader initialization failed... ", L"Error", MB_OK);
		return hr;
	}

	//-create the vertex shader
	hr = deviceObj->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &p_vertexShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex shader creation failed... ", L"Error", MB_OK);
		pVSBlob->Release();
		return hr;
	}
	//-define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	//-create the input layout
	hr = deviceObj->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &p_vertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
	{
		MessageBox(0, L"The Input Layout initialization failed ", L"Error", MB_OK);
		return hr;
	}

	//-compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"Effect.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(0, L"The Pixel Shader initialization failed ", L"Error", MB_OK);
		return hr;
	}

	//-create the pixel shader
	hr = deviceObj->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &p_pixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
	{
		MessageBox(0, L"The Pixel Shader creation failed ", L"Error", MB_OK);
		return hr;
	}

	//-create vertex buffer(Define buffer desc and subbresource data)
	SimpleVertex vertices[] =
	{
		//cube vertices
		/*
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		*/

		//square vertices
		{ XMFLOAT3(-0.5f, 0.0f, 0.f),XMFLOAT2(0.0f,1.0f) },	//down left
		{ XMFLOAT3(-0.5f, 0.5f, 0.f),XMFLOAT2(0.0f,0.0f) }, //top left
		{ XMFLOAT3(0.5f, 0.0f, 0.f),XMFLOAT2(1.0f,1.0f) },	//down right 
		{ XMFLOAT3(0.5f, 0.5f, 0.f),XMFLOAT2(1.0f, 0.0f) },	//top right

	};

	//second player
	SimpleVertex vertices2[] =
	{
		//square 2 vertices
		{ XMFLOAT3(-0.5f, 0.0f, 0.f),XMFLOAT2(0.0f,1.0f) },	//down left
		{ XMFLOAT3(-0.5f, 0.5f, 0.f),XMFLOAT2(0.0f,0.0f) }, //top left
		{ XMFLOAT3(0.5f, 0.0f, 0.f),XMFLOAT2(1.0f,1.0f) },	//down right 
		{ XMFLOAT3(0.5f, 0.5f, 0.f),XMFLOAT2(1.0f, 0.0f) },	//top right

	};

	//Ball
	SimpleVertex vertices3[] =
	{
		//square 2 vertices
		{ XMFLOAT3(-0.5f, 0.0f, 0.f),XMFLOAT2(0.0f,1.0f) },	//down left
		{ XMFLOAT3(-0.5f, 0.5f, 0.f),XMFLOAT2(0.0f,0.0f) }, //top left
		{ XMFLOAT3(0.5f, 0.0f, 0.f),XMFLOAT2(1.0f,1.0f) },	//down right 
		{ XMFLOAT3(0.5f, 0.5f, 0.f),XMFLOAT2(1.0f, 0.0f) },	//top right
	};

	

	//vertex buffer for player1
	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = deviceObj->CreateBuffer(&bd, &InitData, &p_vertexBuffer1);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex Buffer1 initialization failed... ", L"Error", MB_OK);
		return hr;
	}

	hr = CreateWICTextureFromFile(deviceObj, L"player1.png", nullptr, &myTexture1);
	if (FAILED(hr))
	{
		MessageBox(0, L"Texture initialization failed... ", L"Error", MB_OK);
		return hr;
	}

	//vertex buffer for player 2
	D3D11_BUFFER_DESC bd2 = {};
	ZeroMemory(&bd2, sizeof(bd2));
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(SimpleVertex) * 4;
	bd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd2.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData2 = {};
	InitData2.pSysMem = vertices2;
	hr = deviceObj->CreateBuffer(&bd2, &InitData2, &p_vertexBuffer2);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex Buffer2 initialization failed... ", L"Error", MB_OK);
		return hr;
	}
	hr = CreateWICTextureFromFile(deviceObj, L"player2.png", nullptr, &myTexture2);
	if (FAILED(hr))
	{
		MessageBox(0, L"Texture2 initialization failed... ", L"Error", MB_OK);
		return hr;
	}
	//Vertex buffer for the ball
	D3D11_BUFFER_DESC bd3 = {};
	ZeroMemory(&bd3, sizeof(bd3));
	bd3.Usage = D3D11_USAGE_DEFAULT;
	bd3.ByteWidth = sizeof(SimpleVertex) * 4;
	bd3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd3.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData3 = {};
	InitData3.pSysMem = vertices3;
	hr = deviceObj->CreateBuffer(&bd3, &InitData3, &p_vertexBuffer3);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex Buffer 3 initialization failed... ", L"Error", MB_OK);
		return hr;
	}
	hr = CreateWICTextureFromFile(deviceObj, L"ball.png", nullptr, &myTexture3);
	if (FAILED(hr))
	{
		MessageBox(0, L"Texture3 initialization failed... ", L"Error", MB_OK);
		return hr;
	}

	//Power-Ups Obstacle Vertex buffer
	D3D11_BUFFER_DESC bd4 = {};
	ZeroMemory(&bd4, sizeof(bd4));
	bd4.Usage = D3D11_USAGE_DEFAULT;
	bd4.ByteWidth = sizeof(SimpleVertex) * 4;
	bd4.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd4.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData4 = {};
	InitData4.pSysMem = vertices3;
	hr = deviceObj->CreateBuffer(&bd4, &InitData4, &p_vertexBuffer3);
	if (FAILED(hr))
	{
		MessageBox(0, L"Vertex Buffer Power Ups initialization failed... ", L"Error", MB_OK);
		return hr;
	}
	hr = CreateWICTextureFromFile(deviceObj, L"ball.png", nullptr, &myTexture3);
	if (FAILED(hr))
	{
		MessageBox(0, L"Texture4 initialization failed... ", L"Error", MB_OK);
		return hr;
	}
	//-Create Index Buffer
	WORD indices[] =
	{
		0,1,2,
		3,2,1
		/*
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
		*/
	};
	//Index Buffer for player 1
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list (for cube)
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	InitData.pSysMem = indices;
	hr = deviceObj->CreateBuffer(&bd, &InitData, &p_IndexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"Index buffer creation failed... ", L"Error", MB_OK);
		return hr;
	}


	//Index Buffer for player 2
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list (for cube)
	bd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd2.CPUAccessFlags = 0;

	InitData2.pSysMem = indices;
	hr = deviceObj->CreateBuffer(&bd2, &InitData2, &p_IndexBuffer2);
	if (FAILED(hr))
	{
		MessageBox(0, L"Index buffer2 creation failed... ", L"Error", MB_OK);
		return hr;
	}

	//Index Buffer for the ball
	bd3.Usage = D3D11_USAGE_DEFAULT;
	bd3.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
	bd3.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd3.CPUAccessFlags = 0;

	InitData3.pSysMem = indices;
	hr = deviceObj->CreateBuffer(&bd3, &InitData3, &p_IndexBuffer3);
	if (FAILED(hr))
	{
		MessageBox(0, L"Index buffer 3 creation failed... ", L"Error", MB_OK);
		return hr;
	}

	//Index Buffer for the power ups
	bd4.Usage = D3D11_USAGE_DEFAULT;
	bd4.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
	bd4.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd4.CPUAccessFlags = 0;

	InitData4.pSysMem = indices;
	hr = deviceObj->CreateBuffer(&bd4, &InitData4, &p_IndexBuffer4);
	if (FAILED(hr))
	{
		MessageBox(0, L"Index buffer 4 creation failed... ", L"Error", MB_OK);
		return hr;
	}

	//-Create Constant Buffer1
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = deviceObj->CreateBuffer(&bd, nullptr, &p_ConstantBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"Constant buffer creation failed... ", L"Error", MB_OK);
		return hr;
	}

	//player 1 speed
	s1.SetVelocity(40.0f);
	s2.SetVelocity(40.0f);
	//Initialize the ball

	s3.Dir = XMFLOAT2(0, 0);
	s3.Scale = XMFLOAT2(s3.GetWidth() / 4, s3.GetHeight() / 4);

	//Obstacle scale
	//PowerUps[0].SetDimensions(PowerUps[0].GetWidth() * 2, PowerUps[0].GetHeight() * 2);
	//PowerUps[0].Rot = XMFLOAT2(XM_1DIV2PI, 0);
	PowerUps[0].Scale = XMFLOAT2(PowerUps[0].GetWidth()*2, PowerUps[0].GetHeight());
	PowerUps[0].SetDimensions(PowerUps[0].Scale.x, PowerUps[0].Scale.y);

	//-Initialize the matrices
	m_World2 = XMMatrixTranslation(3, 0, 0);		//player 2 matrix
	squareMatrix = XMMatrixTranslation(-3, 0, 0);	//player 1 matrix
	s3.playerMatrix = XMMatrixScaling(s3.Scale.x, s3.Scale.y, 0);
	
	srand(time(NULL));
	int pos = rand() % 2+1;
	int pos2 = rand() % 2 + 1;


	PowerUps[0].powerUpMat = XMMatrixScaling(PowerUps[0].Scale.x, PowerUps[0].Scale.y, 0)* XMMatrixTranslation(pos, pos, 0);
	//PowerUps[1].powerUpMat = XMMatrixScaling(.5f, .5f, 0)*XMMatrixTranslation(pos2, pos2, 0);

	//Initialize the the hpme matrix and the power ups
	for (int i = 0; i < squareNr; i++)
	{
		//Set the scale and the position of the Corner Squares to the default value 
		CornerSquares[i].Scale = XMFLOAT2(3.0f, 3.0f);
		if (i == 0)
		{

			fillSquareMat[i] = XMMatrixScaling(3.f, 3.f, 0) * XMMatrixTranslation(-5.2, -4, 0); //down left
		}
		else if (i == 1)
		{
			fillSquareMat[i] = XMMatrixScaling(3.f, 3.f, 0) * XMMatrixTranslation(-5.2, 4.5, 0); //up left
		}
		else if (i == 2)
		{
			fillSquareMat[i] = XMMatrixScaling(3.f, 3.f, 0) * XMMatrixTranslation(5.2, -4, 0); //down right
		}
		else if (i == 3)
		{
			fillSquareMat[i] = XMMatrixScaling(3.f, 3.f, 0)* XMMatrixTranslation(5.2, 4.5, 0);	//up right
		}

	}

	//-Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	//-Initialize the projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,			// Field of view (pi / 2 radians, or 90 degrees
		width / (FLOAT)height, // Aspect ratio.
		0.01f,					// Near clipping plane.
		100.0f);	// Far clipping plane.

	//Setup the ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(deviceObj, immediateContext);
	ImGui::StyleColorsDark();


	return S_OK;
}
int messageloop()
{
	//-make an instance of the msg structure
	MSG msg = { 0 };

	//-clean the msg structure
	ZeroMemory(&msg, sizeof(MSG));

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (end1&&end2||end3&&end4)
			{
				//Game Over!
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			update();
		}
		else
		{
			//CALLED EVERY FRAME
			Render();
			UpdateControllerState();
			update();
		}
	}
	return (int)msg.wParam;
}

//ImGui External Function
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;
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
	/*
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
	*/

	//
	// Clear the back buffer
	//
	immediateContext->ClearRenderTargetView(renderTargetView, Colors::MidnightBlue);
	immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//-set the shaders
	immediateContext->PSSetShader(p_pixelShader, NULL, 0);
	immediateContext->VSSetShader(p_vertexShader, NULL, 0);

	//-Set vertex buffer stride and offset
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	//set texture for the first square
	immediateContext->PSSetShaderResources(0, 1, &myTexture1);

	//Set vertex buffer for player1
	immediateContext->IASetVertexBuffers(0, 1, &p_vertexBuffer1, &stride, &offset);
	//set index buffer
	immediateContext->IASetIndexBuffer(p_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//set the input layout
	immediateContext->IASetInputLayout(p_vertexLayout);

	//set primitive topology
	immediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//set the rasterizer state
	immediateContext->RSSetState(rasterizerState);
	//Set the sampler state
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	//
	// Clear the depth buffer to 1.0 (max depth)
	//


	immediateContext->VSSetConstantBuffers(0, 1, &p_ConstantBuffer);

	float dt = timer.GetMilisecondsElapsed()*0.0001f;	//Delta Time


	//set the player 1 buffer
	s1.squareBuffer.mWorld = XMMatrixTranspose(squareMatrix);
	s1.squareBuffer.mView = XMMatrixTranspose(m_View);
	s1.squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
	immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &s1.squareBuffer, 0, 0);
	immediateContext->DrawIndexed(6, 0, 0);

	//Player 1 home
	for (int i = 0; i < squareNr - 2; i++)
	{
						//Future PowerUps/Obstacles
		
			PowerUps[i].squareBuffer.mWorld = XMMatrixTranspose(PowerUps[i].powerUpMat);
			PowerUps[i].squareBuffer.mView = XMMatrixTranspose(m_View);
			PowerUps[i].squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
			
			immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &PowerUps[i].squareBuffer, 0, 0);
			immediateContext->DrawIndexed(6, 0, 0);
		
		//Set the Corner Squares squares buffer variables for player 1 and draw
		CornerSquares[i].squareBuffer.mWorld = XMMatrixTranspose(fillSquareMat[i]);
		CornerSquares[i].squareBuffer.mView = XMMatrixTranspose(m_View);
		CornerSquares[i].squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
		immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &CornerSquares[i].squareBuffer, 0, 0);
		immediateContext->DrawIndexed(6, 0, 0);

	}


	//-player 2 vertex Buffer:
	immediateContext->PSSetShaderResources(0, 1, &myTexture2);
	immediateContext->IASetVertexBuffers(0, 1, &p_vertexBuffer2, &stride, &offset);

	//-set index buffer 2
	immediateContext->IASetIndexBuffer(p_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);

	//set Player 2 home
	for (int i = 2; i < squareNr; i++)
	{
		CornerSquares[i].squareBuffer.mWorld = XMMatrixTranspose(fillSquareMat[i]);
		CornerSquares[i].squareBuffer.mView = XMMatrixTranspose(m_View);
		CornerSquares[i].squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
		immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &CornerSquares[i].squareBuffer, 0, 0);
		immediateContext->DrawIndexed(6, 0, 0);

	}

	//set the player 2 buffer variables and Draw
	s2.squareBuffer.mWorld = XMMatrixTranspose(m_World2);
	s2.squareBuffer.mView = XMMatrixTranspose(m_View);
	s2.squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
	immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &s2.squareBuffer, 0, 0);
	immediateContext->DrawIndexed(6, 0, 0);
	
	
	//Set The ball Texture
	immediateContext->PSSetShaderResources(0, 1, &myTexture3);
	//-Ball vertex Buffer:
	immediateContext->IASetVertexBuffers(0, 1, &p_vertexBuffer3, &stride, &offset);
	//-Ball Index Buffer
	immediateContext->IASetIndexBuffer(p_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);

	//set the ball buffer

	s3.squareBuffer.mWorld = XMMatrixTranspose(s3.playerMatrix);
	s3.squareBuffer.mView = XMMatrixTranspose(m_View);
	s3.squareBuffer.mProjection = XMMatrixTranspose(m_Projection);
	immediateContext->UpdateSubresource(p_ConstantBuffer, 0, NULL, &s3.squareBuffer, 0, 0);
	immediateContext->DrawIndexed(6, 0, 0);

	//power ups buffer
	immediateContext->PSSetShaderResources(0, 1, &myTexture3);
	immediateContext->IASetVertexBuffers(0, 1, &p_vertexBuffer3, &stride, &offset);
	immediateContext->IASetIndexBuffer(p_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);

	
	//FpsCounter:

	//-Draw Text
	static int fpsCounter = 0;
	StringConverter str;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)	//if the time is greater than 1 second
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	//Draw Sprite
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(fpsString).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-200.0f, 0.f), XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	//ImGui Drawing
	static std::string outmsg2;


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Player Settings");

	//Player 1 settings

	//Increase Speed
	if (ImGui::Button("Increase Player1 Speed"))
	{
		s1.SetVelocity(s1.GetVelocity() + .5f);

		outmsg2 = "Velocity of player1 is: " + std::to_string(s1.GetVelocity());
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());

	}
	//Draw Sprite
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();

	//Decrease speed
	if (ImGui::Button("Decrease Player1 Speed"))
	{
		s1.SetVelocity(s1.GetVelocity() - .5f);

		outmsg2 = "Velocity of player1 is: " + std::to_string(s1.GetVelocity());
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());

	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();

	//Reset player 1 speed to default value
	if (ImGui::Button("Reset Player1 Speed"))
	{
		s1.SetVelocity(40.0f);
		outmsg2 = "Velocity of player1 is: " + std::to_string(s1.GetVelocity());
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();

	ImGui::NewLine();

	//player 2 settings

	//Increase Speed
	if (ImGui::Button("Increase Player2 Speed"))
	{
		s2.SetVelocity(s2.GetVelocity() + .5f);

		outmsg2 = "Velocity of player2 is: " + std::to_string(s2.GetVelocity());
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());

	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();

	// Decrease speed
		if (ImGui::Button("Decrease Player2 Speed"))
		{
			s2.SetVelocity(s2.GetVelocity() - .5f);

			outmsg2 = "Velocity of player2 is: " + std::to_string(s2.GetVelocity());
			outmsg2 += "\n";
			OutputDebugStringA(outmsg2.c_str());

		}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();
	
	//Reset player 2 speed to default value
	if (ImGui::Button("Reset Player2 Speed"))
	{
		s2.SetVelocity(40.0f);
		outmsg2 = "Velocity of player2 is: " + std::to_string(s2.GetVelocity());
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), str.StringToWide(outmsg2).c_str(), XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(-500.0f, 0.f), XMFLOAT2(0.7f, 0.7f));
	spriteBatch->End();

	ImGui::End();
	
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	

	// Present our back buffer to our front buffer
	//
	SwapChain->Present(0, 0);		//for v syng use (1,0)

}
//Function that cleans the variables
void CleaneupDevice()
{
	//-clean all the devices created
	if (immediateContext) immediateContext->ClearState();
	if (m_depthStencil) m_depthStencil->Release();
	if (m_depthStencilView) m_depthStencilView->Release();
	if (p_ConstantBuffer) p_ConstantBuffer->Release();
	if (p_IndexBuffer) p_IndexBuffer->Release();
	if (p_vertexBuffer1) p_vertexBuffer1->Release();
	if (p_IndexBuffer2) p_IndexBuffer2->Release();
	if (p_vertexBuffer2) p_vertexBuffer2->Release();
	if (p_IndexBuffer3) p_IndexBuffer3->Release();
	if (p_vertexBuffer3) p_vertexBuffer3->Release();
	if (p_vertexLayout) p_vertexLayout->Release();
	if (p_vertexShader) p_vertexShader->Release();
	if (p_pixelShader) p_pixelShader->Release();
	if (p_PixelShaderSolid) p_PixelShaderSolid->Release();
	if (renderTargetView) renderTargetView->Release();
	if (rasterizerState)rasterizerState->Release();
	if (samplerState)samplerState->Release();
	if (myTexture1)myTexture1->Release();
	if (myTexture2)myTexture2->Release();
	if (myTexture3)myTexture3->Release();
	if (SwapChain) SwapChain->Release();
	if (immediateContext) immediateContext->Release();
	if (deviceObj) deviceObj->Release();
}

//Player 1 shooting variables
bool ChargingShot = false;
bool FireShot = false;

//Player 2 shooting variables
bool ChargingShot2 = false;
bool FireShot2 = false;

void Obstactle()
{
	float dt = timer.GetMilisecondsElapsed() * 0.0001f;		//delta time
	static float t = 0.0f;

	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;

	PowerUps[0].Rot.x = t*dt;

}

//called every frame
void update()
{
	float dt = timer.GetMilisecondsElapsed() * 0.0001f;		//delta time
	timer.Restart();
	shoot();

	//Controller Input HANDLING THE DEADZONE 
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (g_Controllers[i].bConnected)
		{
			WORD wButtons = g_Controllers[i].state.Gamepad.wButtons;

			if (g_bDeadZoneOn)
			{
				// Zero value if thumbsticks are within the dead zone 
				if ((g_Controllers[i].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
					g_Controllers[i].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
					(g_Controllers[i].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
						g_Controllers[i].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
				{
					g_Controllers[i].state.Gamepad.sThumbLX = 0;
					g_Controllers[i].state.Gamepad.sThumbLY = 0;
				}

				if ((g_Controllers[i].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
					g_Controllers[i].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
					(g_Controllers[i].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
						g_Controllers[i].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
				{
					g_Controllers[i].state.Gamepad.sThumbRX = 0;
					g_Controllers[i].state.Gamepad.sThumbRY = 0;
				}
			}
		}
	}
	
	//If the player holds the shooting button, the velocity of the bullet is going to be increased by a const value every frame

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
		std::string outmsg2 = "Key: ";
		outmsg2 += keycode;
		outmsg2 += "\n";
		OutputDebugStringA(outmsg2.c_str());
		/*
		if (kbe.isPressed() && kbe.GetKeyCode() == ' ')
		{
		outmsg = "Keycode Pressed: ";
		outmsg += keycode;
		outmsg += "\n";
		ChargingShot = true;

		}
		else if (kbe.isReleased() && kbe.GetKeyCode() == ' ')
		{
		outmsg = "Keycode Released: ";
		outmsg += keycode;
		outmsg += "\n";
		FireShot = true;
		}
		*/

	}
	//Player 1 movement using w a s d
	XMVECTOR tempPos = XMVectorSet(0, 0.0f, 0, 0);
	bool canMove = true;
	tempPos = XMVector2TransformCoord(tempPos, squareMatrix);
	s1.SetPosition(XMVectorGetX(tempPos), XMVectorGetY(tempPos));
	
	if (canMove)
	{
		if (g_Controllers[0].state.Gamepad.sThumbLY >0 || keyboard.KeyIsPressed('W'))
		{
			s1.SetPosition(s1.GetX(), s1.GetY() + s1.GetVelocity()*dt);
			//std::string outmsg = "Position on the X: "+ std::to_string(s1.GetX())+ ", Position on the Y is: "+ std::to_string(s1.GetY()) + "\n";
			//OutputDebugStringA(outmsg.c_str());

		}
		else if (g_Controllers[0].state.Gamepad.sThumbLY <0 || keyboard.KeyIsPressed('S'))
		{
			s1.SetPosition(s1.GetX(), s1.GetY() - s1.GetVelocity()*dt);
			//std::string outmsg = "Position on the X: " + std::to_string(s1.GetX()) + ", Position on the Y is: " + std::to_string(s1.GetY()) + "\n";
			//OutputDebugStringA(outmsg.c_str());
		}
		 if (g_Controllers[0].state.Gamepad.sThumbLX <0 || keyboard.KeyIsPressed('A'))
		{
			s1.SetPosition(s1.GetX() - s1.GetVelocity()*dt, s1.GetY());
			//std::string outmsg = "Position on the X: " + std::to_string(s1.GetX()) + ", Position on the Y is: " + std::to_string(s1.GetY()) + "\n";
			//OutputDebugStringA(outmsg.c_str());

		}
		else if( g_Controllers[0].state.Gamepad.sThumbLX>1 || keyboard.KeyIsPressed('D'))
		{
			s1.SetPosition(s1.GetX() + s1.GetVelocity()*dt, s1.GetY());
			//std::string outmsg = "Position on the X: " + std::to_string(s1.GetX()) + ", Position on the Y is: " + std::to_string(s1.GetY()) + "\n";
			//OutputDebugStringA(outmsg.c_str());
		}
	}
	//Update the player1 matrix
	XMMATRIX Translation = XMMatrixTranslation(s1.GetX(), s1.GetY(), 0.0f);
	squareMatrix = Translation;
	

	//Player 2 movement using arrows
	XMVECTOR tempPos2 = XMVectorSet(0, 0.0f, 0, 0);
	
	tempPos2 = XMVector2TransformCoord(tempPos2, m_World2);
	s2.SetPosition(XMVectorGetX(tempPos2), XMVectorGetY(tempPos2));
	if (g_Controllers[1].state.Gamepad.sThumbLY >0 || keyboard.KeyIsPressed('&'))
	{
		s2.SetPosition(s2.GetX(), s2.GetY() + s2.GetVelocity()*dt);

	}
	else if (g_Controllers[1].state.Gamepad.sThumbLY <0 || keyboard.KeyIsPressed('('))
	{
		s2.SetPosition(s2.GetX(), s2.GetY() - s2.GetVelocity()*dt);
	}
	if (g_Controllers[1].state.Gamepad.sThumbLX <0 || keyboard.KeyIsPressed('%'))
	{
		s2.SetPosition(s2.GetX() - s2.GetVelocity()*dt, s2.GetY());

	}
	else if (g_Controllers[1].state.Gamepad.sThumbLX >0 || keyboard.KeyIsPressed('\''))
	{
		s2.SetPosition(s2.GetX() + s2.GetVelocity()*dt, s2.GetY());
	}
	XMMATRIX Translation2 = XMMatrixTranslation(s2.GetX(), s2.GetY(), 0.f);
	m_World2 = Translation2;

	/*
	XMVECTOR tempPos3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR projDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	tempPos3 = XMVector2TransformCoord(tempPos3, projMatrix);
	s3.SetPosition(XMVectorGetX(tempPos3), XMVectorGetY(tempPos3));
	*/
	/*
	//s3.SetDimensions(s3.GetWidth() / 2, s3.GetHeight() / 2);
	if (keyboard.KeyIsPressed('B'))
	{
	s3.Dir.y += 50.0f*dt;
	//s3.DirVect *= projDir*dt;
	//s3.SetPosition(s3.GetX(), s3.GetY() + 5.0f *dt);

	}
	//XMMATRIX Translation3 = XMMatrixTranslation(s3.GetX(), s3.GetY(), 0.f);
	XMMATRIX Translation3 = XMMatrixTranslation(s3.Dir.x*dt+s3.GetX(), s3.Dir.y*dt+s3.GetY(), 0.f);
	//XMMATRIX Translation3 = XMMatrixTranslation(XMVectorGetX(s3.DirVect), XMVectorGetY(s3.DirVect), 0.f);
	//XMMATRIX projScailing = XMMatrixScaling(s3.Scale.x , s3.Scale.y, 0);
	projMatrix = XMMatrixScaling(s3.Scale.x, s3.Scale.y, 0)*Translation3;
	*/
	/*
	if (keyboard.KeyIsPressed('R'))
	{
	projMatrix = XMMatrixIdentity();
	fillSquareMat[0] = XMMatrixScaling(CornerSquares[0].Scale.x, CornerSquares[0].Scale.y, 0) * XMMatrixTranslation(5.2, 4.5, 0);	//up right
	}
	*/

	bool isColliding1 = false;
	bool isColliding2 = false;
	bool isColliding3 = false;
	// projectile collides the player1
	if (s1.IsColliding(s3))
	{
		isColliding2 = false;
		isColliding1 = true;
		//projMatrix = XMMatrixScaling(s3.Scale.x,s3.Scale.y, 0)*squareMatrix;
		s3.SetPosition(s1.GetX(), s1.GetY());
		s3.SetDimensions(0, 0);
		s3.Dir = XMFLOAT2(0, 0);
		//s3.Scale = XMFLOAT2(0.0f, 0.0f);
	}
	//projectile collides player2
	else if (s2.IsColliding(s3))
	{
		isColliding1 = false;
		isColliding2 = true;
		s3.SetPosition(s2.GetX(), s2.GetY());
		s3.SetDimensions(0.0f, 0.0f);
		s3.Dir = XMFLOAT2(0, 0);
		//s3.Scale = XMFLOAT2(0.0f, 0.0f);
	}

	//if the players collide
	if (s1.IsColliding(s2))
	{
		if (isColliding1)
		{
			//Give the ball to player 2
			s3.SetPosition(s2.GetX(), s2.GetY());
			/*
			canMove = false;
			timer2.Start();
			float jamTime = timer2.GetMilisecondsElapsed()*0.0001;
			if (jamTime > 5)
			{
			canMove = true;
			timer2.Restart();
			}
			*/
		}
		else if (isColliding2)
		{
			//Give the ball to player 1
			s3.SetPosition(s1.GetX(), s1.GetY());
		}
	}
	//OutputDebugStringA("Collision\n");
	//m_World2 = XMMatrixIdentity();
/*
	//Colluding with the obstacle
	if (PowerUps[0].IsCollidingObs(s1))
	{
		OutputDebugStringA("merge\n");
	}
	*/
	//shrinking the squares
	XMVECTOR tempPos4[squareNr];
	float shrinkSpeed = 2.0f;
	
	for (int i = 0; i < squareNr; i++)
	{
		tempPos4[i] = XMVectorSet(0, 0, 0, 0);
		tempPos4[i] = XMVector2TransformCoord(tempPos4[i], fillSquareMat[i]);
		CornerSquares[i].SetPosition(XMVectorGetX(tempPos4[i]), XMVectorGetY(tempPos4[i]));

		//If player 1/2 has the ball and enters in the corner squares, shrink the squares
		//If the ball is in the center of the corner squares, shrink them.
		if (isColliding1&& s1.IsColliding(CornerSquares[i])||s3.IsColliding(CornerSquares[i]))
		{
			CornerSquares[i].Scale.x -= shrinkSpeed*dt;
			CornerSquares[i].Scale.y -= shrinkSpeed*dt;


			if (i == 0)
			{

				fillSquareMat[i] = XMMatrixScaling(CornerSquares[i].Scale.x, CornerSquares[i].Scale.y, 0) * XMMatrixTranslation(-5.2, -4, 0); //down left
				if (CornerSquares[i].Scale.x <= .1f &&CornerSquares[i].Scale.y <= .1f)
				{
					CornerSquares[i].Scale.x = 0.f;
					CornerSquares[i].Scale.y = 0.f;
					end1 = true;
				}

			}
			else if (i == 1)
			{
				fillSquareMat[i] = XMMatrixScaling(CornerSquares[i].Scale.x, CornerSquares[i].Scale.y, 0) * XMMatrixTranslation(-5.2, 4.5, 0); //up left
				if (CornerSquares[i].Scale.x <= .1f &&CornerSquares[i].Scale.y <= .1f)
				{
					CornerSquares[i].Scale.x = 0;
					CornerSquares[i].Scale.y = 0;
					end2 = true;	
				}
			}
		}
		if (isColliding2 && s2.IsColliding(CornerSquares[i])||s3.IsColliding(CornerSquares[i]))
		{
			CornerSquares[i].Scale.x -= shrinkSpeed*dt;
			CornerSquares[i].Scale.y -= shrinkSpeed*dt;
			if (i == 2)
			{
				fillSquareMat[i] = XMMatrixScaling(CornerSquares[i].Scale.x, CornerSquares[i].Scale.y, 0) * XMMatrixTranslation(5.2, -4, 0); //down right
				if (CornerSquares[i].Scale.x <= .1f &&CornerSquares[i].Scale.y <= .1f)
				{
					CornerSquares[i].Scale.x = 0;
					CornerSquares[i].Scale.y = 0;
					end3 = true;	
				}
			}
			else if (i == 3)
			{
				CornerSquares[i].SetDimensions(CornerSquares[i].GetWidth(), CornerSquares[i].GetHeight());
				fillSquareMat[i] = XMMatrixScaling(CornerSquares[i].Scale.x, CornerSquares[i].Scale.y, 0)* XMMatrixTranslation(5.2, 4.5, 0);	//up right

				if (CornerSquares[i].Scale.x <= .1f &&CornerSquares[i].Scale.y <= .1f)
				{
					CornerSquares[i].Scale.x = 0;
					CornerSquares[i].Scale.y = 0;
					end4 = true;	
				
				}
			}

		}

	}

	//Player1 Edge Collision
	if (s1.isCollidingEdgeL())
	{
		OutputDebugStringA("colliding with the edge on the left...\n");
		s1.SetPosition(s1.GetX() + 0.1f, s1.GetY());
		squareMatrix = XMMatrixIdentity();
	}
	else if (s1.isCollidingEdgeR())
	{
		OutputDebugStringA("colliding with the edge on the right...\n");
		s1.SetPosition(s1.GetX() - 0.1f, s1.GetY());
		squareMatrix = XMMatrixIdentity();
	}
	else if (s1.isCollidingEdgeU())
	{
		OutputDebugStringA("colliding with the edge on the up...\n");
		s1.SetPosition(s1.GetX(), s1.GetY() - 0.1f);
		squareMatrix = XMMatrixIdentity();
	}
	else if (s1.isCollidingEdgeD())
	{
		OutputDebugStringA("colliding with the edge on the down...\n");
		s1.SetPosition(s1.GetX(), s1.GetY() + 0.1f);
		squareMatrix = XMMatrixIdentity();
	}

	//Player2 Edge Collision
	if (s2.isCollidingEdgeL())
	{
		OutputDebugStringA("colliding with the edge on the left...\n");
		s2.SetPosition(s2.GetX() + 0.1f, s2.GetY());
		m_World2 = XMMatrixIdentity();
	}
	else if (s2.isCollidingEdgeR())
	{
		OutputDebugStringA("colliding with the edge on the right...\n");
		s2.SetPosition(s2.GetX() - 0.1f, s2.GetY());
		m_World2 = XMMatrixIdentity();
	}
	else if (s2.isCollidingEdgeU())
	{
		OutputDebugStringA("colliding with the edge on the up...\n");
		s2.SetPosition(s2.GetX(), s2.GetY() - 0.1f);
		m_World2 = XMMatrixIdentity();
	}
	else if (s2.isCollidingEdgeD())
	{
		OutputDebugStringA("colliding with the edge on the down...\n");
		s2.SetPosition(s2.GetX(), s2.GetY() + 0.1f);
		m_World2 = XMMatrixIdentity();
	}
	//Update the ball matrix
	s3.UpdateMatrix(dt);
	PowerUps[0].UpdatePowerUpMat(dt,s1, s2);
}

bool isPressedX = false;
bool isReleasedX = false;
void shoot()
{

	
	float dt = timer.GetMilisecondsElapsed() * 0.0001f;
	while (!keyboard.KeyBufferIsEmpty())
	{

		//-get a key
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
		std::string outmsg;
		
	
		//Player 1 shooting checking if the space key was pressed
		if (g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_A&& g_Controllers[0].isPressed() || kbe.isPressed() && kbe.GetKeyCode() == ' ' )
		{
			outmsg = "Keycode Pressed: ";
			outmsg += keycode;
			outmsg += "\n";
			ChargingShot = true;
			//s3.Scale = XMFLOAT2(0.3f, 0.3f);

		}
		//Player 1 shooting checking if the space key was released
		else if (g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_B || kbe.isReleased() && kbe.GetKeyCode() == ' ')
		{
			outmsg = "Keycode Released: ";
			outmsg += keycode;
			outmsg += "\n";
			FireShot = true;
			//s3.Scale = XMFLOAT2(0.3f, 0.3f);
		}

		//Player 2 checking if the enter key was pressed
		if (g_Controllers[1].state.Gamepad.wButtons & XINPUT_GAMEPAD_A  || kbe.isPressed() && kbe.GetKeyCode() == 0x0D)
		{
			outmsg = "Keycode Pressed: ";
			outmsg += keycode;
			outmsg += "\n";
			ChargingShot2 = true;
			//s3.Scale = XMFLOAT2(0.3f, 0.3f);

		}
		//Player 2 checking if the enter key was released
		else if (g_Controllers[1].state.Gamepad.wButtons & XINPUT_GAMEPAD_B || kbe.isReleased() && kbe.GetKeyCode() == 0x0D)
		{
			outmsg = "Keycode Released: ";
			outmsg += keycode;
			outmsg += "\n";
			FireShot2 = true;
			//s3.Scale = XMFLOAT2(0.3f, 0.3f);
		}


		
		OutputDebugStringA(outmsg.c_str());
	}

	//player1 shooting
	if (s1.IsColliding(s3))
	{
		//Charge the velocity of the shoot
		if (ChargingShot && !FireShot)
		{
			std::string msg = "velocity is: ";
 			vel += .0325;	
			msg += std::to_string(vel);
			OutputDebugStringA(msg.c_str());
		}
		//Apply the velocity
		else if (ChargingShot && FireShot)
		{
			if (keyboard.KeyIsPressed('W'))
			{
				//Debugging messages for tests
				std::string outmsg;
				outmsg = "Firing Shot";
				outmsg += "\n";
				OutputDebugStringA(outmsg.c_str());
				
				//based of the direction input coming from the player update it's direction on the y or x axis 
				s3.Dir.y += vel;
				XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect));

				std::string outmsg2;
				outmsg2 = "Velocity on the y is: " + std::to_string(s3.Dir.y);
				outmsg2 += "\n";
				OutputDebugStringA(outmsg2.c_str());

				ChargingShot = false;
				FireShot = false;
				vel = 40.0f;
				std::string msg = "position of the bullet is: " + std::to_string(s3.GetX()) + " , " + std::to_string(s3.GetY());
				OutputDebugStringA(msg.c_str());
			}
			else if (keyboard.KeyIsPressed('S'))
			{
				s3.Dir.y -= vel;
				XMVECTOR tempVect2 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect2 *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect2));

				ChargingShot = false;
				FireShot = false;
				vel = 40.0f;
			}
			if (keyboard.KeyIsPressed('A'))
			{
				s3.Dir.x -= vel;
				XMVECTOR tempVect3 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect3 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVect3), s3.GetY());

				ChargingShot = false;
				FireShot = false;
				vel = 40.0f;
			}
			else if (keyboard.KeyIsPressed('D'))
			{
				s3.Dir.x += vel;
				XMVECTOR tempVect4 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect4 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVect4), s3.GetY());

				ChargingShot = false;
				FireShot = false;
				vel = 40.0f;
			}
		}
		//Shooting using controller
		if (g_Controllers[0].state.Gamepad.bRightTrigger)
		{
			std::string msg = "velocity is: ";
			vel += .0325;
			msg += std::to_string(vel);
			OutputDebugStringA(msg.c_str());
		}
		if (g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_A&& g_Controllers[0].isPressed())
		{
			

			if (g_Controllers[0].state.Gamepad.sThumbLY > 0)
			{
				s3.Dir.y += vel;
				XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect));
				vel = 40.f;
			}
			else if (g_Controllers[0].state.Gamepad.sThumbLY < 0)
			{
				s3.Dir.y -= vel;
				XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect));
				vel = 10.f;
			}
			if (g_Controllers[0].state.Gamepad.sThumbLX < 0)
			{
				s3.Dir.x -= vel;
				XMVECTOR tempVectC1 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVectC1 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVectC1), s3.GetY() );
				vel = 10.f;
			}
			else if (g_Controllers[0].state.Gamepad.sThumbLX > 0)
			{
				s3.Dir.x += vel;
				XMVECTOR tempVectC1 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVectC1 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVectC1), s3.GetY());
				vel = 10.f;
			}

		}
	}
	//Analog for player 2
	else if (s2.IsColliding(s3))
	{
		if (ChargingShot2 && !FireShot2)
		{
			std::string msg = "velocity is: ";
			vel += .0325f;
			msg += std::to_string(vel);
			OutputDebugStringA(msg.c_str());
		}
		else if (ChargingShot2 && FireShot2)
		{

			std::string outmsg;
			outmsg = "Firing Shot";
			outmsg += "\n";
			OutputDebugStringA(outmsg.c_str());
			if (keyboard.KeyIsPressed('&'))
			{

				s3.Dir.y += vel;
				XMVECTOR normVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.f, 0.f));
				normVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(normVect));

				ChargingShot2 = false;
				FireShot2 = false;
				vel = 40.0f;
				std::string msg = "position of the bullet is: " + std::to_string(s3.GetX()) + " , " + std::to_string(s3.GetY());
				OutputDebugStringA(msg.c_str());
			}
			else if (keyboard.KeyIsPressed('('))
			{

				s3.Dir.y -= vel;
				XMVECTOR tempVect2 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.f, 0.f));
				tempVect2 *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect2));

				ChargingShot2 = false;
				FireShot2 = false;
				vel = 40.0f;
			}
			if (keyboard.KeyIsPressed('%'))
			{
				s3.Dir.x -= vel;
				XMVECTOR tempVect3 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.f, 0.f));
				tempVect3 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVect3), s3.GetY());

				ChargingShot2 = false;
				FireShot2 = false;
				vel = 40.0f;
			}
			else if (keyboard.KeyIsPressed('\''))
			{
				s3.Dir.x += vel;
				XMVECTOR tempVect4 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.f, 0.f));
				tempVect4 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVect4), s3.GetY());

				ChargingShot2 = false;
				FireShot2 = false;
				vel = 40.0f;
			}
		}
		if (g_Controllers[1].state.Gamepad.bRightTrigger)
		{
			std::string msg = "velocity is: ";
			vel += .0325;
			msg += std::to_string(vel);
			OutputDebugStringA(msg.c_str());
		}
		if (g_Controllers[1].state.Gamepad.wButtons & XINPUT_GAMEPAD_A&& g_Controllers[0].isPressed())
		{
			if (g_Controllers[1].state.Gamepad.sThumbLY > 0)
			{
				s3.Dir.y += vel;
				XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect));
				vel = 10.f;
			}
			else if (g_Controllers[1].state.Gamepad.sThumbLY < 0)
			{
				s3.Dir.y -= vel;
				XMVECTOR tempVect = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVect *= 1.0f;
				s3.SetPosition(s3.GetX(), s3.GetY() + XMVectorGetY(tempVect));
				vel = 10.f;
			}
			if (g_Controllers[1].state.Gamepad.sThumbLX < 0)
			{
				s3.Dir.x -= vel;
				XMVECTOR tempVectC1 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVectC1 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVectC1), s3.GetY());
				vel = 10.f;
			}
			else if (g_Controllers[1].state.Gamepad.sThumbLX > 0)
			{
				s3.Dir.x += vel;
				XMVECTOR tempVectC1 = XMVector2Normalize(XMVectorSet(s3.Dir.x, s3.Dir.y, 0.0f, 0.0f));
				tempVectC1 *= 1.0f;
				s3.SetPosition(s3.GetX() + XMVectorGetX(tempVectC1), s3.GetY());
				vel = 10.f;
			}

		}
	}
	s3.UpdateMatrix(dt);
}
//Controller Update State function
HRESULT UpdateControllerState()
{
	DWORD dwResult;
	for (DWORD i = 0; i < 4; i++)
	{
		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &g_Controllers[i].state);

		if (dwResult == ERROR_SUCCESS)
			g_Controllers[i].bConnected = true;
		else
			g_Controllers[i].bConnected = false;
	}

	return S_OK;
}

