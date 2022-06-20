#pragma once
#include "Timer.h"

class CScene;
class CGameTimer;
class CGameObject;
class CPlayer;
class CCamera;

class CGameFramework
{
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4*				m_pdxgiFactory;		// DXGI ���丮 �������̽��� ���� ������
	IDXGISwapChain3*			m_pdxgiSwapChain;	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���� �ʿ�
	ID3D12Device*				m_pd3dDevice;		// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	bool						m_bMsaa4xEnable = false;	// ���ø� ����
	UINT						m_nMsaa4xQualityLevels = 0;	// ���ø� ǰ�� ���� 0 �̸� ��� ���� 

	static const UINT			m_nSwapChainBuffers = 2;	// ���� ü���� �ĸ� ������ ����
	UINT						m_nSwapChainBufferIndex;	// ���� ü���� �ĸ� ���� �ε��� 

	ID3D12Resource*				m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];	// ���� Ÿ�� ����
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap;							// ������ �� �������̽� ������
	UINT						m_nRtvDescriptorIncrementSize;						// ���� Ÿ�� ������ ������ ũ��


	ID3D12Resource*				m_pd3dDepthStencilBuffer;		// ���� - ���ٽ� ����
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap;			// ������ �� �������̽� ������
	UINT						m_nDsvDescriptorIncrementSize;	// ���� - ���ٽ� ������ ������ ũ��

	ID3D12CommandQueue*			m_pd3dCommandQueue;		// ��� ť �������̽� ������
	ID3D12CommandAllocator*		m_pd3dCommandAllocator;	// ��� �Ҵ��� �������̽� ������
	ID3D12GraphicsCommandList*	m_pd3dCommandList;		// ��� ����Ʈ �������̽� ������

	ID3D12Fence*				m_pd3dFence;	// �������̽� ������ 
	UINT64						m_nFenceValue;	// �潺�� �� ( �ʱ� �� )
	HANDLE						m_hFenceEvent;	// �̺�Ʈ �ڵ�

	UINT64						m_nFenceValues[m_nSwapChainBuffers];

	CScene*						m_pScene;
	CCamera						*m_pCamera = NULL;

#if defined(_DEBUG)
					/* Debug Layer Interfaces */
	ID3D12Debug*				m_pd3dDebugController;
#endif

	ID3D12PipelineState*		m_pd3dPipelineState; // ������ ������ ���¸� ��Ÿ���� ���� 

	CGameTimer					m_GameTimer;
	_TCHAR						m_pszFrameRate[50];

	//�÷��̾� ��ü�� ���� �������̴�.
	CPlayer *m_pPlayer = NULL;
		
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT m_ptOldCursorPos;
	CGameObject *m_pSelectedObject = NULL;
	CGameObject* m_pLockedObject = NULL;

public:

	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	// ������ ��ũ�� �ʱ�ȭ �ϴ� �Լ� ( �� �����찡 �����Ǹ� ȣ�� )

	void OnDestroy();									// �����찡 ���� �� ȣ�� �Ǵ� �Լ� 

	void CreateSwapChain();					// ���� ü�� ����
	void CreateDirect3DDevice();			// Direct3DDevice �������̽� ���� �Լ� 
											// Rtv : Render Target View   
											// Dsv : Depth Stencil View 
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();		// CommandQueue & List �������̽��� �����ϴ� �Լ�
	void CreateRenderTargetView();			// ���� Ÿ�� �� �����ڸ� �����ϴ� �Լ� 
	void CreateDepthStencilView();			// ���� - ���ٽ� ���ۿ� �����ڸ� �����ϴ� �Լ�

	void BuildObjects();	// �������� ��ü�� �����ϴ� �Լ�
	void ReleaseObjects();	// �������� ��ü�� �Ҹ��Ű�� �Լ�

	void ProcessInput();	// ����� �Է��� ó���ϴ� �Լ�
	void AnimateObjects();	// ������Ʈ���� �������� ó���ϴ� �Լ�
	void FrameAdvance();	// ������Ʈ���� ������ ���ִ� �Լ�

	void WaitForGpuComplete();	// GPU�� ���� ���� �� ���� CPU�� ��ٸ��� �ϴ� �Լ�.

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void OnResizeBackBuffers();
	
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void MoveToNextFrame();
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	virtual void Collision();
};

