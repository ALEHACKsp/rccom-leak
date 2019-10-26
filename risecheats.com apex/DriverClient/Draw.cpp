#include "Draw.h"
D3D11_INPUT_ELEMENT_DESC Layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(Layout);
HRESULT DrawingClass::CompileShaderFromMemory(const char* szdata, SIZE_T len, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromMemory(szdata, len, NULL, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}
/*static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}*/
void DrawingClass::GenerateClassName(wchar_t* ClassName, int Length)
{
	mt19937_64 Generator(time(0));
	uniform_int_distribution<int>distribution(65, 90);
	for (int i = 0; i < Length; i++)
	{
		ClassName[i] = distribution(Generator);
	}
}
HRESULT DrawingClass::InitWindow()
{
	// Register class
   WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
	wcex.lpszMenuName = nullptr;
	GenerateClassName(ClassName, 9);
	wcex.lpszClassName = ClassName;
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	//? Create window
	g_hWnd = CreateWindowExW(WS_EX_LAYERED, ClassName, L"", WS_POPUP , 0, 0, Width,Height, 0, 0, NULL, NULL);
	if (!g_hWnd)
		return E_FAIL;
	SetLayeredWindowAttributes(g_hWnd, 0, 0.0f, LWA_ALPHA);
	SetLayeredWindowAttributes(g_hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
	ShowWindow(g_hWnd, SW_SHOW);

	return S_OK;
}
HRESULT DrawingClass::InitDevice(bool Multisampling, ID3D11Device*             g_pd3dDevice, ID3D11DeviceContext* g_pImmediateContext, IDXGISwapChain*           g_pSwapChain)
{
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Height = Height;
	scd.BufferDesc.Width = Width;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = g_hWnd;
	if (Multisampling)
	{
		scd.SampleDesc.Count = 8;
		scd.SampleDesc.Quality = 0;
	}
	else
	{
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
	}
	hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed create device and swapchain!", "Error", MB_OK);
		return hr;
	}
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);
	ID3D11Texture2D* g_pBackBuffer;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_pBackBuffer);
	hr = g_pd3dDevice->CreateRenderTargetView(g_pBackBuffer, NULL, &g_pRenderTargetView);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed create render target view!", "Error", MB_OK);
		g_pBackBuffer->Release();
		return hr;
	}
	g_pBackBuffer->Release();
	if (Multisampling)
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthBufferDesc.Width = Width;
		depthBufferDesc.Height = Height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 8;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hr = g_pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &g_pDepthStencilBuffer);
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Failed create depthBuffer!", "Error", MB_OK);
		}
		//? Describe depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		g_pd3dDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depthStencilViewDesc, &g_pDepthView);
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Failed create depthView!", "Error", MB_OK);
			return hr;
		}
		//? Describe depth dtencil
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthState);
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Failed create depthState!", "Error", MB_OK);
			return hr;
		}
		g_pImmediateContext->OMSetDepthStencilState(g_pDepthState, 0);
		g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthView);
		//? Describe rasterizer
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 100;
		rasterDesc.DepthBiasClamp = 100.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = true;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 100.0f;
		hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_pRasterizerState);
		if (FAILED(hr))
		{
			MessageBoxA(NULL, "Failed create Rasterizer state!", "Error", MB_OK);
		}
		g_pImmediateContext->RSSetState(g_pRasterizerState);
	}
	else
		g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	ID3D10Blob* pVSBlob = NULL;
	hr = CompileShaderFromMemory(VertexShader, sizeof(VertexShader), "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed compile vertex shader from memory!", "Error", MB_OK);
		return hr;
	}
	ID3D10Blob* pPSBlob = NULL;
	hr = CompileShaderFromMemory(PixelShader, sizeof(PixelShader), "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed compile pixel shader from memory!", "Error", MB_OK);
		return hr;
	}
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed create vertex shader!", "Error", MB_OK);
		return hr;
	}
	g_pd3dDevice->CreateInputLayout(Layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		pPSBlob->Release();
		MessageBoxA(NULL, "Failed create input layout!", "Error", MB_OK);
		return hr;
	}
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
	pVSBlob->Release();
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	if (FAILED(hr))
	{
		pPSBlob->Release();
		MessageBoxA(NULL, "Failed create pixel shader!", "Error", MB_OK);
		return hr;
	}
	pPSBlob->Release();
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexTemplate) * 300;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed create buffer!", "Error", MB_OK);
		return hr;
	}
	UINT stride = sizeof(VertexTemplate);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	m_pBatch = new  DirectX::SpriteBatch(g_pImmediateContext);
	m_pFont = new DirectX::SpriteFont(g_pd3dDevice, L"Arial32.spritefont");
	return S_OK;
}
HRESULT DrawingClass::DrawLine(ID3D11DeviceContext*      g_pImmediateContext, XMFLOAT2 pos1, XMFLOAT2 pos2)
{
	HRESULT hr = S_OK;
	VertexTemplate* v = nullptr;
	D3D11_MAPPED_SUBRESOURCE mapData;
	hr = g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed map vertex buffer!", "Error", MB_OK);
		return hr;
	}
	v = (VertexTemplate*)mapData.pData;
	v[0].Pos = XMFLOAT3(pos1.x, pos1.y, 0.0f);
	v[0].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	v[1].Pos = XMFLOAT3(pos2.x, pos2.y, 0.0f);
	v[1].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	g_pImmediateContext->Unmap(g_pVertexBuffer, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	//g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);
	g_pImmediateContext->Draw(2, 0);
	return S_OK;
}
HRESULT DrawingClass::DrawCircle(ID3D11DeviceContext*      g_pImmediateContext,XMFLOAT2 center, UINT radius, UINT numVertices)
{
	HRESULT hr = S_OK;
	UINT ViewportNumber = 1;
	D3D11_VIEWPORT vp;
	g_pImmediateContext->RSGetViewports(&ViewportNumber, &vp);
	float WedgeAngle = (float)((2 * 3.141592654f) / numVertices);
	D3D11_MAPPED_SUBRESOURCE mapData;
	VertexTemplate* v = nullptr;
	hr = g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed map vertex buffer!", "Error", MB_OK);
		return hr;
	}
	v = (VertexTemplate*)mapData.pData;
	for (int i = 0; i <= numVertices; i++)
	{
		float Theta = (float)(i * WedgeAngle);
		float x = (float)(center.x + radius * cos(Theta));
		float y = (float)(center.y - radius * sin(Theta));
		v[i].Pos.x = 2.0f * (x - 0.5f) / vp.Width - 1.0f;
		v[i].Pos.y = 1.0f - 2.0f * (y - 0.5f) / vp.Height;
		v[i].Pos.z = 0.0f;
		v[i].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	}
	g_pImmediateContext->Unmap(g_pVertexBuffer, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);
	UINT stride = sizeof(VertexTemplate);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pImmediateContext->Draw(numVertices + 1, 0);
	return S_OK;
}
HRESULT DrawingClass::DrawString(const char* Text, DirectX::XMFLOAT2 Pos, DirectX::XMVECTOR Color)
{
	m_pBatch->Begin();
	m_pFont->DrawString(m_pBatch, Text, Pos, Color, 0);
	m_pBatch->End();
	return S_OK;
}
HRESULT DrawingClass::DrawRect(ID3D11DeviceContext * g_pImmediateContext, XMFLOAT2 From, XMFLOAT2 To)
{
	HRESULT hr = S_OK;
	VertexTemplate* v = nullptr;
	D3D11_MAPPED_SUBRESOURCE mapData;
	hr = g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData);
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed map vertex buffer!", "Error", MB_OK);
		return hr;
	}
	v = (VertexTemplate*)mapData.pData;
	v[0].Pos = XMFLOAT3(From.x, From.y, 0.0f);
	v[0].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	v[1].Pos = XMFLOAT3(To.x, From.y, 0.0f);
	v[1].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	v[2].Pos = XMFLOAT3(To.x, To.y, 0.0f);
	v[2].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	v[3].Pos = XMFLOAT3(From.x, To.y, 0.0f);
	v[3].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	v[4].Pos = XMFLOAT3(From.x, From.y, 0.0f);
	v[4].color = XMFLOAT4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f);
	g_pImmediateContext->Unmap(g_pVertexBuffer, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);
	g_pImmediateContext->Draw(5, 0);
	return S_OK;
}
void DrawingClass::Render(IDXGISwapChain*           g_pSwapChain)
{
	g_pSwapChain->Present(0, 0);
}
void DrawingClass::ClearRenderTarget()
{
	float Color[] = { 0.0f,0.0f, 0.0f, 0.0f };
	//g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, (const FLOAT*)&Color);
}