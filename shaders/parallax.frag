#version 330 core

in FS_IN
{
    smooth vec3 position;
    smooth vec3 normal;
    smooth vec2 tex_coord;
    flat vec3 origin;
    flat mat3x2 view_offset_basis;
} fs_in;

uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D height_tex;
uniform float depth;

out vec4 fs_out;

void tex_coord_from_position(out vec2 tex_coord, in vec3 position)
{
    tex_coord = fs_in.view_offset_basis * (position - fs_in.origin);
}

void main()
{
    if (dot(fs_in.normal, vec3(0, 0, 1)) <= 0) {
        discard;
    }

    float height_scale = depth / dot(fs_in.normal, vec3(0, 0, 1));
    vec3 light_dir = normalize(fs_in.position);
    vec2 tex_coord = fs_in.tex_coord;

    float min_height = 0;
    float max_height = 1;

    int it = 5;
    int steps = 4;
    while (it-- > 0) {
        int i;
        for (i = 1; i <= steps; i++) {
            float height = max_height - i / float(steps) * (max_height - min_height);
            vec3 offset = (1 - height) * height_scale * light_dir;
            tex_coord_from_position(tex_coord, fs_in.position + offset);
            float tex_height = texture(height_tex, tex_coord).r;
            if (tex_height > height) break;
        }
        if (i > 1 && i < 5) {
            float new_max_height = max_height - (i - 1) / float(steps) * (max_height - min_height);
            float new_min_height = max_height - i / float(steps) * (max_height - min_height);
            max_height = new_max_height;
            min_height = new_min_height;
        } else break;
    }

    if (tex_coord.x <= 0 || tex_coord.x >= 1 || tex_coord.y <= 0 || tex_coord.y >= 1) discard;

    mat2x3 t_view_offset_basis = transpose(fs_in.view_offset_basis);

    mat3 normal_basis = mat3(
        normalize(t_view_offset_basis[0]),
        normalize(t_view_offset_basis[1]),
        fs_in.normal
        );
    vec3 normal = normal_basis * (texture(normal_tex, tex_coord).xyz - 0.5) * 2;
    float diffuse = clamp(dot(normal, vec3(0, 0, 1)), 0, 1);

    fs_out = diffuse * texture(diffuse_tex, tex_coord);
}
