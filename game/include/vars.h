#ifndef VARS_H
#define VARS_H

#include "general.h"
#include "platform.h"

struct HotloadedVariables;
extern HotloadedVariables *hotloaded_vars;

struct Display {
	bool fullscreen;
	bool vsync;
};

struct Audio {
	float master_volume;
};

struct Dev {
	bool debug;
	
	float console_openness_dt;
	float console_font_size;
};

enum VarType {
	VAR_TYPE_S32,
	VAR_TYPE_FLOAT32,
	VAR_TYPE_BOOL,
	VAR_TYPE_STRING,
};

struct VarBinding {
	String section;
	String name;
	void  *target;
	
	VarType type;
};

struct HotloadedVariables {
	String path;
	FileTimestamp last_modified;

	Display display;
	Audio   audio;
	Dev     dev;

    Array<VarBinding> bindings;
	Arena string_arena; // This is here to make sure we don't leak strings every reload.
};

void init_vars(HotloadedVariables *hv);
void reload_vars(HotloadedVariables *hv);
void update_vars(HotloadedVariables *hv);

#endif
