#include "vars.h"

#include "application.h"
#include <stdlib.h>

HotloadedVariables *hotloaded_vars = nullptr;

// Macros since C++ makes this parsing thing hell.

// section:    Refers to the struct that lives here in the code (check vars.h).
// name:       Refers to the variable that is typed within the hotloaded-tweak file.
// target_var: Directly points to the associated member within the struct.
// type_enum:  The type of the member declared within the struct.
#define Attach(section_name, var_name, target_var, type_enum) \
    array_add(&hv->bindings, VarBinding { \
        string_literal_create(section_name), \
        string_literal_create(var_name), \
        (void *)&(target_var), \
        type_enum \
        } \
    );

static const char *get_cstr_from_line(String line) {
	Arena *scratch = get_current_arena_frame();
	StringBuilder sb = {};
	strbuild_append_string(scratch, &sb, line);

	return string_to_cstr(strbuild_terminate(scratch, &sb));
}

static bool parse_int(String string, s32 *result) {
	if (string_is_empty(string)) return false;
	
	const char *s = get_cstr_from_line(string);
	char *end;
	s64 value = strtol(s, &end, 10); // 10 = Decimal.
	if (end == s) return false;
	
	*result = (s32)value;
	return true;
}

static bool parse_float(String string, float32 *result) {
    if (string_is_empty(string)) return false;

	const char *s = get_cstr_from_line(string);
	char *end;
	float value = strtof(s, &end);
	*result = (float32)value;
	return true;
}

static bool parse_bool(String string, bool *result) {
	if (string_is_empty(string)) return false;

	if (string_comp(string, string_literal_create("false")) || string_comp(string, string_literal_create("0"))) {
		*result = false;
	} else if (string_comp(string, string_literal_create("true")) || string_comp(string, string_literal_create("1"))) {
		*result = true;
	}
	return true;
}

static bool parse_string(String string, String *result, Arena *arena) {
	if (string_is_empty(string)) return false;
	
	*result = string_copy(arena, string);
	return true;
}
   
void init_vars(HotloadedVariables *hv) {
	hotloaded_vars = hv;
	arena_init(&hv->string_arena, VARS_STRING_ARENA_SIZE);

	Arena *arena = get_permanent_arena();
    array_init(&hv->bindings, arena, MAX_VAR_BINDINGS);

	Attach("Display", "fullscreen", hv->display.fullscreen, VAR_TYPE_BOOL);
	Attach("Display", "vsync",      hv->display.vsync,      VAR_TYPE_BOOL);

	Attach("Audio", "master_volume", hv->audio.master_volume, VAR_TYPE_FLOAT32);

	Attach("Dev", "debug",               hv->dev.debug,               VAR_TYPE_BOOL);
	Attach("Dev", "console_openness_dt", hv->dev.console_openness_dt, VAR_TYPE_FLOAT32);
	Attach("Dev", "console_font_size",   hv->dev.console_font_size,   VAR_TYPE_FLOAT32);

	StringBuilder sb = {};
	strbuild_append_cstring(arena, &sb, get_data_path());
	strbuild_append_cstring(arena, &sb, "hotloaded.variables");
	hv->path = strbuild_terminate(arena, &sb);

    reload_vars(hv);
}

