﻿#include "stdafx.h"
#include "Mesh.h"


CMesh::~CMesh() { 

	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release(); 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release(); 

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release(); 
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

	if (m_pVertices) delete[] m_pVertices; 
	if (m_pnIndices) delete[] m_pnIndices;

}

void CMesh::ReleaseUploadBuffers() 
{ 

	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) 
		m_pd3dVertexUploadBuffer->Release(); 
	m_pd3dVertexUploadBuffer = NULL; 

	if (m_pd3dIndexUploadBuffer) 
		m_pd3dIndexUploadBuffer->Release(); 
	m_pd3dIndexUploadBuffer = NULL;

};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList) 
{

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology); 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView); 

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView); 
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0); 
	} 
	else
	{ 
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0); 
	} 

}
	

CTriangleMesh::CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList) 
{
	 
	m_nVertices = 3; 
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	CDiffusedVertex pVertices[3]; 
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)); 
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress(); 
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 

}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{ 
	
	m_nVertices = 8;
	
	m_nStride = sizeof(CDiffusedVertex); 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pVertices = new CDiffusedVertex[m_nVertices];
	m_pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR); 
	m_pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR); 
	m_pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR); 
	m_pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR); 
	m_pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	m_pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형    
	m_pnIndices[0] = 3; 
	m_pnIndices[1] = 1; 
	m_pnIndices[2] = 0;
	
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형   
	m_pnIndices[3] = 2; 
	m_pnIndices[4] = 1; 
	m_pnIndices[5] = 3; 
	
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형   
	m_pnIndices[6] = 0;
	m_pnIndices[7] = 5;
	m_pnIndices[8] = 4; 
	
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형   
	m_pnIndices[9] = 1; 
	m_pnIndices[10] = 5;
	m_pnIndices[11] = 0; 
	
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형   
	m_pnIndices[12] = 3; 
	m_pnIndices[13] = 4; 
	m_pnIndices[14] = 7; 
	
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형   
	m_pnIndices[15] = 0;
	m_pnIndices[16] = 4;
	m_pnIndices[17] = 3; 
	
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형 
	m_pnIndices[18] = 1; 
	m_pnIndices[19] = 6;
	m_pnIndices[20] = 5; 
	
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형   
	m_pnIndices[21] = 2; 
	m_pnIndices[22] = 6;
	m_pnIndices[23] = 1; 
	
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형   
	m_pnIndices[24] = 2;
	m_pnIndices[25] = 7;
	m_pnIndices[26] = 6; 
	
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형   
	m_pnIndices[27] = 3;
	m_pnIndices[28] = 7; 
	m_pnIndices[29] = 2; 
	
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형   
	m_pnIndices[30] = 6;
	m_pnIndices[31] = 4;
	m_pnIndices[32] = 5; 
	
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형    
	m_pnIndices[33] = 7;
	m_pnIndices[34] = 4;
	m_pnIndices[35] = 6;
	 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress(); 
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT; 
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices; 

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}


