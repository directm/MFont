#include "MFont.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

char* readFile(const char* path,const char* mode,long& size)
{
    FILE* f;
    fopen_s(&f,path,mode);
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);
     char* data=new char[size];
    fread(data,1,size,f);
    fclose(f);
    return data;
}

#define FT_CEIL(X)  (((X+63)&-64)/64)
#define FT_FLOOR(X) ((X&-64)/64)

void makeBitmap(MBitMap* bm,int w,int h)
{
    bm->w=w;
    bm->h=h;
    bm->data=new uint[w*h];
    memset(bm->data,0,bm->w*bm->h*sizeof(uint));
}


MFont::MFont()
{
    mUsePo2=false;
    for(int i=0;i<256;++i){
            mChars[i].bm.data=0;
    }
}

MFont::~MFont()
{
    close();
}


bool MFont::load(const char* path,int size,uint color,MCOLORFORMAT format)
{
    long s;
    unsigned char* buffer=(unsigned char*)readFile(path,"rb",s);
    bool r=loadFromMemory(buffer,s,size,color,format);
    delete[]buffer;
    return r;
}

bool MFont::loadFromMemory(const unsigned char* data,long dataSize,int size,uint color,MCOLORFORMAT format)
{
    FT_Library    library;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_Error      error;

    error = FT_Init_FreeType( &library );
    if(error)
    {
        return false;
    }
    error=FT_New_Memory_Face(library,data,dataSize,0, &face );
    if(error)
    {
        return false;
    }
    error = FT_Set_Char_Size(face,size*64,0,0,0);
    slot = face->glyph;
    FT_Fixed scale = face->size->metrics.y_scale;
    int ascent=FT_CEIL(FT_MulFix(face->ascender, scale));
    int descent=FT_CEIL(FT_MulFix(face->descender, scale));
    mHeight=ascent-descent;

    int n=0;
   for ( n = 32; n < 128; n++ )
   {

     error =FT_Load_Glyph(face,FT_Get_Char_Index(face,n),FT_LOAD_DEFAULT);

     FT_Render_Glyph(slot,FT_RENDER_MODE_NORMAL);

     if ( error )
       continue;

     MChar& c=mChars[n];
     c.xAdvance=FT_CEIL(slot->metrics.horiAdvance);
     c.yoff=ascent-FT_FLOOR(slot->metrics.horiBearingY);
     makeBitmap(&c.bm,slot->bitmap.width,slot->bitmap.rows);
     if(format==M_BGR)
       color=color& 0x00FFFFFF;
    else color=color&0xFFFFFF00;

     for(int i=0;i<c.bm.h;++i)
     {
         for(int j=0;j<c.bm.w;++j)
            {
                int id=i*c.bm.w+j;
                if(format==M_BGR){
                    unsigned int y=0xFFFFFFFF&slot->bitmap.buffer[id];

                    y=y<<24;
                    c.bm.data[id]=y;
                }
                else
                {
                    c.bm.data[id]=slot->bitmap.buffer[id];
                }
            }
     }

   }
   mColor=color;
   FT_Done_Face(face);
   FT_Done_FreeType( library );
   return true;
}

void MFont::setPowerOf2Sizes(bool usePo2)
{
    mUsePo2=usePo2;
}

static int po2(int x)
{
    int z = 1;

    while ( z < x ) {
        z <<= 1;
    }
    return z;
}
void MFont::makeText(const char* text,MBitMap* out)
{
    int w=0;
    unsigned int l=strlen(text);
    for(unsigned int i=0;i<l;++i)
    {
        MChar &h=mChars[text[i]];
        w+=h.bm.w>h.xAdvance?h.bm.w:h.xAdvance;
    }
    int h=mHeight;
    if(mUsePo2){
        w=po2(w);
        h=po2(mHeight);
    }

    makeBitmap(out,w,h);
    w=0;
    for(unsigned i=0;i<l;++i)
    {
       MChar &h=mChars[text[i]];
       fillBitmap(h.bm,*out,w,h.yoff);
       w+=h.xAdvance;
    }

}

