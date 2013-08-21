#include "module.h"

#include <string>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "form.h"


void ModuleCamera::Load()
{
	auto stg = Global->Get<Settings>("settings");

	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = "data.txt";
	
	// for now, only load the first camera
	float data[8] = { 0 };
	if(Archive->Read(path, file, data))
	{
		unsigned int id = Entity->New();
		auto cam = Entity->Add<Camera>(id);
		auto frm = Entity->Add<Form>(id);

		cam->Pitch = data[0];
		frm->Position(vec3(data[1], data[2], data[3]));
		frm->Quaternion(quat(data[4], data[5], data[6], data[7]));
		frm->Body->setActivationState(DISABLE_DEACTIVATION);
	}
}

void ModuleCamera::Save()
{
	auto stg = Global->Get<Settings>("settings");
	auto cms = Entity->Get<Camera>();

	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = "data.txt";

	// for now, only save the first camera
	unsigned int id = cms.begin()->first;
	auto cam = Entity->Get<Camera>(id);
	auto frm = Entity->Get<Form>(id);
	
	vec3 position = frm->Position();
	quat rotation = frm->Quaternion();
	float data[8] = { cam->Pitch, position.x, position.y, position.z, rotation.w, rotation.x, rotation.y, rotation.z };

	bool result = Archive->Write(path, file, data, sizeof data);
	if(!result) Debug->Fail("save fail");
}
