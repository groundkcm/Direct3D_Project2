#pragma once

#include "Mesh.h"
class CMesh;
class CCamera;
class CShader;
class CPlayer;

class CGameObject {

public: 
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

private: 
	int m_nReferences = 0;

public: 
	XMFLOAT4X4					m_xmf4x4World = Matrix4x4::Identity();
	bool						m_bActive = true;
	float						m_fMovingSpeed = 0.0f;
	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3					m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float						m_fRotationSpeed = 0.0f;
	CMesh						*m_pMesh = NULL;
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

	void AddRef() { m_nReferences++; } 
	void Release() { if (--m_nReferences <= 0) delete this; }
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

protected:
	CShader *m_pShader = NULL;
	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다. 
	CMesh **m_ppMeshes = NULL;
	int m_nMeshes = 0;

public: 
	void ReleaseUploadBuffers();
	virtual void SetShader(CShader *pShader);
	virtual void SetMesh(int nIndex, CMesh *pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender(); 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void SetActive(bool bActive) { m_bActive = bActive; }
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }

	void UpdateBoundingBox();
	virtual void OnUpdateTransform();


public: 
	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); 
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual void ReleaseShaderVariables();

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp(); 
	XMFLOAT3 GetRight();
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	//게임 객체의 위치를 설정한다. 
	void SetPosition(float x, float y, float z); 
	void SetPosition(XMFLOAT3 xmf3Position);

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f); 
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//게임 객체를 회전(x-축, y-축, z-축)한다. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	//모델 좌표계의 픽킹 광선을 생성한다. 
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection); 
	
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다. 
	int PickObjectByRayIntersection(int index, XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float *pfHitDistance);

};

class CRotatingObject : public CGameObject {

public: 
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private: 
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public: 
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; } 
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	
	virtual void Animate(float fTimeElapsed);

};

//////////////////////////////////////////////////////////

class CHeightMapTerrain : public CGameObject {

public: 
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color); virtual ~CHeightMapTerrain();

private: 
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage *m_pHeightMapImage;
		
	//높이 맵의 가로와 세로 크기이다. 
	int m_nWidth; 
	int m_nLength;
		 
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다. 
	XMFLOAT3 m_xmf3Scale;
	
public: 
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다. 
	float GetHeight(float x, float z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);}
	
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다. 
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); } 
	float GetLength() { return(m_nLength * m_xmf3Scale.z); } 

};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Animate(float fElapsedTime);

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingDelayTime = 0.3f;
	float						m_fLockingTime = 4.0f;
	CGameObject* m_pLockedObject = NULL;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void Reset();
};

class CAirplaneObject : public CGameObject
{

public:
	CAirplaneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes = 1);
	virtual ~CAirplaneObject();

	float						m_fBulletEffectiveRange = 150.0f;
	CBulletObject* m_ppBullets[50];

	virtual void OnUpdateTransform();
	void FireBullet(CGameObject* pLockedObject);
	
	void OnPrepareRender();
	//virtual void OnPlayerUpdateCallback(float fTimeElapsed);
};