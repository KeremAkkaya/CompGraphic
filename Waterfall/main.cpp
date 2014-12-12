#include "common.h"
#include "waterfallprogram.h"
#include <iostream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>

using std::string;
using std::ifstream;

std::unique_ptr<WaterfallProgram> gWaterfallProgram;

void displayFunc()
{
    gWaterfallProgram->drawFrame();

    TwDraw();

    glutSwapBuffers();
}

void idleFunc()
{
    glutPostRedisplay();
}

void keyboardFunc(unsigned char button, int x, int y)
{
    if (TwEventKeyboardGLUT(button, x, y)) {
        return;
    }

    switch (button) {
    case 27:
        exit(0);
    }
}

void reshapeFunc(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return;
    }
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

void closeFunc()
{
    gWaterfallProgram.reset();
}

int main(int argc, char** argv)
{
    std::cout << "Let get it started" << std::endl;

    srand(time(NULL));

    size_t const default_width = 1024;
    size_t const default_height = 1024;

    glutInit(&argc, argv);
    glutInitWindowSize(default_width, default_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 0);
    glutCreateWindow("Waterfall");

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed" << std::endl;
        return 1;
    }

    if (!GLEW_VERSION_3_0) {
        std::cerr << "OpenGL 3.0 not supported" << std::endl;
        return 1;
    }

    glutReshapeFunc(reshapeFunc);
    glutDisplayFunc(displayFunc);
    glutIdleFunc(idleFunc);
    glutCloseFunc(closeFunc);
    glutKeyboardFunc(keyboardFunc);

    glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
    TwGLUTModifiersFunc(glutGetModifiers);

    try {
        gWaterfallProgram.reset(new WaterfallProgram());
        glutMainLoop();
    }
    catch (std::exception const& except) {
        std::cout << except.what() << std::endl;
        return 1;
    }

    return 0;
}