void reload_vars(HotloadedVariables* hv) {
	arena_reset(&hv->string_arena);

	FILE *file = fopen(hv->path.data, "r");
	if (!file) {
		fprintf(stderr, "[INIT_VARS]: ERROR :: Could not load file from path: %s\n", hv->path.data);
		return;
	}

	Arena *scratch = get_current_arena_frame();
	String current_section = {};
	String current_line    = {};

	char buffer[VARS_LINE_BUFFER_SIZE];
	s32 line_number = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		line_number += 1;

		current_line = string_create(scratch, buffer);
		s64 newline = string_find_char_reverse(current_line, '\n'); 
		if (newline >= 0) {
			current_line.data[newline] = '\0';
			current_line.length = (u64)newline;
		}

		// Trim whitespaces to the left and right.
		current_line = string_trim_left(current_line);
		current_line = string_trim_right(current_line);

		// Skip comments and empty lines.
		if (current_line.data[0] == '#' || current_line.data[0] == '\0' || current_line.data[0] == '\n') {
			continue;
		}

		// Check for section headers.
		if (current_line.data[0] == ':' && current_line.data[1] == '/') {
			current_section = string_view(current_line.data, current_line.length);
			current_section = string_slice(current_section, 2, current_section.length);
			current_section = string_trim_left(current_section); // Eat any whitespaces incase.

			// printf("\nEntering section: " string_fmt "\n", string_arg(current_section));
			continue;
		}
		
		// Parse the current line.
		const u32 MAX_TOKEN_COUNT = 3;
		String *tokens = push_array_to_arena(scratch, String, MAX_TOKEN_COUNT);
		s32 token_count = string_split_whitespace(current_line, tokens, MAX_TOKEN_COUNT);
		if (token_count != 3) {
			fprintf(stderr,	"ERROR :: Invalid syntax at line %d within data/hotloaded.variables.\n", line_number);
			printf("-> Token count mismatch! Parser expects the following structure <variable> = <value>\n");
			printf("-> We instead the received the following: %s\n\n", current_line.data);
			continue;
		} 
		
		String var    = string_view(tokens[0].data, tokens[0].length);
		String equals = string_view(tokens[1].data, tokens[1].length); 
		String value  = string_view(tokens[2].data, tokens[2].length);
		if (!string_comp(equals, string_literal_create("="))) {
			fprintf(stderr, "ERROR :: Expected '=' at line %d\n", line_number);
			printf("-> We instead received the following: %s\n\n", current_line.data);
			continue;
		}

		// Binding the variables' value to the structs.
		bool found = false;
        For (hv->bindings) {
            if (!string_comp(it->section, current_section)) continue;
            if (!string_comp(it->name, var))                continue;

            found = true;
            switch (it->type) {
            case VAR_TYPE_S32: {
				s32 parsed;
				if (parse_int(value, &parsed)) {
					*(s32 *)it->target = parsed;
				} else {
					fprintf(stderr, "WARNING :: Could not parse s32 for '%s' at line %d.\n", var.data, line_number);		
				}
				
            } break;

            case VAR_TYPE_FLOAT32: {
				float32 parsed;
				if (parse_float(value, &parsed)) {
					*(float32 *)it->target = parsed;
				} else {
					fprintf(stderr, "WARNING :: Could not parse float32 for '%s' at line %d.\n", var.data, line_number);		
				}
            } break;

            case VAR_TYPE_BOOL: {
                bool parsed;
                if (parse_bool(value, &parsed)) {
					*(bool *)it->target = parsed;
				} else {
					fprintf(stderr, "WARNING :: Could not parse bool for '%s' at line %d.\n", var.data, line_number);
				}
            } break;

            case VAR_TYPE_STRING: {
                String parsed;
                if (parse_string(value, &parsed, &hv->string_arena)) {
					*(String *)it->target = parsed;
				} else {
					fprintf(stderr, "WARNING :: Could not parse string for '%s' at line %d.\n", var.data, line_number);
				}
            } break;


            } break;
        }

        if (!found) {
            fprintf(stderr, "WARNING :: Unknown variable '%s' at section '%s' at line %d.\n", var.data, current_section.data, line_number);
        }

		// printf("Data parsed at line %d: Variable name: " string_fmt ", Value: " string_fmt "\n", line_number, string_arg(var), string_arg(value));
	}

	fclose(file);
}

void update_vars(HotloadedVariables *hv) {
	FileTimestamp current_timestamp = platform_get_file_timestamp(hv->path.data);
	if (current_timestamp != hv->last_modified) {
		Arena *scratch = get_current_arena_frame();
		String message = string_format(scratch, "[Hotloader] :: File modified at path " string_fmt, string_arg(hv->path));
		push_log(message, CONSOLE_LOG_INFO);
		
		hv->last_modified = current_timestamp;
		reload_vars(hv);
	}
}
