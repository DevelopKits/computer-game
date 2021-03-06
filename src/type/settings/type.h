#pragma once

#include "system/system.h"

#include <string>
#include <unordered_map>
#include <typeindex>
#include <sfml/System.hpp>


struct Settings
{
	template <typename T>
	void Set(std::string Key, T Value)
	{
		auto i = list.find(Key);
		auto entry = std::make_pair((void*)(new T(Value)), std::type_index(typeid(T)));
		if (i == list.end()) {
			// create new
			list.insert(std::make_pair(Key, entry));
		} else {
			// change existing
			delete i->second.first;
			i->second = entry;
		}
	}
	template <typename T>
	T *Get(std::string Key)
	{
		auto i = list.find(Key);
		if (i == list.end()) {
			// setting not found
			ManagerLog::Fail("settings", "cannot get because (" + Key + ") does not exist");
			return nullptr;
		}
		if (i->second.second != std::type_index(typeid(T))) {
			// type doesn't match
			ManagerLog::Fail("settings", "cannot get because (" + Key + ") type doesn't match");
			return nullptr;
		}
		return static_cast<T*>(i->second.first);
	}
	bool Is(std::string Key)
	{
		bool *result = Get<bool>(Key);
		if (!result) return false;
		return *result;
	}
private:
	std::unordered_map<std::string, std::pair<void*, std::type_index>> list;
};