CCubeMeshDiffused::~CCubeMeshDiffused()
{ 

}

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList) 
{
	
	m_nVertices = 24 * 3; 
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pVertices = new CDiffusedVertex[m_nVertices];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//비행기 메쉬의 위쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 아래쪽 면 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 오른쪽 면 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 왼쪽 면 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 뒤쪽/왼쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR)); 
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//메쉬의 바운딩 박스(모델 좌표계)를 생성한다.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused() 
{

}


CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale) 
{
	
	m_nWidth = nWidth; 
	m_nLength = nLength; 
	m_xmf3Scale = xmf3Scale;

	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength]; 
	
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead; ::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL); 
	
	::CloseHandle(hFile);
	
	
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++) 
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapPixels[x + (y*m_nWidth)]; 
		}
	}

	if (pHeightMapPixels) 
		delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage() 
{ 

	if (m_pHeightMapPixels) 
		delete[] m_pHeightMapPixels; 

	m_pHeightMapPixels = NULL; 

}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z) 
{ 

	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) 
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth); 
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1; 
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y; 
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y; 
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y; 
	
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z); 
	 
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);

}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz) { 

	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) 
		return(0.0f);

	int x = (int)fx; int z = (int)fz; 
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)]; 
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)]; 
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)]; 
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER  
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft) { 
	
	
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);

		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);

	}

	else { 

		
	
		if (fzPercent < (1.0f - fxPercent)) 
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft); 
		
		else 
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	} 
#endif

	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);

}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void *pContext) : CMesh(pd3dDevice, pd3dCommandList) 
{
	
	m_nVertices = nWidth * nLength; 
	m_nStride = sizeof(CDiffusedVertex);
							
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	CDiffusedVertex *pVertices = new CDiffusedVertex[m_nVertices];
	
	
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX; 
	
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++) 
		{ 	
			XMFLOAT3 xmf3Position = XMFLOAT3((x*m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z*m_xmf3Scale.z)); XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color); pVertices[i] = CDiffusedVertex(xmf3Position, xmf3Color); 
			
			if (fHeight < fMinHeight) 
				fMinHeight = fHeight;

			if (fHeight > fMaxHeight) 
				fMaxHeight = fHeight; 
		} 
	} 
	
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	
	delete[] pVertices;
	
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1); 
	
	UINT *pnIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < nLength - 1; z++) {
		if ((z % 2) == 0) { 
			for (int x = 0; x < nWidth; x++) 
			{ 
				if ((x == 0) && (z > 0)) 
					pnIndices[j++] = (UINT)(x + (z * nWidth)); 

				pnIndices[j++] = (UINT)(x + (z * nWidth)); 
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth); 
			} 
		}
		else { 
			
			for (int x = nWidth - 1; x >= 0; x--) 
			{ 
				if (x == (nWidth - 1))
					pnIndices[j++] = (UINT)(x + (z * nWidth));
				
				pnIndices[j++] = (UINT)(x + (z * nWidth)); 
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			} 
		} 
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT; 
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	
	delete[] pnIndices;
				
}
				
CHeightMapGridMesh::~CHeightMapGridMesh()
{ 

}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext) 
{
	
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext; 
	BYTE *pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale(); 
	
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z*nWidth)] * xmf3Scale.y; 
	
	return(fHeight); 

}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void *pContext) 
{ 
	
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection); 

	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale(); 
	
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f); 
	

	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection); 
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection); 
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection); 
	fScale = (fScale / 4.0f) + 0.05f; 
	
	if (fScale > 1.0f) 
		fScale = 1.0f;

	if (fScale < 0.25f)
		fScale = 0.25f; 
	
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	
	return(xmf4Color); 

}


CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fRadius, int nSlices, int nStacks) : CMesh(pd3dDevice, pd3dCommandList) { 
	

	
	m_nStride = sizeof(CDiffusedVertex); 
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new CDiffusedVertex[m_nVertices];
	
	float fDeltaPhi = float(XM_PI / nStacks);

	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);

	int k = 0; 
	m_pVertices[k++] = CDiffusedVertex(0.0f, +fRadius, 0.0f, RANDOM_COLOR);
	float theta_i, phi_j; 
	
	for (int j = 1; j < nStacks; j++) 
	{ 
		phi_j = fDeltaPhi * j; 
		for (int i = 0; i < nSlices; i++)
		{ 
			theta_i = fDeltaTheta * i;
			m_pVertices[k++] = CDiffusedVertex(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i), RANDOM_COLOR); 
		} 
	} 
	m_pVertices[k] = CDiffusedVertex(0.0f, -fRadius, 0.0f, RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress(); 
	m_d3dVertexBufferView.StrideInBytes = m_nStride; 
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	k = 0;
	
	for (int i = 0; i < nSlices; i++) 
	{ 
		m_pnIndices[k++] = 0; m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	} 

	for (int j = 0; j < nStacks-2; j++)
	{ 
		for (int i = 0; i < nSlices; i++) 
		{ 
			m_pnIndices[k++] = 1 + (i + (j * nSlices)); 
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices)); 
			
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		} 
	}

	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1); 
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i; 
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress(); 
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT; 
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius, fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshDiffused::~CSphereMeshDiffused() { 

}

int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float *pfNearHitDistance) {

	//하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있다. 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다. 
	int nIntersections = 0; 
	BYTE *pbPositions = (BYTE *)m_pVertices;
	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1; 
	

	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin); 
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection); 
	
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection, *pfNearHitDistance); 
	
	if (bIntersected)
	{ 
		float fNearHitDistance = FLT_MAX; 
	
		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_nStride));
			float fHitDistance;

			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections); 
}