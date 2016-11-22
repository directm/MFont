#define MFONT_USE_SDL
#include "Graphics_SDL.h"

int main(int argc,char* argv[])
{
   Graphics_SDL g;
   g.init(800,600);

   MFont font;
   font.load("x.ttf",50,MFont::Mrgb(255,255,255));

   Uint32 t0=SDL_GetTicks();
   MBitMap bm;
   font.makeText("SDL makeText !",&bm);
   SDL_Texture* t=g.createTexture(bm);
   SDL_Log("%f\n",(SDL_GetTicks()-t0)*0.001f);

   g.setBmFont(&font);

   bool running=true;
   SDL_Event e;
   while(running)
   {
       while(SDL_PollEvent(&e))
       {
            switch(e.type)
            {
            case SDL_QUIT:
                running=false;
                break;
            }
       }
       SDL_RenderClear(g.renderer);
       g.drawTexture(t,100,100,bm.w,bm.h);
       g.drawString("SDL drawString !",100,100+font.mHeight);
       SDL_RenderPresent(g.renderer);
       SDL_Delay(1);
   }
    g.close();
    system("PAUSE");
    return 0;
}
