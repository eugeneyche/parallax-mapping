#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;

uniform mat4 projection;
uniform mat4 view;

out GS_IN
{
    flat vec3 position;
    flat vec3 normal;
    flat vec2 tex_coord;
} vs_out;

void main()
{
    mat4 it_view = transpose(inverse(view));
    vs_out.position = vec3(view * vec4(position, 1));
    vs_out.normal = vec3(it_view * vec4(normal, 0));
    vs_out.tex_coord = tex_coord;
    gl_Position = projection * vec4(vs_out.position, 1);
}
