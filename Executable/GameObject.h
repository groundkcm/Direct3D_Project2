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

	bool						booster = false;

	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();
	CGameObject					*m_pObjectCollided = NULL;
	bool						m_pPlayerCollided = false;

	void AddRef() { m_nReferences++; } 
	void Release() { if (--m_nReferences <= 0) delete this; }
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

protected:
	CShader *m_pShader = NULL;
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
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList); 
	virtual void ReleaseShaderVariables();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp(); 
	XMFLOAT3 GetRight();
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void SetPosition(float x, float y, float z); 
	void SetPosition(XMFLOAT3 xmf3Position);

	void MoveStrafe(float fDistance = 1.0f); 
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection); 
	
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


class CHeightMapTerrain : public CGameObject {

public: 
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color); virtual ~CHeightMapTerrain();

private: 
	CHeightMapImage *m_pHeightMapImage;
		
	int m_nWidth; 
	int m_nLength;
		 
	XMFLOAT3 m_xmf3Scale;
	
public: 
	float GetHeight(float x, float z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);}
	
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); } 
	float GetLength() { return(m_nLength * m_xmf3Scale.z); } 

};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(float fEffectiveRange, int nMeshes = 1);
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

	void OnUpdateTransform();
	void FireBullet(CGameObject* pLockedObject);
	
	void OnPrepareRender();
};