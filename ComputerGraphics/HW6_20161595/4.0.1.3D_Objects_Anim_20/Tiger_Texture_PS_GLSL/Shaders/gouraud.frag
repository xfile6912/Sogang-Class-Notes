#version 400
in vec3 v_position_EC;
in vec3 v_normal_EC;
in vec2 v_tex_coord;
in vec4 v_shaded_color;
layout (location = 0) out vec4 final_color;

void main(void) {
	final_color = v_shaded_color;
}
