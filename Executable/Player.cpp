#include "stdafx.h"
#include "Mesh.h"
#include "Player.h"

CPlayer::CPlayer(int nMeshes) : CGameObject(nMeshes) {

	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f); 
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f); 

	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

}

CPlayer::~CPlayer()
{

	ReleaseShaderVariables();

	if (m_pCamera)
		delete m_pCamera;

}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {

	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (m_pCamera)
		m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

}
void CPlayer::ReleaseShaderVariables() {

	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) 
		m_pCamera->ReleaseShaderVariables();

}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) { 

	CGameObject::UpdateShaderVariables(pd3dCommandList);

}


void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) {

	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0); 
		
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance); 
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance); 
		
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance); 
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�.
		Move(xmf3Shift, bUpdateVelocity); 
	} 

}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity) { 

	if (bUpdateVelocity) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift); 
	} 
	else { 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift); 
		
		m_pCamera->Move(xmf3Shift); 
	} 

}

 //�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�. 
void CPlayer::Rotate(float x, float y, float z) { 
	
	DWORD nCameraMode = m_pCamera->GetMode(); 
	
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA)) 
	{ 
		if (x != 0.0f) {
			m_fPitch += x; 

			if (m_fPitch > +89.0f) 
			{ 
				x -= (m_fPitch - 89.0f);
				m_fPitch = +89.0f; 
			}
			if (m_fPitch < -89.0f)
			{
				x -= (m_fPitch + 89.0f);
				m_fPitch = -89.0f;
			}
		} 
		
		if (y != 0.0f) { 
			//���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����.
			m_fYaw += y;
			
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f; 
			if (m_fYaw < 0.0f) m_fYaw += 360.0f; 
		} 
		
		if (z != 0.0f) {
			
			m_fRoll += z; 
			
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; } 
		}
		
		m_pCamera->Rotate(x, y, z);
		
		if (y != 0.0f) { XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate); m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate); 
		}
		
	}
	else if (nCameraMode == SPACESHIP_CAMERA) { 
		m_pCamera->Rotate(x, y, z); 
		if (x != 0.0f) { 
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate); 
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate); 
		} 
		if (y != 0.0f) { 
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)); 
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate); 
		} 
		if (z != 0.0f) 
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}

	}
	m_xmf3Look = Vector3::Normalize(m_xmf3Look); 
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	
}
void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}
void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}
//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�.
void CPlayer::Update(float fTimeElapsed) 
{ 

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed; 
	
	if (fLength > m_fMaxVelocityXZ) 
	{ 
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength); 
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength); 
	}
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed; 
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y); 
	
	if (fLength > m_fMaxVelocityY) 
		m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	Move(m_xmf3Velocity, false);


	if (m_pPlayerUpdatedContext)
		OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCameraMode = m_pCamera->GetMode();
	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->Update(m_xmf3Position, fTimeElapsed); 
	
	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) 
		OnCameraUpdateCallback(fTimeElapsed);
	
	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position); 
	
	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�.
	m_pCamera->RegenerateViewMatrix();
		
	fLength = Vector3::Length(m_xmf3Velocity); 

	float fDeceleration = (m_fFriction * fTimeElapsed); 
	
	if (fDeceleration > fLength) 
		fDeceleration = fLength;

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true)); 

}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode) 
{
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�. 
	CCamera* pNewCamera = NULL; 
	
	switch (nNewCameraMode)
	{ 
	case FIRST_PERSON_CAMERA: 
		pNewCamera = new CFirstPersonCamera(m_pCamera); 
		break; 
	
	case THIRD_PERSON_CAMERA: 
		pNewCamera = new CThirdPersonCamera(m_pCamera); 
		break; 
	
	case SPACESHIP_CAMERA: 
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break; 
	} 
	
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{ 
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z)); 
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f)); 
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f; 
		m_fRoll = 0.0f; 

		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look); 
		if (m_xmf3Look.x < 0.0f) 
			m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera) { 
		
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector(); 
		m_xmf3Look = m_pCamera->GetLookVector(); 
	}
	if (pNewCamera) {
	 pNewCamera->SetMode(nNewCameraMode);
	 
	 //���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�. 
	 pNewCamera->SetPlayer(this); 
	}
	 if (m_pCamera) 
		 delete m_pCamera;

	 return(pNewCamera);
 }

void CPlayer::OnPrepareRender() 
{ 
	m_xmf4x4World._11 = m_xmf3Right.x;		m_xmf4x4World._12 = m_xmf3Right.y;		m_xmf4x4World._13 = m_xmf3Right.z; 
	m_xmf4x4World._21 = m_xmf3Up.x;			m_xmf4x4World._22 = m_xmf3Up.y;			m_xmf4x4World._23 = m_xmf3Up.z; 
	m_xmf4x4World._31 = m_xmf3Look.x;		m_xmf4x4World._32 = m_xmf3Look.y;		m_xmf4x4World._33 = m_xmf3Look.z; 
	m_xmf4x4World._41 = m_xmf3Position.x;	m_xmf4x4World._42 = m_xmf3Position.y;	m_xmf4x4World._43 = m_xmf3Position.z;

}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) 
{
	 
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00; 
	 
	 //ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) 
		CGameObject::Render(pd3dCommandList, pCamera);
	
	for (int i = 0; i < 50; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i];
}

