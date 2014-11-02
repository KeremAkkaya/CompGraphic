#include "common.h"
#include "shader.h"
#include "model.h"

#ifndef APIENTRY
#define APIENTRY
#endif


void TW_CALL toggle_fullscreen_callback( void * )
{
    glutFullScreenToggle();
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

static const string MODEL_FILE         = "model.obj";
static const string VERTEX_SHADER      = "shaders/0.glslvs";
static const string FRAGMENT_SHADER    = "shaders/0.glslfs";


struct triangle
{
    const vec2 v1, v2, v3;

    triangle(const vec2& v1, const vec2& v2, const vec2& v3)
        : v1(v1), v2(v2), v3(v3)
    {}

    vec2 centroid() const
    {
        return (v1 + v2 + v3) / 3.0f;
    }
};

class sample_t
{
    enum ColorMode { NORMALS, FUNC };

public:
    sample_t();
    ~sample_t();

    void init_buffer();
    void draw_frame(float time_from_start);
    void change_mode();
    void init(string const& vs_file, string const& fs_file);

    void refresh(mat4 m);

private:
    bool   skeleton_;
    triangle triangle_;
    float  cell_size_;
    ColorMode mode_;

    GLuint vs_, fs_, program_;
    GLuint vx_buf_;
    quat   rotation_by_control_;

    vvec3 data_;
    Model model_;

    float v_, k_;
    vec3 center_;
    float max_;
};

void switch_colors_callback(void * sample)
{
    static_cast<sample_t*>(sample)->change_mode();
}


sample_t::sample_t()
    : skeleton_(false), triangle_(vec2(0, 25), vec2(20, -15), vec2(-20, -15)), cell_size_(3.0f), mode_(NORMALS),
      v_(1), k_(1), center_(vec3(0, 0, 0)), max_(0)
{
    TwInit(TW_OPENGL, NULL);

    // Определение "контролов" GUI
    TwBar *bar = TwNewBar("Parameters");
    TwDefine(" Parameters size='500 150' color='70 100 120' valueswidth=220 iconpos=topleft");

    TwAddVarRW(bar, "v", TW_TYPE_FLOAT, &v_, " min=-100 max=100 step=1 label='V' keyincr=p keydecr=o");
    TwAddVarRW(bar, "k", TW_TYPE_FLOAT, &k_, " min=-100 max=100 step=1 label='K' keyincr=l keydecr=k");

    TwAddVarRW(bar, "Skeleton", TW_TYPE_BOOLCPP, &skeleton_, " true='ON' false='OFF' key=w");

    TwAddButton(bar, "SwitchColorMode", switch_colors_callback, this,
                " label = 'Switch color mode' key=g");

    TwAddButton(bar, "Fullscreen toggle", toggle_fullscreen_callback, NULL,
                " label='Toggle fullscreen mode' key=f");

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation_by_control_,
               " label='Object orientation' opened=true help='Change the object orientation.' ");

    model_.load(MODEL_FILE);

    for (size_t i = 0; i < model_.vertices_count(); ++i) {
        data_.push_back(model_.vertices_[i]);
        data_.push_back(model_.normals_[i]);
    }

    init(VERTEX_SHADER, FRAGMENT_SHADER);
}

void sample_t::refresh(mat4 m)
{
    for (size_t i = 0; i < model_.vertices_count(); ++i) {
        center_ += model_.vertices_[i];
    }

    center_ = center_ / float(model_.vertices_count());

    for (size_t i = 0; i < model_.vertices_count(); ++i) {
        if (glm::length(center_ - model_.vertices_[i]) > max_) {
            max_ = glm::length(center_ - model_.vertices_[i]);
        }
    }

    center_ = vec3(m * vec4(center_, 1));
}

void sample_t::change_mode()
{
    if (mode_ == NORMALS) {
        mode_ = FUNC;
    }
    else {
        mode_ = NORMALS;
    }
}

sample_t::~sample_t()
{
    // Удаление ресурсов OpenGL
    glDeleteProgram(program_);
    glDeleteShader(vs_);
    glDeleteShader(fs_);
    glDeleteBuffers(1, &vx_buf_);

    TwDeleteAllBars();
    TwTerminate();
}

void sample_t::init(string const& vs_file, string const& fs_file)
{
    vs_ = create_shader(GL_VERTEX_SHADER  , vs_file.c_str());
    fs_ = create_shader(GL_FRAGMENT_SHADER, fs_file.c_str());

    program_ = create_program(vs_, fs_);

    init_buffer();
}

