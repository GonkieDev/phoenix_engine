// PHOENIX.ObjectShader.vert.glsl

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) in vec3 inPos;

void main()
{
    gl_Position = vec4(inPos, 1.0f);
}
