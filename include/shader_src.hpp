#pragma once

const char *const VERTEX_SHADER_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);        
}
)";
const char *const COLOR_VERTEX_SHADER_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);        
  ourColor = aColor;        
}
)";

const char *const ORANGE_FRAG_SHADER_SRC = R"(
#version 330 core
out vec4 FragColor;
void main() {
  FragColor = vec4(196.0f / 255, 178.0f / 255, 138.0f / 255, 1.0f);
}
)";
