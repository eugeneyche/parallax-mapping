#include "draw.hpp"
#include "image.hpp"
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int window_width = 800;
int window_height = 600;
const char* window_title = "Parallax Mapping";

struct Camera
{
    glm::mat4 view;
    glm::mat4 projection;
} cam;

glm::vec3 target;
float min_cam_dist = 3.f;
float max_cam_dist = 10.f;
float cam_dist = 5.f;
float rotate_x;
float rotate_y;
float depth = 0.2;

void update_view()
{
    glm::vec3 offset = cam_dist * glm::vec3{
        glm::cos(rotate_y) * glm::sin(rotate_x),
        glm::sin(rotate_y),
        glm::cos(rotate_y) * glm::cos(rotate_x)
    };
    cam.view = glm::lookAt(
        target + offset,
        target,
        glm::vec3{0.f, 1.f, 0.f}
        );
}

glm::vec3 cursor;

void on_scroll(GLFWwindow* window, double x, double y)
{
    double speed = 0.5f;
    cam_dist = glm::clamp(static_cast<float>(cam_dist - speed * y), min_cam_dist, max_cam_dist);
    update_view();
}

void on_cursor_pos(GLFWwindow* window, double x, double y)
{
    double speed = 0.01f;
    rotate_x = rotate_x + -speed * (x - cursor.x);
    float angle_extr = glm::pi<double>() / 2 - 0.1f;
    rotate_y = glm::clamp(static_cast<float>(rotate_y + speed * (y - cursor.y)), -angle_extr, angle_extr);
    cursor.x = x;
    cursor.y = y;
    update_view();
}

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            depth = glm::clamp(depth + 0.05f, 0.f, 1.f);
        }
        if (key == GLFW_KEY_S) {
            depth = glm::clamp(depth - 0.05f, 0.f, 1.f);
        }
    }
}

void on_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    if (button == 0) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double x, y;
            glfwGetCursorPos(window, &x, &y); 
            cursor.x = x;
            cursor.y = y;
            glfwSetCursorPosCallback(window, on_cursor_pos);
        } else if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPosCallback(window, nullptr);
        }
    } 
}

void make_grid(std::vector<VertPC>& grid, int size, const glm::vec3& color)
{
    float half_size = size / 2.f;
    for (int i = 0; i <= size; i++) {
        grid.push_back({glm::vec3{-half_size + i, 0, -half_size}, color});
        grid.push_back({glm::vec3{-half_size + i, 0,  half_size}, color});
        grid.push_back({glm::vec3{-half_size, 0, -half_size + i}, color});
        grid.push_back({glm::vec3{ half_size, 0, -half_size + i}, color});
    }
}

struct VertPNT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

struct Geometry
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei count;
};

