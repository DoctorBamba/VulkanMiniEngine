#pragma once

#include "../Workspace.h"
#include "../Graphics/CTexture2D.h"
#include "../Graphics/CAllocator.h"

#define UI_MAXIMUM_NUMBER_OF_WIDGETS (1 << 16)


typedef Vector2D__<Uint16> UiCoords;
typedef Uint UiWidgetId;

struct UiLocation
{
	UiCoords			m_offset;
	Vector2D__<Float>	m_pivot;
};


class UiWidgetRenderer;

//The next class is the control-class of the EUI-GUI...
class UiWidget
{
	friend UiWidgetRenderer;

	public:
		static CBasicAllocator* WidgeteAllocator;

	protected :

		//Basic members...
		std::string			m_Name;						//The name off the control
		UiLocation			m_Location;					//The control's position in parent-space(In pixels)
		Uint16				m_Width;					//The width of the control(In pixels)
		Uint16				m_Height;					//The height of the control(In pixels)
		Float				m_Depth;					//A depth-factor for determine the rendering's order(Control with hider m_Depth-value draw on those with lower m_Depth value)
		Float				m_Angle;					//The orintetion-angle of the control(In radians, counter-clock-wise, from the "right-axis"(x-axis))
		Bool				m_Visibale;					//The visibility of the control(Note - if visiblity is false the control's childrens isn't rendering too)
		Bool				m_ChildrenVisibility;		//The visibility of the control's children(Some meshod use to disable only the rendering of children for draw them at
														//themself. For exsample the CUI_View view control use this meshod for rendering his children in a new render target)
		
		UiWidgetId			m_Id;

		//Hirarhy...
		std::list<UiWidget*>	p_Children;					//A some child-control(Usually is the first child, but it's must by. For get the first-child use "CUI_Control::Get_FirstChild()")
		UiWidget*				p_Parent;					//The parent of the control
		
		CTexture2D*		m_MainTexture;

	public:

		UiWidget(UiWidget* parent_);

		//Hirarhy...
		UiWidget* GetParent();

		Uint16	GetWidth();
		Uint16	GetHeight();
		Uint16	GetDepth();
		Float	GetAngle();

		UiCoords GetDimensions();

		UiCoords GetLocalPosition();
		UiCoords GetPosition();

		std::list<UiWidget*>& GetChildrens() { return p_Children; }

		Uint GetWidgetId() { return m_Id; }

};