CAirplanePlayer::CAirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void* pContext, int nMeshes) : CPlayer(nMeshes) 
{
	CMesh *pAirplaneMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f));
	
	SetMesh(0, pAirplaneMesh); 
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth() * 0.5f, pTerrain->GetLength() * 0.5f);
	//SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f, fHeight + 100.0f, pTerrain->GetLength() * 0.5f));
	SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f, fHeight + 100.0f, pTerrain->GetLength() * 0.5f));

	SetPlayerUpdatedContext(pTerrain);

	SetCameraUpdatedContext(pTerrain);

	CCubeMeshDiffused* pBulletMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1.0f, 4.0f, 1.0f);
	for (int i = 0; i < 50; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange, 1);
		m_ppBullets[i]->SetMesh(0, pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(120.0f);
		m_ppBullets[i]->SetActive(false);
	}

	CPlayerShader *pShader = new CPlayerShader();
	
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

}

CAirplanePlayer::~CAirplanePlayer() 
{ 
	for (int i = 0; i < 50; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CAirplanePlayer::OnPrepareRender() {

	CPlayer::OnPrepareRender();
	//����� ���� �׸��� ���� x-������ 90�� ȸ���Ѵ�. 

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f); 
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

} 
CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { 

	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00; 

	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera); 

	switch (nNewCameraMode) { 
	case FIRST_PERSON_CAMERA: 
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f); 

		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode); 
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f)); 
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break; 

	case SPACESHIP_CAMERA:
		SetFriction(125.0f); 
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f)); 
		SetMaxVelocityXZ(400.0f); 
		SetMaxVelocityY(400.0f);

		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f); 
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f)); 
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
		break; 

	case THIRD_PERSON_CAMERA: 
		SetFriction(250.0f); 
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f)); 
		SetMaxVelocityXZ(400.0f); 
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode); 
		
		m_pCamera->SetTimeLag(0.25f); 
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f); 
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); 
		break; 

	default: 
		break; 
	} 
	Update(fTimeElapsed);

	return(m_pCamera);

}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CAirplanePlayer::FireBullet(CGameObject* pLockedObject)
{
	
	if (pLockedObject)
	{
		LookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		OnUpdateTransform();
	}
	

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
CHeightMapTerrain* pMap;
void CAirplanePlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{

	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	pMap = pTerrain;
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;


	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}

}

void CAirplanePlayer::OnCameraUpdateCallback(float fTimeElapsed) {

	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	/*
		���� �ʿ��� ī�޶��� ���� ��ġ (x, z)�� ���� ������ ����(y ��)�� ���Ѵ�.
		�� ���� ī�޶��� ��ġ ������ y-�� �� �� ũ�� ī�޶� ������ �Ʒ��� �ְ� �ȴ�.
		�̷��� �Ǹ� ���� �׸��� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻� �Ѵ�(ī�޶� ���� �ȿ� �����Ƿ� �ﰢ���� ���ε� ������ �ٲ��).
		�̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ �� ���� y-���� �ּҰ��� (������ ���� + 5)�� �����Ѵ�.
		ī�޶��� ��ġ ������ y-���� �ּҰ��� ������ ��� ��ġ���� ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����ؾ� �Ѵ�.
	*/
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			//3��Ī ī�޶��� ��� ī�޶� ��ġ(y-��ǥ)�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�. 
			CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}


}


/////////////////////////////////////////////////////////////////////////////////

CTerrainPlayer::CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CPlayer(nMeshes) 
{
	
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	CTerrainPlayer::OnPrepareRender();

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth()*0.5f, pTerrain->GetLength()*0.5f); 
	SetPosition(XMFLOAT3(pTerrain->GetWidth()*0.5f, fHeight + 50.0f, pTerrain->GetLength()*0.5f)); 
	
	SetPlayerUpdatedContext(pTerrain); 
	
	SetCameraUpdatedContext(pTerrain);

	CMesh* pAirplaneMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f));

	SetMesh(0, pAirplaneMesh); 

	//�÷��̾ �������� ���̴��� �����Ѵ�. 
	CPlayerShader *pShader = new CPlayerShader(); 
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature); 
	SetShader(pShader);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

CTerrainPlayer::~CTerrainPlayer() 
{ 

}

void CTerrainPlayer::OnPrepareRender()
{

	CPlayer::OnPrepareRender();
	//����� ���� �׸��� ���� x-������ 90�� ȸ���Ѵ�. 

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

}

CCamera *CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) 
{

	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00; 
	if (nCurrentCameraMode == nNewCameraMode) 
		return(m_pCamera); 

	switch (nNewCameraMode) {

	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f); 
		//1��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�. 
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f)); 
		SetMaxVelocityXZ(300.0f); 
		SetMaxVelocityY(400.0f); 
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode); 
		m_pCamera->SetTimeLag(0.0f); 
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f)); 
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f); 
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

	case SPACESHIP_CAMERA: 
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break; 
	
	case THIRD_PERSON_CAMERA: 
		SetFriction(250.0f); 
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f); 
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode); 
		m_pCamera->SetTimeLag(0.25f); 
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f); 
		break; 
	
	default: 
		break; 
	} 
	Update(fTimeElapsed);
		
	return(m_pCamera);
	
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed) 
{
	
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
	
	if (xmf3PlayerPosition.y < fHeight)
	{ 
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity(); 
		xmf3PlayerVelocity.y = 0.0f; 
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight; 
		SetPosition(xmf3PlayerPosition); 
	}

}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed) 
{

	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight) 
	{
		xmf3CameraPosition.y = fHeight; 
		m_pCamera->SetPosition(xmf3CameraPosition); 
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA) 
		{ 
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera; 
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}


}