#include "common.h"
#include "shader.h"

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

static const string SCREEN_VERTEX_SHADER      = "shaders/2.glslvs";
static const string SCREEN_FRAGMENT_SHADER    = "shaders/2.glslfs";
static const string NONSCREEN_VERTEX_SHADER   = "shaders//3.glslvs";
static const string NONSCREEN_FRAGMENT_SHADER = "shaders//3.glslfs";

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
    enum Shaders { SCREEN, NONSCREEN };

public:
   sample_t();
   ~sample_t();

   void init_buffer();
   void draw_frame(float time_from_start);
   void change_mode();
   void init(string const& vs_file, string const& fs_file);

private:
   bool   wireframe_;
   triangle triangle_;
   float  cell_size_;
   Shaders mode_;

   GLuint vs_, fs_, program_;
   GLuint vx_buf_;
   quat   rotation_by_control_;
};

void switch_shaders_callback(void * sample)
{
    static_cast<sample_t*>(sample)->change_mode();
}


sample_t::sample_t()
   : wireframe_(false), triangle_(vec2(0, 25), vec2(20, -15), vec2(-20, -15)), cell_size_(3.0f), mode_(SCREEN)
{
   TwInit(TW_OPENGL, NULL);

   // Определение "контролов" GUI
   TwBar *bar = TwNewBar("Parameters");
   TwDefine(" Parameters size='500 150' color='70 100 120' valueswidth=220 iconpos=topleft");

   TwAddVarRW(bar, "Wireframe mode", TW_TYPE_BOOLCPP, &wireframe_, " true='ON' false='OFF' key=w");

   TwAddVarRW(bar, "CellSize", TW_TYPE_FLOAT, &cell_size_, " min=1 max=10 step=1 label='Checkerboard cell size' keyincr=p keydecr=o");

   TwAddButton(bar, "Fullscreen toggle", toggle_fullscreen_callback, NULL,
               " label='Toggle fullscreen mode' key=f");

   TwAddButton(bar, "SwitchShaders", switch_shaders_callback, this,
               " label='SWITCH SHADERS' key=g");

   TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &rotation_by_control_,
              " label='Object orientation' opened=true help='Change the object orientation.' ");

    init(SCREEN_VERTEX_SHADER, SCREEN_FRAGMENT_SHADER);
}

void sample_t::change_mode()
{
    glDeleteProgram(program_);
    glDeleteShader(vs_);
    glDeleteShader(fs_);
    glDeleteBuffers(1, &vx_buf_);

    if (mode_ == SCREEN) {
        mode_ = NONSCREEN;
        init(NONSCREEN_VERTEX_SHADER, NONSCREEN_FRAGMENT_SHADER);
    }
    else {
        mode_ = SCREEN;
        init(SCREEN_VERTEX_SHADER, SCREEN_FRAGMENT_SHADER);
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

   // Данные для визуализации
   vec2 const data[3] =
   {
        triangle_.v1 //vec2(0, 2)
      , triangle_.v2 //vec2(sqrt(3), -1)
      , triangle_.v3 //vec2(-sqrt(3), -1)
   };

   // Копируем данные для текущего буфера на GPU
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 3, data, GL_STATIC_DRAW);

   // Сбрасываем текущий активный буфер
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sample_t::draw_frame( float time_from_start )
{
    float const rotation_angle = time_from_start * 90;

    float const w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float const h = (float)glutGet(GLUT_WINDOW_HEIGHT);

    mat4 const proj             = perspective(45.0f, w / h, 0.1f, 100.0f);
    mat4 const view             = lookAt(vec3(0, 0, 80), vec3(0, 0, 0), vec3(0, 1, 0));
    quat const rotation_by_time = quat(vec3(0, 0, radians(rotation_angle)));
    mat4 const full_rotate      = mat4_cast(rotation_by_control_ * rotation_by_time);
    mat4 const trans            = translate(mat4(), vec3(-triangle_.centroid(), 0));
    mat4 const reverse_trans    = translate(mat4(), vec3( triangle_.centroid(), 0));

    mat4 const modelview        = view * reverse_trans * full_rotate * trans;
    mat4 const mvp              = proj * modelview;

    if (wireframe_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_);

    GLuint const mvp_location = glGetUniformLocation(program_, "mvp");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    GLuint const cell_size_location = glGetUniformLocation(program_, "cell_size");
    glUniform1i(cell_size_location, cell_size_);

    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_);

    GLuint const pos_location = glGetAttribLocation(program_, "in_pos");
    glVertexAttribPointer(pos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
    glEnableVertexAttribArray(pos_location);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(pos_location);
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
