#include "pch.h"
#include "SampleDirectXTKRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Windows::Foundation;
using namespace DirectX2DPlayground;
using namespace Microsoft::WRL;

SampleDirectXTKRenderer::SampleDirectXTKRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_mouseX(-1.0f),
	m_mouseY(-1.0f),
	m_isAlive(true),
	m_drawRect(false)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void SampleDirectXTKRenderer::CreateWindowSizeDependentResources()
{
	Size size = m_deviceResources->GetOutputSize();

	m_screenPos.x = size.Width / 2.f;
	m_screenPos.y = size.Height / 2.f;

	UpdateCatRect();
}

void SampleDirectXTKRenderer::CreateDeviceDependentResources()
{
	ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			m_deviceResources->GetD3DDevice(),
			L"cat.png",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> cat;

	DX::ThrowIfFailed(resource.As(&cat));
	cat->GetDesc(&m_catDesc);

	m_origin.x = float(m_catDesc.Width / 2);
	m_origin.y = float(m_catDesc.Height / 2);

	m_spriteBatch.reset(new DirectX::SpriteBatch(m_deviceResources->GetD3DDeviceContext()));

	DX::ThrowIfFailed(m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
							D2D1::ColorF(D2D1::ColorF::Red, 1.0f), m_rectBrush.GetAddressOf()));
}
void SampleDirectXTKRenderer::ReleaseDeviceDependentResources()
{
	m_texture.Reset();
	m_spriteBatch.reset();
	m_rectBrush.Reset();
}

void SampleDirectXTKRenderer::UpdateCatRect()
{
	m_catRect = D2D1::RectF(m_screenPos.x - (m_catDesc.Width / 2.f),
		m_screenPos.y - (m_catDesc.Height / 2.f),
		m_screenPos.x + (m_catDesc.Width / 2.f),
		m_screenPos.y + (m_catDesc.Height / 2.f));
}

void SampleDirectXTKRenderer::Update(const DX::StepTimer &timer)
{
	if (m_isAlive)
	{
		if ((m_mouseX >= 0.f && m_mouseY >= 0.f) &&
			(m_mouseX >= m_catRect.left && m_mouseX <= m_catRect.right) &&
			(m_mouseY >= m_catRect.top && m_mouseY <= m_catRect.bottom))
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

void SampleDirectXTKRenderer::Render()
{
	if (m_isAlive)
	{
		m_spriteBatch->Begin();
		m_spriteBatch->Draw(m_texture.Get(), m_screenPos, nullptr, DirectX::Colors::White, 0.f, m_origin);
		m_spriteBatch->End();


		if (m_drawRect)
		{
			ComPtr<ID2D1DeviceContext> context = m_deviceResources->GetD2DDeviceContext();

			context->BeginDraw();

			context->DrawRectangle(
				m_catRect,
				m_rectBrush.Get());

			context->EndDraw();
		}
	}
}