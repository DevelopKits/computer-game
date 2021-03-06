#include "module.h"

#include <unordered_map>
#include <cstdlib>
#include <glew/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <sfml/Window/Keyboard.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include "type/model/type.h"
#include "type/form/type.h"
#include "type/print/type.h"
#include "type/light/type.h"

using namespace std;
using namespace sf;
using namespace glm;

void ModuleModel::Init()
{
	Listeners();
	Callbacks();

	Entity->Add<Print>(Entity->New())->Text = [=]{
		auto fms = Entity->Get<Model>();
		return "Forms     " + to_string(fms.size()      ) + '\n'
			 + "Meshes    " + to_string(meshes.size()   ) + '\n'
			 + "Materials " + to_string(materials.size()) + '\n'
			 + "Textures  " + to_string(textures.size() ) + '\n'
			 + "Shapes    " + to_string(shapes.size()   );
	};

	Script->Run("init.js");
}

void ModuleModel::Update()
{
	Script->Run("update.js");
}

void ModuleModel::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		ReloadMeshes();
		ReloadMaterials();
		ReloadTextures();
		ReloadShapes();
	});
}
