#pragma once

#include "system.h"
#include "debug.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

#include "material.h"
#include "texture.h"


class ModuleMaterial : public Module
{
	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto mts = Entity->Get<StorageMaterial>();
		for(auto i = mts.begin(); i != mts.end(); ++i)
		{
			if(i->second->Changed)
			{
				Load(i->first);
				i->second->Changed = false;
			}
		}
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto mts = Entity->Get<StorageMaterial>();
			int Count = 0;
			for(auto i = mts.begin(); i != mts.end(); ++i)
			{
				bool Changed = true; // check if the file actually changed
				if(Changed)
				{
					i->second->Changed = true;
					Count++;
				}
			}
			if(Count > 0)
			{
				Debug::Info("Materials reloaded " + to_string(Count));
			}
		});
	}

	void Load(unsigned int Id)
	{
		auto mat = Entity->Get<StorageMaterial>(Id);

		ifstream stream(Name() + "/" + mat->Path);
		if(!stream.is_open())
		{
			Debug::Fail("Material (" + mat->Path + ") cannot be loaded.");
			return;
		}

		string Name, Diffuse, Normal, Specular;

		string line;
		while(getline(stream, line))
		{
			istringstream input(line);
			string key;
			input >> key;

			if(key == "") continue;
			else if(key == "newmtl"  ) input >> Name;
			else if(key == "map_Kd"  ) input >> Diffuse;
			else if(key == "map_Bump") input >> Normal;
			else if(key == "map_Ns"  ) input >> Specular;
		}

		if(Diffuse  != "") mat->Diffuse  = Texture(Diffuse );
		if(Normal   != "") mat->Normal   = Texture(Normal  );
		if(Specular != "") mat->Specular = Texture(Specular);
	}

	unsigned int Texture(string Path)
	{
		auto txs = Entity->Get<StorageTexture>();
		for(auto i : txs)
			if(i.second->Path == Path)
				return i.first;

		unsigned int id = Entity->New();
		auto tex = Entity->Add<StorageTexture>(id);
		tex->Path = Path;
		return id;
	}
};