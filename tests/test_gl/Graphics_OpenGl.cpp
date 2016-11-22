#include "Graphics_OpenGl.h"
#include <stdio.h>


Graphics_OpenGl::Graphics_OpenGl()
{

}

Graphics_OpenGl::~Graphics_OpenGl()
{

}

void Graphics_OpenGl::init(int w,int h)
{

    //window=SDL_CreateWindow("MFont",32,32,w,h,SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
    //glContext=SDL_GL_CreateContext(window);

}

GLuint Graphics_OpenGl::createTexture(const MBitMap& bm)
{
    int w=(bm.w);
    int h=(bm.h);
    GLuint t=0;
    glGenTextures(1,&t);

    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA,
             w, h,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             bm.data);
    return t;
}

void Graphics_OpenGl::setBmFont(const MFont* font)
{
    mFont=(MFont*)font;
    MBitMap bm;
    mFont->generateAtlas(&bm,32,127);
    charsImg=createTexture(bm);
    delete[]bm.data;
}

void Graphics_OpenGl::drawTexture(GLuint texture,int x,int y,int w,int h)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);

    glVertex2d(x,y);

    glTexCoord2f(1,0);
    glVertex2d(x+w,y);

    glTexCoord2f(1,1);
    glVertex2d(x+w,y+h);

    glTexCoord2f(0,1);
    glVertex2d(x,y+h);
    glEnd();
}

void Graphics_OpenGl::drawString(const char* str,int x,int y)
{
    size_t l=strlen(str);
    for(size_t i=0;i<l;++i)
    {
        MChar &h=mFont->mChars[str[i]];
        float u0,v0,u1,v1;
        mFont->getUv(str[i],u0,v0,u1,v1);
        y+=h.yoff;
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,charsImg);
        glBegin(GL_QUADS);
        glTexCoord2f(u0,v0);
        glVertex2d(x,y);

        glTexCoord2f(u1,v0);
        glVertex2d(x+h.bm.w,y);

        glTexCoord2f(u1,v1);
        glVertex2d(x+h.bm.w,y+h.bm.h);

        glTexCoord2f(u0,v1);
        glVertex2d(x,y+h.bm.h);
        glEnd();
        x+=h.xAdvance;
        y-=h.yoff;
    }
}

void Graphics_OpenGl::close()
{
    //SDL_DestroyWindow(window);
}
