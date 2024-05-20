#include "CameraController.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../../MathHelpers.h"
#include "../../../Engine/Renderer/WindowManager.h"
#include "../../../../Source/Engine/System/Context/SysContextProvider.h"

void CameraController::Initialise()
{

	auto* pWindowManager = C_SysContext::Get<WindowManager>();
	view = pWindowManager->GetWindow()->getDefaultView();

}

void CameraController::Update()
{

	sf::Vector2f position = sf::Vector2f(0, 0);

	if (targetMode == TARGET_OBJECT)
	{

		if (targetObject != nullptr)
		{
			position = targetObject->GetPosition();
		}

	}
	else
	{

		position = targetPosition;

	}

	//Screenshake
	offsetPosition.x = RandomRangeDouble(-screenshakeAmount, screenshakeAmount);
	offsetPosition.y = RandomRangeDouble(-screenshakeAmount, screenshakeAmount);
	screenshakeAmount *= screenshakeFade;

	//Lerp the view to the target position
	viewPosition.x = Lerp(viewPosition.x, position.x, 0.1);
	viewPosition.y = Lerp(viewPosition.y, position.y, 0.1);

	//Set the view to the window manager's view
	view.setCenter(viewPosition + offsetPosition);
	auto* pWindowManager = C_SysContext::Get<WindowManager>();
	pWindowManager->GetWindow()->setView(view);

}

void CameraController::SetTargetMode(TARGET_MODE _targetMode)
{

	targetMode = _targetMode;

}

void CameraController::SetTargetObject(GameObject* _object)
{

	targetObject = _object;

}

void CameraController::SetTargetPosition(sf::Vector2f _position)
{

	targetPosition = _position;

}

void CameraController::ScreenShake(double _amount)
{

	screenshakeAmount = _amount;

}

sf::Vector2f CameraController::GetPosition()
{
	return sf::Vector2f();
}

void CameraController::SetPosition(float px, float py)
{
}

void CameraController::SetPosition(sf::Vector2f ppos)
{
	view.setCenter(ppos + offsetPosition);
}

void CameraController::GetPosition(float& x, float& y)
{
}
