#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "settings.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "light.h"


void ModuleRenderer::Init()
{
	Opengl->Init();
	Pipeline();
	Uniforms();
	Listeners();
}

void ModuleRenderer::Update()
{
	Forms(GetPass("form"));

	Light(GetPass("light"));

	for(uint i = 2; i < passes.size() - 1; ++i)
		Quad(&passes[i].second);

	Quad(&passes.back().second, true);
}

void ModuleRenderer::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			// check whether file actually changed
			glDeleteProgram(i->second.Shader); // simply do this in CreateProgram every time?
			i->second.Shader = CreateProgram(i->second.Vertex, i->second.Fragment);
		}

		Uniforms();
	});

	Event->Listen("WindowRecreated", [=]{
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			glDeleteFramebuffers(1, &i->second.Framebuffer); // simply do this in SetupFramebuffer every time?
			i->second.Framebuffer = CreateFramebuffer(i->second.Targets, i->second.Samplers, i->second.Size);
		}

		Uniforms();
	});

	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		for(auto i : passes)
			for(auto j : i.second.Targets)
				TextureResize(j.second.first, j.second.second, Vector2u(Vector2f(Size) * i.second.Size));

		Uniforms();
	});

	Event->Listen("ShaderUpdated", [=]{
		Uniforms();
	});
}
