#ifndef FONT_H
#define FONT_H

typedef  unsigned int uint;
struct MBitMap
    {
        uint* data;
        int w,h;
    };

    struct V4f
    {
        float u0,v0,
              u1,v1;
    };

    struct MChar
    {
        MBitMap bm;
        int     xAdvance;
        int     yoff;
        int     x,y;
        V4f     uv;
    };

enum MCOLORFORMAT
{
    M_RGB,
    M_BGR
};

class MFont
{
    public:

        MFont();

        virtual ~MFont();

        //set this befor load
        void    setStyle(bool italic,bool outline);

        bool    load(const char* path,int fontSize
                     ,uint color=0xFFFFFFFF
                     ,MCOLORFORMAT format=M_RGB);

        bool    loadFromMemory(const unsigned char* data,long dataSize,
                               int fontSize
                               ,uint color=0xFFFFFFFF
                               ,MCOLORFORMAT format=M_RGB);

        void    setPowerOf2Sizes(bool usePo2);

        void    makeText(const char* text,MBitMap* out);

        int     textWidth(const char* text);

        void    generateAtlas(MBitMap* atlas,int firstChar=32
                                      ,int lastChar=127);

        void    generateAtlas(MBitMap* atlas,const char* atlasChars);

        void    getUv(char c,float& u0,float& v0
                          ,float& u1,float& v1);

        void    getCharRect(char c,int& x,int& y
                          ,int& w,int& h);

        inline void setColor(uint color);

        static unsigned int Mrgb(int r,int g,int b);

        static unsigned int Mbgr(int g,int b,int r);

        static void freeBm(MBitMap* bm);

        void close();

    public:
        int     mHeight;
        MChar   mChars[256];
        uint    mColor;
        bool    mBold;
        bool    mOutline;
        bool    mItalic;
    private:
        bool    mUsePo2;
        void    fillBitmap(const MBitMap& src,MBitMap& dst,int x,int y);
};

#endif // FONT_H
