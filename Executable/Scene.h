#pragma once

class CTimer;
class CShader;
class CObjectsShader;
class CCamera;
class CGameObject;
class CHeightMapTerrain;

class CScene
{

public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera *pCamera);

	/* 따라하기 7번 */

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice); 
	ID3D12RootSignature *GetGraphicsRootSignature();
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다. 
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;


	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;

protected: 
	CHeightMapTerrain *m_pTerrain = NULL;

public: 
	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }

};

