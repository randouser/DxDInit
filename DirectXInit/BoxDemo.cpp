#include <Windows.h>
#include <d3d11.h>
#include "DxAppBase.h"
#include "d3dUtil.h"
#include "MathHelper.h"
#include <stdint.h>
#include <tchar.h>
#include "d3dx11effect.h"
#include <xnamath.h>
#include "BoxDemo.h"

BoxDemo::BoxDemo(HINSTANCE hWnd)
	: DxAppBase(hWnd), pBoxVertexBuffer(NULL), pBoxIndexBuffer(NULL), pFX(NULL), pTech(NULL),
	pfxWorldViewProj(NULL), pInputLayout(NULL), mTheta(1.5f * MathHelper::Pi), mPhi(0.25f * MathHelper::Pi), mRadius(5.0f)
{

	strMainWindowCaption = _T("Box Demo");

	mLastMousePos.x = mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();

	//set our world, view, proj matrices to identity
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}

BoxDemo::~BoxDemo()
{

	ReleaseCOM(pBoxVertexBuffer);
	ReleaseCOM(pBoxIndexBuffer);
	ReleaseCOM(pFX);
	ReleaseCOM(pInputLayout);
}


bool BoxDemo::InitApp()
{

	if (!DxAppBase::InitApp())
	{
		return false;
	}

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	return true;

}


bool BoxDemo::OnResizeHandler()
{

	if (!DxAppBase::OnResizeHandler())
		return false;

	//Window resized, update aspect ratio, recompute the projection matrix

	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, CurAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, p);
	return true;
}

void BoxDemo::ProcSceneUpdate(float dt)
{

	//Convert spherical coords to radial
	// (radius * sin(mPhi) = hypotenus of triangle in xz plane from mTheta)
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	//build view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}



void BoxDemo::ProcSceneDraw()
{

	//lock manager
	_dxMgr.LockMgr();

	//get current device context

	ID3D11DeviceContext *pContext = _dxMgr.CurrentDeviceContext();

	//Clear back buffer blue
	pContext->ClearRenderTargetView(_dxMgr.CurrentRenderTargetView(), reinterpret_cast<const float*>(&Colors::Blue));

	//clear depth and stencil buffer
	pContext->ClearDepthStencilView(_dxMgr.CurrentDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//set the input layout
	pContext->IASetInputLayout(pInputLayout);

	//set topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(BoxVertex);
	UINT offset = 0;

	//set vertex buffer
	pContext->IASetVertexBuffers(0, 1, &pBoxVertexBuffer, &stride, &offset);

	//set index buffer
	pContext->IASetIndexBuffer(pBoxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	//set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX worldViewProj = world*view*proj;

	//update the constant buffer wvp matrix for vertex shader
	pfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	//get a handle to the technique description
	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{

		//get pass by index... update constant buffer in video memory, bind shaders to pipeline
		pTech->GetPassByIndex(p)->Apply(0, pContext);

		//Draw the 36 indicies into the vertex buffer to the back buffer
		pContext->DrawIndexed(36, 0, 0);

	}

	//flip the back buffer and... front buffer?
	HR(_dxMgr.CurrentSwapChain()->Present(0,0));

	//unlock mgr
	_dxMgr.UnlockMgr();

}


void BoxDemo::HandleMouseDown(WPARAM bState, int x, int y)
{

	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(this->handleMainWindow);
}

void BoxDemo::HandleMouseUp(WPARAM bState, int x, int y)
{
	ReleaseCapture();
}


void BoxDemo::HandleMouseMove(WPARAM bState, int x, int y)
{
	
	if (bState & MK_LBUTTON != 0)
	{

		//make each pixel correspond to a quarter of a degree
		float dx = XMConvertToRadians( 0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians( 0.25f*static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around box
		mTheta += dx;
		mPhi += dy;

		//restrict angle mPhi
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if (bState & MK_RBUTTON != 0)
	{

		//adjust radius
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

		//update camera radius based on input
		mRadius += dx - dy;

		//restrict the radius
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;

}
















