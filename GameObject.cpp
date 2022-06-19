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

	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
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

	//게임 객체가 포함하는 모든 메쉬를 렌더링한다.
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
	
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
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
//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook() { 

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); 

}

//게임 객체의 로컬 y-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetUp() {

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); 

}

//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight() { 

	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); 

}

//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance) { 

	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Right = GetRight(); 

	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);

	CGameObject::SetPosition(xmf3Position);

}

//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance) { 
	
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp(); 
	
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);

	CGameObject::SetPosition(xmf3Position);

}

//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance) { 
	XMFLOAT3 xmf3Position = GetPosition(); 
	XMFLOAT3 xmf3Look = GetLook();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	
	CGameObject::SetPosition(xmf3Position); 

}

//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) {

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	/*XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));*/
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection) {
	
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f); 

	//카메라 좌표계의 원점을 모델 좌표계로 변환한다. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);

	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다. 
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse); 

	//광선의 방향 벡터를 구한다. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin)); 

}

int CGameObject::PickObjectByRayIntersection(int index, XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float *pfHitDistance) {

	int nIntersected = 0; 

	if (m_ppMeshes[index])
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection; 
		//모델 좌표계의 광선을 생성한다. 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다. 
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
	
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth; m_nLength = nLength;

	/*
		지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크 기이다.
		cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.
	*/ 
	int cxQuadsPerBlock = nBlockWidth - 1; 
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다. 
	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock; long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
	m_nMeshes = cxBlocks * czBlocks; 
	
	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다.
	m_ppMeshes = new CMesh*[m_nMeshes]; 
	
	for (int i = 0; i < m_nMeshes; i++)
		m_ppMeshes[i] = NULL;
	
	CHeightMapGridMesh *pHeightMapGridMesh = NULL; 
	
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다. 
			xStart = x * (nBlockWidth - 1); 
			zStart = z * (nBlockLength - 1);

			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다. 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}
	//지형을 렌더링하기 위한 셰이더를 생성한다. 
	CTerrainShader *pShader = new CTerrainShader(); 

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature); 
	SetShader(pShader); 

}

CHeightMapTerrain::~CHeightMapTerrain(void) {

	if (m_pHeightMapImage)
		delete m_pHeightMapImage;

}

CBulletObject::CBulletObject(float fEffectiveRange)
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
	//SetPlayerUpdatedContext(pTerrain);

	//SetCameraUpdatedContext(pTerrain);

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

void CAirplaneObject::OnPrepareRender() {

	CGameObject::OnPrepareRender();
	//비행기 모델을 그리기 전에 x-축으로 90도 회전한다. 

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	static bool num{ false };
	if (!num) {
		m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
		num = true;
	}
}

void CAirplaneObject::OnUpdateTransform()
{
	CGameObject::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CAirplaneObject::FireBullet(CGameObject* pLockedObject)
{

	/*if (pLockedObject)
	{
		LookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		OnUpdateTransform();
	}*/


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

//void CAirplaneObject::OnPlayerUpdateCallback(float fTimeElapsed) {
//
//	XMFLOAT3 xmf3PlayerPosition = GetPosition();
//	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
//	/*
//	지형에서 플레이어의 현재 위치 (x, z)의 지형 높이(y)를 구한다.
//	그리고 플레이어 메쉬의 높이가 12이고 플레이어의 중심이 직육면체의 가운데이므로 y 값에 메쉬의 높이의 절반을 더하면 플레이어의 위치가 된다.
//	*/
//	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
//
//	/*
//		플레이어의 위치 벡터의 y-값이 음수이면(예를 들어, 중력이 적용되는 경우) 플레이어의 위치 벡터의 y-값이 점점 작아지게 된다.
//		이때 플레이어의 현재 위치 벡터의 y 값이 지형의 높이(실제로 지형의 높이 + 6)보다 작으면 플레이어의 일부가 지형 아래에 있게 된다.
//		이러한 경우를 방지하려면 플레이어의 속도 벡터의 y 값을 0으로 만들고 플레이어 의 위치 벡터의 y-값을 지형의 높이(실제로 지형의 높이 + 6)로 설정한다.
//		그러면 플레이어는 항상 지형 위에 있게 된 다.
//	*/
//
//	if (xmf3PlayerPosition.y < fHeight)
//	{
//		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
//		xmf3PlayerVelocity.y = 0.0f;
//		SetVelocity(xmf3PlayerVelocity);
//		xmf3PlayerPosition.y = fHeight;
//		SetPosition(xmf3PlayerPosition);
//	}
//
//}