void make_cube(Geometry& cube)
{
#define MAKE_FACE(v1, v2, v3, v4, normal)\
    {v1, normal, {0.f, 0.f}},\
    {v2, normal, {1.f, 0.f}},\
    {v3, normal, {1.f, 1.f}},\
    {v4, normal, {0.f, 1.f}}
#define MAKE_FACE_INDEX(i) i, i + 1, i + 2, i, i + 2, i + 3\

    std::vector<VertPNT> vertex_data = {
        MAKE_FACE(
            glm::vec3(-1.f,-1.f, 1.f),
            glm::vec3( 1.f,-1.f, 1.f),
            glm::vec3( 1.f, 1.f, 1.f),
            glm::vec3(-1.f, 1.f, 1.f),
            glm::vec3( 0.f, 0.f, 1.f)
            ),
        MAKE_FACE(
            glm::vec3( 1.f,-1.f, 1.f),
            glm::vec3( 1.f,-1.f,-1.f),
            glm::vec3( 1.f, 1.f,-1.f),
            glm::vec3( 1.f, 1.f, 1.f),
            glm::vec3( 1.f, 0.f, 0.f)
            ),
        MAKE_FACE(
            glm::vec3( 1.f,-1.f,-1.f),
            glm::vec3(-1.f,-1.f,-1.f),
            glm::vec3(-1.f, 1.f,-1.f),
            glm::vec3( 1.f, 1.f,-1.f),
            glm::vec3( 0.f, 0.f,-1.f)
            ),
        MAKE_FACE(
            glm::vec3(-1.f,-1.f,-1.f),
            glm::vec3(-1.f,-1.f, 1.f),
            glm::vec3(-1.f, 1.f, 1.f),
            glm::vec3(-1.f, 1.f,-1.f),
            glm::vec3(-1.f, 0.f, 0.f)
            ),
        MAKE_FACE(
            glm::vec3(-1.f, 1.f, 1.f),
            glm::vec3( 1.f, 1.f, 1.f),
            glm::vec3( 1.f, 1.f,-1.f),
            glm::vec3(-1.f, 1.f,-1.f),
            glm::vec3( 0.f, 1.f, 0.f)
            ),
        MAKE_FACE(
            glm::vec3(-1.f,-1.f,-1.f),
            glm::vec3( 1.f,-1.f,-1.f),
            glm::vec3( 1.f,-1.f, 1.f),
            glm::vec3(-1.f,-1.f, 1.f),
            glm::vec3( 0.f,-1.f, 0.f)
            ),
    };

    std::vector<GLuint> index_data = {
        MAKE_FACE_INDEX( 0), MAKE_FACE_INDEX( 4), MAKE_FACE_INDEX( 8),
        MAKE_FACE_INDEX(12), MAKE_FACE_INDEX(16), MAKE_FACE_INDEX(20),
    };
    cube.count = 36;

    glGenVertexArrays(1, &cube.vao);
    glGenBuffers(1, &cube.vbo);
    glGenBuffers(1, &cube.ebo);
    glBindVertexArray(cube.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertPNT) * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index_data.size(), index_data.data(), GL_STATIC_READ);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertPNT), reinterpret_cast<GLvoid*>(offsetof(VertPNT, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(VertPNT), reinterpret_cast<GLvoid*>(offsetof(VertPNT, normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertPNT), reinterpret_cast<GLvoid*>(offsetof(VertPNT, tex_coord)));

#undef MAKE_FACE
#undef MAKE_FACE_INDEX
}

GLuint program;
GLuint diffuse_tex;
GLuint height_tex;
GLuint normal_tex;
GLint loc_projection;
GLint loc_view;
GLint loc_depth;
GLint loc_diffuse_tex;
GLint loc_normal_tex;
GLint loc_height_tex;

bool init_parallax_mapping(ShaderManager* sm, ImageLoader* il)
{
    GLuint vert = sm->make_shader(GL_VERTEX_SHADER, "shaders/parallax.vert");
    GLuint geom = sm->make_shader(GL_GEOMETRY_SHADER, "shaders/parallax.geom");
    GLuint frag = sm->make_shader(GL_FRAGMENT_SHADER, "shaders/parallax.frag");
    program = sm->make_program({vert, geom, frag});
    if (program == 0u) return false;
    glDeleteShader(vert);
    glDeleteShader(geom);
    glDeleteShader(frag);
    loc_projection = glGetUniformLocation(program, "projection");
    loc_view = glGetUniformLocation(program, "view");
    loc_depth = glGetUniformLocation(program, "depth");
    loc_diffuse_tex = glGetUniformLocation(program, "diffuse_tex");
    loc_normal_tex = glGetUniformLocation(program, "normal_tex");
    loc_height_tex = glGetUniformLocation(program, "height_tex");

    diffuse_tex = il->make_texture_from_image("images/wall_diffuse.png");
    normal_tex = il->make_texture_from_image("images/wall_normal.png");
    height_tex = il->make_texture_from_image("images/wall_height.png");
    return true;
}

int main()
{
    if (not glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Failed to create window.\n");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (not gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        return -1;
    }

    glfwSetMouseButtonCallback(window, on_mouse_button);
    glfwSetKeyCallback(window, on_key);
    glfwSetScrollCallback(window, on_scroll);

    ShaderManager sm;
    ImageLoader il;
    DrawUtil du {&sm};

    if (not il.init()) {
        fprintf(stderr, "Failed to initialize image loader.\n");
        return -1;
    }

    if (not du.init()) {
        fprintf(stderr, "Failed to initialize draw util.\n");
        return -1;
    }

    if (not init_parallax_mapping(&sm, &il)) {
        fprintf(stderr, "Failed to initialize parallax mapping.\n");
        return -1;
    }

    float aspect = static_cast<float>(window_width) / window_height;
    cam.projection = glm::perspective(1.f, aspect, 0.1f, 1000.f);

    std::vector<VertPC> grid;
    make_grid(grid, 10, glm::vec3{0.3f, 0.3f, 0.3f});
    update_view();

    Geometry cube;
    make_cube(cube);

    glClearColor(0.5f, 0.5f, 0.5f, 0.f);

    while (not glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        du.draw(GL_LINES, cam.projection, cam.view, grid);

        glUseProgram(program);
        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(cam.projection));
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(cam.view));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuse_tex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_tex);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, height_tex);
        glUniform1f(loc_depth, depth);
        glUniform1i(loc_diffuse_tex, 1);
        glUniform1i(loc_normal_tex, 2);
        glUniform1i(loc_height_tex, 3);
        glBindVertexArray(cube.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ebo);
        glDrawElements(GL_TRIANGLES, cube.count - 12, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
