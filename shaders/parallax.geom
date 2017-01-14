#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in GS_IN
{
    flat vec3 position;
    flat vec3 normal;
    flat vec2 tex_coord;
} gs_in [];

out FS_IN
{
    smooth vec3 position;
    smooth vec3 normal;
    smooth vec2 tex_coord;
    flat vec3 origin;
    flat mat3x2 view_offset_basis;
} gs_out;

void main()
{
    mat2x3 view_offset_space = mat2x3(
        gs_in[1].position - gs_in[0].position,
        gs_in[2].position - gs_in[0].position
        ) * 
        inverse(mat2(
        gs_in[1].tex_coord - gs_in[0].tex_coord,
        gs_in[2].tex_coord - gs_in[0].tex_coord
        ));

    gs_out.origin = gs_in[0].position - view_offset_space * gs_in[0].tex_coord;
    gs_out.view_offset_basis = transpose(mat2x3(
        view_offset_space[0] / dot(
            view_offset_space[0],
            view_offset_space[0]
            ),
        view_offset_space[1] / dot(
            view_offset_space[1],
            view_offset_space[1]
            )
        ));

    gs_out.position = gs_in[0].position;
    gs_out.normal = gs_in[0].normal;
    gs_out.tex_coord = gs_in[0].tex_coord;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gs_out.position = gs_in[1].position;
    gs_out.normal = gs_in[1].normal;
    gs_out.tex_coord = gs_in[1].tex_coord;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gs_out.position = gs_in[2].position;
    gs_out.normal = gs_in[2].normal;
    gs_out.tex_coord = gs_in[2].tex_coord;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}
