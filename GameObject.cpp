#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"

CGameObject::CGameObject(int nMeshes) {

	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes; 
	m_ppMeshes = NULL;
	
	if (m_nMeshes > 0) 
	{ 
		m_ppMeshes = new CMesh*[m_nMeshes]; 

		for (int i = 0; i < m_nMeshes; i++) 
			m_ppMeshes[i] = NULL; 
	}

}


CGameObject::~CGameObject() { 

	if (m_ppMeshes) 
	{ 
		for (int i = 0; i < m_nMeshes; i++) 
		{ 
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL; 
		} 
		delete[] m_ppMeshes;
	}

	if (m_pShader) 
	{ 
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release(); 
	}

}

void CGameObject::SetShader(CShader *pShader) { 

	if (m_pShader) m_pShader->Release();
	
	m_pShader = pShader; 
	
	if (m_pShader) m_pShader->AddRef(); 

}

void CGameObject::SetMesh(int nIndex, CMesh *pMesh) { 
	
	if (m_ppMeshes) 
	{
		if (m_ppMeshes[nIndex]) 
			m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh; 

		if (pMesh) 
			pMesh->AddRef(); 
	} 

}

void CGameObject::ReleaseUploadBuffers() { 

	if (m_ppMeshes)
	{ 
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) 
				m_ppMeshes[i]->ReleaseUploadBuffers();
		} 
	}

}

void CGameObject::Animate(float fTimeElapsed) 
{ 

}

void CGameObject::OnPrepareRender() 
{
	m_xmf4x4World._11 = GetRight().x;		m_xmf4x4World._12 = GetRight().y;		m_xmf4x4World._13 = GetRight().z;
	m_xmf4x4World._21 = GetUp().x;			m_xmf4x4World._22 = GetUp().y;			m_xmf4x4World._23 = GetUp().z;
	m_xmf4x4World._31 = GetLook().x;		m_xmf4x4World._32 = GetLook().y;		m_xmf4x4World._33 = GetLook().z;
	m_xmf4x4World._41 = GetPosition().x;	m_xmf4x4World._42 = GetPosition().y;	m_xmf4x4World._43 = GetPosition().z;

}

void CGameObject::OnUpdateTransform()
{
	m_xmf4x4World._11 = GetRight().x;		m_xmf4x4World._12 = GetRight().y;		m_xmf4x4World._13 = GetRight().z;
	m_xmf4x4World._21 = GetUp().x;			m_xmf4x4World._22 = GetUp().y;			m_xmf4x4World._23 = GetUp().z;
	m_xmf4x4World._31 = GetLook().x;		m_xmf4x4World._32 = GetLook().y;		m_xmf4x4World._33 = GetLook().z;
	m_xmf4x4World._41 = GetPosition().x;	m_xmf4x4World._42 = GetPosition().y;	m_xmf4x4World._43 = GetPosition().z;
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {
	
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);

	if (m_ppMeshes)
	{ 
		for (int i = 0; i < m_nMeshes; i++) 
		{ 
			if (m_ppMeshes[i]) 
				m_ppMeshes[i]->Render(pd3dCommandList);
		} 
	} 

}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle) { 
	
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle)); 
	
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World); 

}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { 

}

void CGameObject::ReleaseShaderVariables() { 

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {
	
	XMFLOAT4X4 xmf4x4World; 

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0); 

}

void CGameObject::SetPosition(float x, float y, float z) { 
	
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z; 

}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position) { 
	
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z); 

}

XMFLOAT3 CGameObject::GetPosition() { 

	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 

}
XMFLOAT3 CGameObject::GetLook() { 

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); 

}

XMFLOAT3 CGameObject::GetUp() {

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); 

}
 
XMFLOAT3 CGameObject::GetRight() { 

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); 

}

void CGameObject::MoveStrafe(float fDistance) { 

	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Right = GetRight(); 

	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);

	CGameObject::SetPosition(xmf3Position);

}

void CGameObject::MoveUp(float fDistance) { 
	
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp(); 
	
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);

	CGameObject::SetPosition(xmf3Position);

}

void CGameObject::MoveForward(float fDistance) { 
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Look = GetLook();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	
	CGameObject::SetPosition(xmf3Position); 

}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) {

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection) {
	
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f); 

	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);

	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse); 

	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin)); 

}

int CGameObject::PickObjectByRayIntersection(int index, XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float *pfHitDistance) {

	int nIntersected = 0; 

	if (m_ppMeshes[index])
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection; 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		nIntersected = m_ppMeshes[index]->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}

	return(nIntersected); 
}


