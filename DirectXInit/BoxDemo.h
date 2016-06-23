#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "DxAppBase.h"
#include "d3dUtil.h"
#include "MathHelper.h"
#include <stdint.h>
#include <tchar.h>
#include "d3dx11effect.h"
#include <xnamath.h>

//define our vertex structure

typedef struct BoxVertex
{
	//position
	XMFLOAT3 Pos;

	//color
	XMFLOAT4 Color;

} BoxVertex, *pBoxVertex;


//subclass our base dx framework
class BoxDemo : public DxAppBase
{
public:

	BoxDemo(HINSTANCE hWnd);
	virtual ~BoxDemo();

	//virtual methods to implement
	virtual bool InitApp();
	virtual bool OnResizeHandler();
	virtual void ProcSceneUpdate(float _dt);
	virtual void ProcSceneDraw();

	//Overrides for mouse input
	virtual void HandleMouseDown(WPARAM bState, int x, int y);
	virtual void HandleMouseUp(WPARAM bState, int x, int y);
	virtual void HandleMouseMove(WPARAM bState, int x, int y);


private:

	//Internal implementations

	//Build geometry buffers
	void BuildGeometryBuffers();
	//Build our effects (render states, vertex shaders, pixel shaders)
	void BuildFX();
	//Build our vertex layouts
	void BuildVertexLayout();

private:

	//Internal class fields
	ID3D11Buffer *pBoxVertexBuffer;
	ID3D11Buffer *pBoxIndexBuffer;

	//Handle to effects11 interface
	ID3DX11Effect *pFX;
	//handle to effects technique
	ID3DX11EffectTechnique *pTech;
	//Handle to constant buffer variable for world-view-projection matrix for box
	ID3DX11EffectMatrixVariable *pfxWorldViewProj;

	//Handle to our input layout for our vertex type
	ID3D11InputLayout* pInputLayout;

	//class fields for world, view, projection matrices
	XMFLOAT4X4 mWorld;	//local space to world space matrix for our box vertices (wont change)
	XMFLOAT4X4 mView;	//World space to view space matrix (will change as camera position and orientation changes)
	XMFLOAT4X4 mProj;	//View space to homogenous clip space matrix (may change if aspect ratio of window changes)


	//spherical coordinates for camera
	float mTheta; //angle in xz plane from
	float mPhi;	  //angle for y axis with "hinge" at origin, in plane created by the angle theta and y axis
	float mRadius; //length in meters of radius, measured by ray from origin along phi

	POINT mLastMousePos;	//last position of mouse right after they click in window

};