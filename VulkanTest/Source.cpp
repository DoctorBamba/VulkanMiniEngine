#include "CApplication.h"
#include "Engine/Engine_Events.h"
#include "Graphics/Buffers/CVertexBuffer.h"
#include "Scene/CScene.h"
#include "Scene/Planet/CPlanet.h"
#include "EUI/UiWidgetRenderer.h"

CApplication* Application;

int main()
{
	Application = new CApplication();
	
	return Application->Run();
}


class ControledCamera : public CPersCamera
{
	private:
		Float alpha;
		Float teta;

		const Float MovementSpeed	= 0.02;
		const Float MouseSensetive	= 0.001f;

		Bool space_realesed = true;
		Byte frese_look = 1;

		Vector3D position;
	public:

		ControledCamera() : CPersCamera()
		{
			name = "ControledCammera";
			SetUp(Vector3D(0.0f, 0.0f, 1.0f));
			SetZlimits(0.2f, 100.0f);
			SetSolidAngles(1.2, 1.2);

			alpha = 0.0f;
			teta  = 1.0f;

			position  = Vector3D(0.0, 0.0f, 120.0f);
			direction = Vector3D(-200.0f, 0.0f, 0.0f);
		}
		
		Void Update() override
		{
			CWindow* root_window = Application->GetRootWindow();

			Coordinates client_center = root_window->GetClientDimantions() * 0.5f;
			Coordinates mouse_coords  = root_window->GetCursorPosition();

			Int16 delta_x = mouse_coords[0] - client_center[0];
			Int16 delta_y = mouse_coords[1] - client_center[1];

			alpha -= MouseSensetive * (Float)delta_x;
			teta  += MouseSensetive * (Float)delta_y;

			if (teta < 0.001)
				teta = 0.001;
			if (teta > PI - 0.001)
				teta = PI - 0.001;

			if (root_window->IsKeyPressed(GLFW_KEY_W))
				position += MovementSpeed * Vector3D(cos(alpha), sin(alpha), 0.0f);
			if (root_window->IsKeyPressed(GLFW_KEY_S))
				position -= MovementSpeed * Vector3D(cos(alpha), sin(alpha), 0.0f);

			if (root_window->IsKeyPressed(GLFW_KEY_D))
				position += MovementSpeed * Vector3D(sin(alpha), -cos(alpha), 0.0f);
			if (root_window->IsKeyPressed(GLFW_KEY_A))
				position -= MovementSpeed * Vector3D(sin(alpha), -cos(alpha), 0.0f);

			if (root_window->IsKeyPressed(GLFW_KEY_E))
				position += MovementSpeed * Vector3D(0.0f, 0.0f, 1.0f);
			if (root_window->IsKeyPressed(GLFW_KEY_Q))
				position -= MovementSpeed * Vector3D(0.0f, 0.0f, 1.0f);

			if (root_window->IsKeyPressed(GLFW_KEY_SPACE) && space_realesed)
			{
				frese_look = (frese_look + 1) % 2;
				space_realesed = false;
			}

			if (!root_window->IsKeyPressed(GLFW_KEY_SPACE))
				space_realesed = true;

			if (!frese_look)
			{
				root_window->SetCursorPosition(client_center);
				direction = position + Vector3D(cos(alpha) * sin(teta), sin(alpha) * sin(teta), cos(teta));
			}

			offset_transform[0][3] = position[0];
			offset_transform[1][3] = position[1];
			offset_transform[2][3] = position[2];
		};
};

CScene* Scene;
ControledCamera* Camera;
CArmature* doric;
CSpotLight* spot_light;

CPlanet* planet;

UiWidgetRenderer* UiRenderer;

Void Engine::Init_Event(CGpuUploadTask* upload_task_)
{
	//UiRenderer = new UiWidgetRenderer();

	Scene = new CScene(L"Example/scene.fbx", graphics);
	
	doric = dynamic_cast<CArmature*>(SearchObjectForward("Doric", Scene->root_object));

	Camera = new ControledCamera;
	Camera->SetUp(Vector3D(0.0f, 0.0f, 1.0f));
	Camera->SetZlimits(1.0f, 1000.0f);
	Camera->SetSolidAngles(1.2f, 1.2f);
	Scene->AddCamera(Camera, 0.0f);
	Scene->root_object->AddChild(Camera);
	

	planet = new CPlanet();
	Scene->root_object->AddChild(planet);

	spot_light = new CSpotLight();
	spot_light->SetOffsetTransform(Translate(Vector3D(300.0f, 300.0f, 300.0f)));
	spot_light->SetLookPoint(Vector3D(0.0f, 0.0f, 0.0f));
	spot_light->SetUp(Vector3D(0.0f, 0.0f, 1.0f));
	spot_light->SetDistance(300.0f);
	spot_light->SetFallout(0.0f, 0.0f, 1.0f);
	spot_light->SetColor(Vector4D(1.0f, 1.0f, 1.0f, 1.0f));

	Scene->AddLight(spot_light);

	Scene->CallObjectsInitEvent(upload_task_);
}

Float teta = 0.0f;

Void Engine::Draw_Event(CGpuDrawTask* draw_task_, CFrameBuffer* target_)
{
	Scene->CallObjectsUpdateEvent();

	Camera->SetFrameBuffer(target_);
	Camera->SetTime(teta);

	CAnimation* animation = Scene->GetAnimation("Doric|Box");
	doric->ApplyAnimation(animation, teta, LINEAR_INTERPOLATION_FUNC);

	//Camera->SetOffsetTransform(PE_Translate(PE_Vector3D(cos(teta) * 4.3f, sin(teta) * 4.3f, 4.5f)));
	

	renderer->Draw(draw_task_, Scene);


	teta += 0.0001f;
}

