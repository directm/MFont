#include "Graphics_SDL.h"

Graphics_SDL::Graphics_SDL()
{
    charsImg=0;
}

Graphics_SDL::~Graphics_SDL()
{

}


void Graphics_SDL::init(int w,int h)
{
    window=SDL_CreateWindow("MFont",32,32,w,h,SDL_WINDOW_SHOWN);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer,&info);
}

SDL_Texture* Graphics_SDL::createTexture(const MBitMap& bm)
{
    SDL_Surface *s=SDL_CreateRGBSurface(0,bm.w,bm.h,32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF);
    memcpy(s->pixels,bm.data,bm.w*bm.h*sizeof(unsigned int));
    SDL_Texture*t= SDL_CreateTextureFromSurface(renderer,s);
    SDL_FreeSurface(s);
    return t;
}

void Graphics_SDL::setBmFont(const MFont* font)
{
    mFont=(MFont*)font;
    MBitMap bm;
    mFont->generateAtlas(&bm,32,127);
    //mFont->generateAtlas(&bm,"AabcdefghijklmMnopqrstuvwxz0123456789ABCDEFGHIJCLMNOP");
    charsImg=createTexture(bm);

    delete[]bm.data;
}

void Graphics_SDL::drawTexture(SDL_Texture* texture,int x,int y,int w,int h)
{
    SDL_Rect dst={x,y,w,h};
    SDL_RenderCopy(renderer,texture,0,&dst);
}

void Graphics_SDL::drawString(const char* str,int x,int y)
{
    size_t l=SDL_strlen(str);
    for(size_t i=0;i<l;++i)
    {
        MChar &h=mFont->mChars[str[i]];
        SDL_Rect dst={x,y+h.yoff,h.bm.w,h.bm.h};
        SDL_Rect src={h.x,h.y,h.bm.w,h.bm.h};
        SDL_RenderCopy(renderer,charsImg,&src,&dst);
        x+=h.xAdvance;
    }
}

void Graphics_SDL::close()
{
     if(charsImg)
        SDL_DestroyTexture(charsImg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
