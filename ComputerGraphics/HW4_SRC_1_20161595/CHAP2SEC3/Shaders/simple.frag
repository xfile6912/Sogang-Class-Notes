#version 330

in vec4 v_color;

layout (location = 0) out vec4 final_color;

void main(void) {
    final_color = v_color;
}