void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}



///////////////////////////////////////////////////////////////////////////

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes) {
	
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f); 
	m_fRotationSpeed = 15.0f; 

}

CRotatingObject::~CRotatingObject() {

}

void CRotatingObject::Animate(float fTimeElapsed) { 
	
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed); 

}

//////////////////////////////////////////////////////////////

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0) {
	
	m_nWidth = nWidth; m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1; 
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock; long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks; 
	
	m_ppMeshes = new CMesh*[m_nMeshes]; 
	
	for (int i = 0; i < m_nMeshes; i++)
		m_ppMeshes[i] = NULL;
	
	CHeightMapGridMesh *pHeightMapGridMesh = NULL; 
	
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1); 
			zStart = z * (nBlockLength - 1);
 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	} 
	CTerrainShader *pShader = new CTerrainShader(); 

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature); 
	SetShader(pShader); 

}

CHeightMapTerrain::~CHeightMapTerrain(void) {

	if (m_pHeightMapImage)
		delete m_pHeightMapImage;

}

CBulletObject::CBulletObject(float fEffectiveRange, int nMeshes) : CGameObject(nMeshes)
{
	m_fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;

	m_bActive = false;
}

void CBulletObject::Animate(float fElapsedTime)
{
	m_fElapsedTimeAfterFire += fElapsedTime;

	float fDistance = m_fMovingSpeed * fElapsedTime;

	if ((m_fElapsedTimeAfterFire > m_fLockingDelayTime) && m_pLockedObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMVECTOR xmvPosition = XMLoadFloat3(&xmf3Position);

		XMFLOAT3 xmf3LockedObjectPosition = m_pLockedObject->GetPosition();
		XMVECTOR xmvLockedObjectPosition = XMLoadFloat3(&xmf3LockedObjectPosition);
		XMVECTOR xmvToLockedObject = xmvLockedObjectPosition - xmvPosition;
		xmvToLockedObject = XMVector3Normalize(xmvToLockedObject);

		XMVECTOR xmvMovingDirection = XMLoadFloat3(&m_xmf3MovingDirection);
		xmvMovingDirection = XMVector3Normalize(XMVectorLerp(xmvMovingDirection, xmvToLockedObject, 0.25f));
		XMStoreFloat3(&m_xmf3MovingDirection, xmvMovingDirection);
	}
#ifdef _WITH_VECTOR_OPERATION
	XMFLOAT3 xmf3Position = GetPosition();

	m_fRotationAngle += m_fRotationSpeed * fElapsedTime;
	if (m_fRotationAngle > 360.0f) m_fRotationAngle = m_fRotationAngle - 360.0f;

	XMFLOAT4X4 mtxRotate1 = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationAngle, 0.0f);

	XMFLOAT3 xmf3RotationAxis = Vector3::CrossProduct(m_xmf3RotationAxis, m_xmf3MovingDirection, true);
	float fDotProduct = Vector3::DotProduct(m_xmf3RotationAxis, m_xmf3MovingDirection);
	float fRotationAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : (float)XMConvertToDegrees(acos(fDotProduct));
	XMFLOAT4X4 mtxRotate2 = Matrix4x4::RotationAxis(xmf3RotationAxis, fRotationAngle);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate1, mtxRotate2);

	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
#else
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
	m_fMovingDistance += fDistance;
#endif

	UpdateBoundingBox();

	if ((m_fMovingDistance > m_fBulletEffectiveRange) || (m_fElapsedTimeAfterFire > m_fLockingTime)) Reset();
}

CAirplaneObject::CAirplaneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes) : CGameObject(nMeshes) 
{

	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 4.0f, 1.0f);
	for (int i = 0; i < 50; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(0, pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(120.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

CAirplaneObject::~CAirplaneObject()
{
	for (int i = 0; i < 50; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CAirplaneObject::OnPrepareRender() 
{

	CGameObject::OnPrepareRender();

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);

}

void CAirplaneObject::OnUpdateTransform()
{
	CGameObject::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CAirplaneObject::FireBullet(CGameObject* pLockedObject)
{

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < 50; i++)
	{
		if (!m_ppBullets[i]->m_bActive)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3Direction = GetUp();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);

		if (pLockedObject)
		{
			pBulletObject->m_pLockedObject = pLockedObject;
		}
	}
}
