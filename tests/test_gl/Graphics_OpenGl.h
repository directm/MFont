#ifndef GRAPHICS_OPENGL_H
#define GRAPHICS_OPENGL_H
#include <windows.h>
#include <gl/gl.h>
#include "MFont.h"
class Graphics_OpenGl
{
    public:
        Graphics_OpenGl();
        virtual ~Graphics_OpenGl();
        void init(int screenW,int screenH);
        void setBmFont(const MFont* font);

        GLuint createTexture(const MBitMap& bm);
        void drawTexture(GLuint texture,int x,int y,int w,int h);
        void drawString(const char* str,int x,int y);
        void close();
    public :
        //SDL_Window*   window;
        //SDL_GLContext glContext;;
        GLuint        charsImg;
        MFont*        mFont;
};

#endif // GRAPHICS_OPENGL_H
