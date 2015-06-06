#include "pch.h"
#include "KatysCollisionRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Windows::Foundation;
using namespace DirectX2DPlayground;
using namespace Microsoft::WRL;

KatiesCollisionRenderer::KatiesCollisionRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_mouseX(-1.0f),
	m_mouseY(-1.0f),
	m_isAlive(true),
	m_drawRect(false),
	m_shouldMoveTile(DEFAULT_TILE_MOVE)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void KatiesCollisionRenderer::CreateWindowSizeDependentResources()
{
	Size size = m_deviceResources->GetOutputSize();

	m_screenPos.x = size.Width / 2.f;
	m_screenPos.y = size.Height / 2.f;

	UpdateCatRect();
}

void KatiesCollisionRenderer::CreateDeviceDependentResources()
{
	ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
		m_deviceResources->GetD3DDevice(),
		L"tile.png",
		resource.GetAddressOf(),
		m_tile.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> cat;

	m_states.reset(new DirectX::CommonStates(m_deviceResources->GetD3DDevice()));

	DX::ThrowIfFailed(resource.As(&cat));
	cat->GetDesc(&m_tileDesc);

	m_tileMapRect.left = 0;
	m_tileMapRect.right = m_tileDesc.Width * 8;
	m_tileMapRect.top = 0;
	m_tileMapRect.bottom = m_tileDesc.Height;


	m_origin.x = float(m_tileDesc.Width * 4);
	m_origin.y = float(m_tileDesc.Height / 2);

	m_spriteBatch.reset(new DirectX::SpriteBatch(m_deviceResources->GetD3DDeviceContext()));

	DX::ThrowIfFailed(m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red, 1.0f), m_rectBrush.GetAddressOf()));
}
void KatiesCollisionRenderer::ReleaseDeviceDependentResources()
{
	m_tile.Reset();
	m_spriteBatch.reset();
	m_rectBrush.Reset();
	m_states.reset();
}

void KatiesCollisionRenderer::UpdateCatRect()
{
	m_tileRect = D2D1::RectF(m_screenPos.x - (m_tileDesc.Width / 2.f),
		m_screenPos.y - (m_tileDesc.Height / 2.f),
		m_screenPos.x + (m_tileDesc.Width / 2.f),
		m_screenPos.y + (m_tileDesc.Height / 2.f));
}

void KatiesCollisionRenderer::Update(const DX::StepTimer &timer)
{
	if (m_isAlive && m_shouldMoveTile)
	{
		if ((m_mouseX >= 0.f && m_mouseY >= 0.f) &&
			(m_mouseX >= m_tileRect.left && m_mouseX <= m_tileRect.right) &&
			(m_mouseY >= m_tileRect.top && m_mouseY <= m_tileRect.bottom))
		{
			m_isAlive = false;
		}

		Size size = m_deviceResources->GetOutputSize();
		m_secondCounter += timer.GetElapsedSeconds();

		if (m_secondCounter > MOVE_CAT_PERIOD)
		{
			//use screen dimensions - 5%
			int moveAreaX = int(size.Width - (size.Width * 0.05));
			int moveAreaY = int(size.Height - (size.Height * 0.05));

			m_screenPos.x = float(rand() % moveAreaX);
			m_screenPos.y = float(rand() % moveAreaY);

			m_secondCounter = 0;
		}

		UpdateCatRect();
	}
}

void KatiesCollisionRenderer::Render()
{
	if (m_isAlive)
	{
		//D2D1::Matrix3x2F::Skew();
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, m_states->LinearWrap());
		m_spriteBatch->Draw(m_tile.Get(), m_screenPos, &m_tileMapRect, DirectX::Colors::White, DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4, m_origin);
		m_spriteBatch->End();


		if (m_drawRect)
		{
			ComPtr<ID2D1DeviceContext> context = m_deviceResources->GetD2DDeviceContext();

			context->BeginDraw();

			context->DrawRectangle(
				m_tileRect,
				m_rectBrush.Get());

			context->EndDraw();
		}
	}
}