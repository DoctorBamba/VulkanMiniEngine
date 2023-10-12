#include "UiWidget.h"

CBasicAllocator* UiWidget::WidgeteAllocator = new CBasicAllocator(UI_MAXIMUM_NUMBER_OF_WIDGETS);

UiWidget::UiWidget(UiWidget* parent_) : m_Id(UiWidget::WidgeteAllocator->AllocateElement())
{
	//Set start-value...
	m_Name				= "New_Control";
	m_Location			= UiLocation();
	m_Width				= 0;
	m_Height			= 0;
	m_Angle				= 0.0f;
	m_Depth				= 0.0f;
	m_Visibale			= false;

	p_Parent			= parent_;
}

UiWidget* UiWidget::GetParent()
{
	return p_Parent;
}

Uint16 UiWidget::GetWidth()
{
	return m_Width;
}

Uint16 UiWidget::GetHeight()
{
	return m_Height;
}

Uint16 UiWidget::GetDepth()
{
	return m_Depth;
}


Float UiWidget::GetAngle()
{
	return m_Angle;
}


UiCoords UiWidget::GetDimensions()
{
	return UiCoords(m_Width, m_Height);
}

UiCoords UiWidget::GetLocalPosition()
{
	if (p_Parent != nullptr)
		return UiCoords(m_Location.m_pivot[0] * p_Parent->GetWidth(), m_Location.m_pivot[1] * p_Parent->GetHeight()) + m_Location.m_offset;

	return m_Location.m_offset;
}

UiCoords UiWidget::GetPosition()
{
	UiCoords position;
	UiWidget* widget = this;

	while (widget != nullptr)
	{
		position += widget->GetLocalPosition();
		widget = widget->GetParent();
	}

	return position;
}