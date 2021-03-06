#include "module.h"

#include <sfml/Window.hpp>
#include <sfml/Graphics/RenderWindow.hpp>
using namespace sf;

#include "type/camera/type.h"


void ModuleCamera::State()
{
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));
	State(cam->Active);
}

void ModuleCamera::State(bool Active)
{
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));
	auto wnd = Global->Get<RenderWindow>("window");

	cam->Active = Active;
	wnd->setMouseCursorVisible(!Active);

	if(Active) Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
}

#ifdef _WIN32
#include <Windows.h>
bool ModuleCamera::Focus()
{
	auto wnd = Global->Get<RenderWindow>("window");

	HWND handle = wnd->getSystemHandle();
	bool focus      = (handle == GetFocus());
	bool foreground = (handle == GetForegroundWindow());

	// focus window if in foreground.
	if(focus != foreground)
	{
		SetFocus(handle);
		SetForegroundWindow(handle);
	}

	return focus;
}
#else
bool ModuleCamera::Focus()
{
	return false;
}
#endif
