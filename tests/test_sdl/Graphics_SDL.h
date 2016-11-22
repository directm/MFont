#ifndef Graphics_SDL_H
#define Graphics_SDL_H

#include <SDL/SDL.h>
#include "MFont.h"
class Graphics_SDL
{
    public:
        Graphics_SDL();
        virtual ~Graphics_SDL();
        void init(int screenW,int screenH);
        void setBmFont(const MFont* font);

        SDL_Texture* createTexture(const MBitMap& bm);
        void drawTexture(SDL_Texture*texture,int x,int y,int w,int h);
        void drawString(const char* str,int x,int y);
        void close();
    public :
        SDL_Window*   window;
        SDL_Renderer* renderer;
        SDL_Texture*  charsImg;
        MFont*        mFont;
};

#endif // SDL_RENDERER_H
