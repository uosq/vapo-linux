#pragma once

class CBaseEntity;

static float (*attribute_hook_value_float_original)(float, const char*, CBaseEntity*, void*, bool);

class AttributeManager {
public:
	float AttributeHookValue(float defaultValue, const char* attrib_name, CBaseEntity* entity) {
		return attribute_hook_value_float_original(defaultValue, attrib_name, entity, nullptr, true);
	}  
};