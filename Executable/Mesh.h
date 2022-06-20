#pragma once

class CVertex
{

protected: 
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).    
	XMFLOAT3 m_xmf3Position;

public: 
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); } 
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; } 
	~CVertex() { }

};

class CDiffusedVertex : public CVertex 
{

protected: //정점의 색상이다.    
	XMFLOAT4 m_xmf4Diffuse;

public: 
	CDiffusedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); } 
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; } 
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; } 
	~CDiffusedVertex() { }

};


class CMesh
{

public:    
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {};
	virtual ~CMesh();

private: 
	int m_nReferences = 0;

public: 
	void AddRef() { m_nReferences++; } 
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
	//광선과 메쉬의 교차를 검사하고 교차하는 횟수와 거리를 반환하는 함수이다. 
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float *pfNearHitDistance);

	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

protected: 
	ID3D12Resource *m_pd3dVertexBuffer = NULL; 
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;	   
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; 
	
	UINT m_nSlot = 0; 
	UINT m_nVertices = 0; 
	UINT m_nStride = 0; 
	UINT m_nOffset = 0;

protected: 
	ID3D12Resource *m_pd3dIndexBuffer = NULL; 
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL; 
	
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
		   
	UINT m_nIndices = 0; //인덱스 버퍼에 포함되는 인덱스의 개수이다.
	UINT m_nStartIndex = 0; //인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다.
	int m_nBaseVertex = 0; //인덱스 버퍼의 인덱스에 더해질 인덱스이다

	CDiffusedVertex *m_pVertices = NULL; 
	UINT *m_pnIndices = NULL;

	//모델 좌표계의 OOBB 바운딩 박스이다.
	BoundingOrientedBox m_xmBoundingBox;

public: 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

};

///////////////////////////////////////////////////

class CTriangleMesh : public CMesh { 

public:    
	CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);    

	virtual ~CTriangleMesh() { }
};

class CCubeMeshDiffused : public CMesh {

public: 
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다.
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f); 
	virtual ~CCubeMeshDiffused(); 

};


//////////////////////////////////////////////////

class CAirplaneMeshDiffused : public CMesh { 

public:
	CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f)); 
	virtual ~CAirplaneMeshDiffused();

};

//////////////////////////////////////////////////

class CHeightMapImage {

private: 
	BYTE *m_pHeightMapPixels; //높이 맵 이미지의 가로와 세로 크기이다. 
	int m_nWidth; int m_nLength; //높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다. 
	XMFLOAT3 m_xmf3Scale;

public: 
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale); 
	~CHeightMapImage(void);
		
	float GetHeight(float x, float z); //높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
		
	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); } 
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

};

///////////////////////////////////////////////////

class CHeightMapGridMesh : public CMesh {

protected: 
	//격자의 크기(가로: x-방향, 세로: z-방향)이다. 
	int m_nWidth; 
	int m_nLength; 

	XMFLOAT3 m_xmf3Scale;

public: 
	CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void *pContext = NULL); virtual ~CHeightMapGridMesh();
	XMFLOAT3 GetScale() { return(m_xmf3Scale); } int GetWidth() { return(m_nWidth); } int GetLength() { return(m_nLength); }
	
	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다. 
	virtual float OnGetHeight(int x, int z, void *pContext);
	
	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext); 

};

/////////////////////////////////////////////////////////////////////////////

class CSphereMeshDiffused : public CMesh { 

public: 
	CSphereMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20); 
	virtual ~CSphereMeshDiffused();

};