void sample_t::init_buffer()
{
    // Создание пустого буфера
    glGenBuffers(1, &vx_buf_);
    // Делаем буфер активным
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_);

    // Копируем данные для текущего буфера на GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data_.size(), &data_[0], GL_STATIC_DRAW);

    // Сбрасываем текущий активный буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sample_t::draw_frame( float time_from_start )
{
    float const w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float const h = (float)glutGet(GLUT_WINDOW_HEIGHT);

    mat4 const proj             = perspective(45.0f, w / h, 0.1f, 100.0f);
    mat4 const view             = lookAt(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 const full_rotate      = mat4_cast(rotation_by_control_);

    mat4 const modelview        = view * full_rotate;
    mat4 const mvp              = proj * modelview;

    refresh(modelview); //or refresh(mat4(1.0f));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(program_);

    GLuint const mvp_location = glGetUniformLocation(program_, "mvp");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    GLuint const is_skeleton_location = glGetUniformLocation(program_, "is_skeleton");
    glUniform1i(is_skeleton_location, false);

    GLuint const T_location = glGetUniformLocation(program_, "T");
    glUniform1f(T_location, time_from_start);

    GLuint const k_location = glGetUniformLocation(program_, "k");
    glUniform1f(k_location, k_);

    GLuint const v_location = glGetUniformLocation(program_, "v");
    glUniform1f(v_location, v_);

    GLuint const center_location = glGetUniformLocation(program_, "center");
    glUniform3f(center_location, center_[0], center_[1], center_[2]);

    GLuint const max_location = glGetUniformLocation(program_, "max");
    glUniform1f(max_location, max_);

    GLuint const func_mode_location = glGetUniformLocation(program_, "func_mode");
    if (mode_ == NORMALS) {
        glUniform1i(func_mode_location, false);
    }
    else {
        glUniform1i(func_mode_location, true);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_);

    GLuint const pos_location = glGetAttribLocation(program_, "in_pos");
    glEnableVertexAttribArray(pos_location);
    glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), 0);

    GLuint const color_location = glGetAttribLocation(program_, "in_color");
    glEnableVertexAttribArray(color_location);
    glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (GLvoid*)(sizeof(vec3)));

    glDrawArrays(GL_TRIANGLES, 0, data_.size() / 2);

    if (skeleton_) {
        glPolygonOffset(-1, -1);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        GLboolean const is_skeleton_location = glGetUniformLocation(program_, "is_skeleton");
        glUniform1i(is_skeleton_location, true);

        glDrawArrays(GL_TRIANGLES, 0, data_.size() / 2);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glDisableVertexAttribArray(pos_location);
    glDisableVertexAttribArray(color_location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

unique_ptr<sample_t> g_sample;

// отрисовка кадра
void display_func()
{
    static chrono::system_clock::time_point const start = chrono::system_clock::now();

    // вызов функции отрисовки с передачей ей времени от первого вызова
    g_sample->draw_frame(chrono::duration<float>(chrono::system_clock::now() - start).count());

    // отрисовка GUI
    TwDraw();

    // смена front и back buffer'а (напоминаю, что у нас используется режим двойной буферизации)
    glutSwapBuffers();
}

// Переисовка кадра в отсутствии других сообщений
void idle_func()
{
    glutPostRedisplay();
}

void keyboard_func( unsigned char button, int x, int y )
{
    if (TwEventKeyboardGLUT(button, x, y))
        return;

    switch(button)
    {
    case 27:
        // g_sample.reset();
        exit(0);
    }
}

// Отработка изменения размеров окна
void reshape_func( int width, int height )
{
    if (width <= 0 || height <= 0)
        return;
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

// Очищаем все ресурсы, пока контекст ещё не удалён
void close_func()
{
    g_sample.reset();
}

// callback на различные сообщения от OpenGL
void APIENTRY gl_debug_proc(  GLenum         //source
                              , GLenum         type
                              , GLuint         //id
                              , GLenum         //severity
                              , GLsizei        //length
                              , GLchar const * message

                              , GLvoid * //user_param
                              )
{
    if (type == GL_DEBUG_TYPE_ERROR_ARB)
    {
        cerr << message << endl;
        exit(1);
    }
}

#include <sstream>

int main( int argc, char ** argv )
{
    // Размеры окна по-умолчанию
    size_t const default_width  = 800;
    size_t const default_height = 800;

    glutInit               (&argc, argv);
    glutInitWindowSize     (default_width, default_height);
    // Указание формата буфера экрана:
    // - GLUT_DOUBLE - двойная буферизация
    // - GLUT_RGB - 3-ёх компонентный цвет
    // - GLUT_DEPTH - будет использоваться буфер глубины
    glutInitDisplayMode    (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    // Создаем контекст версии 3.2
    glutInitContextVersion (3, 0);
    // Контекст будет поддерживать отладку и "устаревшую" функциональность, которой, например, может пользоваться библиотека AntTweakBar
    // glutInitContextFlags   (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    // Указание либо на core либо на compatibility профил
    //glutInitContextProfile (GLUT_COMPATIBILITY_PROFILE );
    int window_handle = glutCreateWindow("OpenGL basic sample");

    // Инициализация указателей на функции OpenGL
    if (glewInit() != GLEW_OK)
    {
        cerr << "GLEW init failed" << endl;
        return 1;
    }

    // Проверка созданности контекста той версии, какой мы запрашивали
    if (!GLEW_VERSION_3_0)
    {
        cerr << "OpenGL 3.0 not supported" << endl;
        return 1;
    }

    // подписываемся на оконные события
    glutReshapeFunc(reshape_func);
    glutDisplayFunc(display_func);
    glutIdleFunc   (idle_func   );
    glutCloseFunc  (close_func  );
    glutKeyboardFunc(keyboard_func);

    // подписываемся на события для AntTweakBar'а
    glutMouseFunc        ((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    glutMotionFunc       ((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutSpecialFunc      ((GLUTspecialfun    )TwEventSpecialGLUT    );
    TwGLUTModifiersFunc  (glutGetModifiers);

    try
    {
        // Создание класса-примера
        g_sample.reset(new sample_t());
        // Вход в главный цикл приложения
        glutMainLoop();
    }
    catch( std::exception const & except )
    {
        std::cout << except.what() << endl;
        return 1;
    }

    return 0;
}
