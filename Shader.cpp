//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Mesh.h"

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) {

	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);

}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) {

	D3D12_SHADER_BYTECODE d3dShaderByteCode;

	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);

}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char *pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
}

void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature) 
{

	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);

	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterialColors *pMaterialColors)
{
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CIlluminatedShader::CIlluminatedShader()
{
}

CIlluminatedShader::~CIlluminatedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CIlluminatedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] ={ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CIlluminatedShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CIlluminatedShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CIlluminatedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_d3dPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[1]);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CIlluminatedShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);
}

/////////////////////////////////////////////////////////////

CPlayerShader::CPlayerShader() {

}

CPlayerShader::~CPlayerShader() {

}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout() {

	UINT nInputElementDescs = 3;

	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);

}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) {

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", ppd3dShaderBlob));

}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) {

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", ppd3dShaderBlob));

}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature) {

	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}

/////////////////////////////////////////////////////////////

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout() 
{

	UINT nInputElementDescs = 3;

	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);

}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) 
{

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", ppd3dShaderBlob));

}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) 
{

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", ppd3dShaderBlob));

}

void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature) 
{

	m_nPipelineStates = 1;

	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext) 
{

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	//float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();
	//float fxPitch = 12.0f * 3.5f;
	//float fyPitch = 12.0f * 3.5f;
	//float fzPitch = 12.0f * 3.5f;

	////직육면체를 지형 표면에 그리고 지형보다 높은 위치에 일정한 간격으로 배치한다.
	//int xObjects = int(fTerrainWidth / fxPitch), yObjects = 2, zObjects = int(fTerrainLength / fzPitch);

	//m_nObjects = xObjects * yObjects * zObjects;
	//m_ppObjects = new CGameObject * [m_nObjects];

	//CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	//CSphereMeshDiffused* pSphereMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, 20, 20);

	//XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	//CRotatingObject* pRotatingObject = NULL;
	//for (int i = 0, x = 0; x < xObjects; x++)
	//{
	//	for (int z = 0; z < zObjects; z++)
	//	{
	//		for (int y = 0; y < yObjects; y++)
	//		{
	//			pRotatingObject = new CRotatingObject(1);
	//			pRotatingObject->SetMesh(0, (i % 2) ? (CMesh*)pCubeMesh : (CMesh*)pSphereMesh);

	//			float xPosition = x * fxPitch;
	//			float zPosition = z * fzPitch;
	//			float fHeight = pTerrain->GetHeight(xPosition, zPosition);

	//			pRotatingObject->SetPosition(xPosition, fHeight + (y * 10.0f * fyPitch) + 6.0f, zPosition);
	//			if (y == 0)
	//			{
	//				/*
	//					지형의 표면에 위치하는 직육면체는 지형의 기울기에 따라 방향이 다르게 배치한다.
	//					직육면체가 위치할 지형의 법선 벡터 방향과 직육면체의 y-축이 일치하도록 한다.
	//				*/
	//				xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
	//				xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
	//				if (Vector3::IsZero(xmf3RotateAxis))
	//					xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//				float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));

	//				pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
	//			}
	//			pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	//			pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
	//			m_ppObjects[i++] = pRotatingObject;
	//		}
	//	}
	//}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

void CObjectsShader::ReleaseObjects() {

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}

}

void CObjectsShader::AnimateObjects(float fTimeElapsed) {

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}

}

void CObjectsShader::ReleaseUploadBuffers() {

	if (m_ppObjects) {
		for (int j = 0; j < m_nObjects; j++)
			m_ppObjects[j]->ReleaseUploadBuffers();
	}

}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {

	CShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++) {
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}

}

CGameObject* CObjectsShader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance) {

	int nIntersected = 0; *pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;

	CGameObject* pSelectedObject = NULL;

	for (int j = 0; j < m_nObjects; j++) {
		nIntersected = m_ppObjects[j]->PickObjectByRayIntersection(0, xmf3PickPosition, xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}

	return(pSelectedObject);

}

/////////////////////////////////////////////////////////

CTerrainShader::CTerrainShader() 
{
}

CTerrainShader::~CTerrainShader() 
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout() 
{

	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;

	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);

}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) 
{

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", ppd3dShaderBlob));

}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) 
{

	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", ppd3dShaderBlob));

}

void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature) 
{

	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}