void MFont::generateAtlas(MBitMap* atlas,int firstChar,int lastChar)
{
    ++lastChar;
    int w=0;
    int y=0;
    int targetW=1024;
    int targetY=mHeight;
    bool b=false;
    int yy=mChars[firstChar].bm.h;
    for(int i=firstChar;i<lastChar;++i)
    {
        MChar &h=mChars[i];
        w+=h.bm.w;
        if(yy<h.bm.h){
            yy=h.bm.h;
         }

        if(w>=targetW)
        {
            w=0;
            targetY+=yy;
            b=true;
        }
    }
    if(!b)
        targetW=w;
    if(mUsePo2)
    {
        targetW=po2(targetW);
        targetY=po2(targetY);
    }
    makeBitmap(atlas,targetW,targetY);
    w=0;
    y=0;
    yy=mChars[firstChar].bm.h;
    for(int i=firstChar;i<lastChar;++i)
    {
        MChar &h=mChars[i];
        if(yy<h.bm.h){
            yy=h.bm.h;
         }

        if(w+h.bm.w>=targetW)
        {
            w=0;
            y+=yy;
        }
        h.x=w;
        h.y=y;
        h.uv.u0=(float)w/targetW;
        h.uv.v0=(float)y/targetY;
        h.uv.u1=(float)(w+h.bm.w)/targetW;
        h.uv.v1=(float)(y+h.bm.h)/targetY;
        fillBitmap(mChars[i].bm,*atlas,w,y);
        w+=h.bm.w;
    }
}

void MFont::generateAtlas(MBitMap* atlas,const char* atlasChars)
{
    int w=0;
    int y=0;
    int targetW=1024;
    int targetY=mHeight;
    bool b=false;
    int l=strlen(atlasChars);
    int yy=mChars[atlasChars[0]].bm.h;
    for(int i=0;i<l;++i)
    {
        MChar &h=mChars[atlasChars[i]];
        w+=h.bm.w;
        if(yy<h.bm.h){
            yy=h.bm.h;
         }
        if(w>=targetW)
        {
            w=0;
            targetY+=yy;
            b=true;
        }
    }
    if(!b)
        targetW=w;
    if(mUsePo2)
    {
        targetW=po2(targetW);
        targetY=po2(targetY);
    }
    makeBitmap(atlas,targetW,targetY);
    w=0;
    y=0;
    yy=mChars[atlasChars[0]].bm.h;
    for(int i=0;i<l;++i)
    {
        MChar &h=mChars[atlasChars[i]];
         if(yy<h.bm.h){
            yy=h.bm.h;
         }

        if(w+h.bm.w>=targetW)
        {
            w=0;
            y+=yy;
        }

        h.x=w;
        h.y=y;
        h.uv.u0=(float)w/targetW;
        h.uv.v0=(float)y/targetY;
        h.uv.u1=(float)(w+h.bm.w)/targetW;
        h.uv.v1=(float)(y+h.bm.h)/targetY;
        fillBitmap(mChars[atlasChars[i]].bm,*atlas,w,y);
        w+=h.bm.w;
    }
}

void MFont::getUv(char c,float& u0,float& v0
                          ,float& u1,float& v1)
{
    MChar&h= mChars[c];
    u0=h.uv.u0;
    u1=h.uv.u1;
    v0=h.uv.v0;
    v1=h.uv.v1;
}

void MFont::getCharRect(char c,int& x,int& y
                          ,int& w,int& h)
{
   MChar& hr= mChars[c];
   x=hr.x;
   y=hr.y;
   w=hr.bm.w;
   h=hr.bm.h;
}

inline void MFont::setColor(uint color)
{
    mColor=color;
}

unsigned int MFont::Mrgb(int r,int g,int b)
{
    return ((r<<24)|(g<<16)|b<<8|0);
}

unsigned int MFont::Mbgr(int g,int b,int r)
{
    return (0|g<<16|b<<8|r);
}

int MFont::textWidth(const char* text)
{
    int w=0;
    int l=strlen(text);
    for(int i=0;i<l;++i)
    {
        w+=mChars[text[i]].xAdvance;
    }
    return w;
}

void MFont::freeBm(MBitMap* bm)
{
    if(bm->data){
        delete bm->data;
        bm->data=0;
    }
}

void MFont::close()
{
    for(int i=32;i<128;++i){
        freeBm(&mChars[i].bm);
    }

}


void MFont::fillBitmap(const MBitMap& src,MBitMap& dst,int x,int y)
{
    for(int i=0;i<src.h;++i)
    {
        for(int j=0;j<src.w;++j)
        {
            unsigned int & d=dst.data[(i+y)*dst.w+j+x];
            unsigned int & s=src.data[i*src.w+j];
            if(x+j>=dst.w)
                break;
            if(s){
               d|=(s|mColor);
            }

        }
    }
}

