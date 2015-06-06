#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

//Include the DirectXTK
#include "DirectXHelpers.h"
#include "SpriteBatch.h"
#include "WICTextureLoader.h"
#include "SimpleMath.h"
#include "CommonStates.h"

#include <DirectXMath.h>

#define MOVE_CAT_PERIOD 2.f
#define DEFAULT_TILE_MOVE false

namespace DirectX2DPlayground
{
	class KatiesCollisionRenderer
	{
	public:
		KatiesCollisionRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void UpdateMouseInputState(float x, float y) { m_mouseX = x; m_mouseY = y; }
		bool IsAlive() { return m_isAlive; }
		void DrawDebugRect(bool drawRect) { m_drawRect = drawRect; }


	private:
		void UpdateCatRect();
		//void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		//DirectXTK objects

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tile;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_rectBrush;
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
		std::unique_ptr<DirectX::CommonStates> m_states;
		DirectX::SimpleMath::Vector2 m_screenPos;
		DirectX::SimpleMath::Vector2 m_origin;
		CD3D11_TEXTURE2D_DESC m_tileDesc;

		D2D1_RECT_F m_tileRect;
		RECT m_tileMapRect;

		float m_secondCounter;
		float m_mouseX;
		float m_mouseY;

		//Is the cat still alive??
		bool m_isAlive;
		bool m_drawRect;
		bool m_shouldMoveTile;
	};
}