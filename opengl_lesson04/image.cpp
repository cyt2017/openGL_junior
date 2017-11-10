#include "image.h"

IMAGE::IMAGE(QObject *parent, int w, int h, void *data) : QObject(parent)
{
    _wrapType   =   0;
    if (w == 0 || h == 0 || data== 0)
    {
        _width  =   0;
        _height =   0;
        _pixel  =   0;
    }
    else
    {
        _width  =   w;
        _height =   h;
        _pixel  =   new uint[w * h];
        memcpy(_pixel,data,w * h * sizeof(uint));
    }
}

IMAGE::~IMAGE()
{
    delete []_pixel;
}

void IMAGE::setWrapType(int type)
{
    _wrapType   =   type;
}
int IMAGE::w() const
{
    return  _width;
}
int IMAGE::h() const
{
    return  _height;
}

CELL::Rgba IMAGE::setPixel(int x,int y) const
{
    return  CELL::Rgba(_pixel[y * _width + x]);
}

CELL::Rgba IMAGE::pixelUV(float u,float v)
{
    float x   =   u * _width;
    float y   =   v * _height;
    if (_wrapType == 0)
    {
        return  setPixel((unsigned)(x)%_width,(unsigned)(y)%_height);
    }
    else
    {
        if (x >= _width)
        {
            x   =   _width - 1;
        }
        if (y >= _height)
        {
            y   =   _height - 1;
        }
        return  setPixel(x,y);
    }
}


IMAGE* IMAGE::loadFromFile(const char* fileName)
{
    //1 获取图片格式
    FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
    if (fifmt == FIF_UNKNOWN)
    {
        return  0;
    }
    //2 加载图片
    FIBITMAP    *dib = FreeImage_Load(fifmt, fileName,0);

    FREE_IMAGE_COLOR_TYPE type    =   FreeImage_GetColorType(dib);

    //! 获取数据指针
    FIBITMAP*   temp    =   dib;
    dib =   FreeImage_ConvertTo32Bits(dib);
    FreeImage_Unload(temp);

    BYTE*   pixels  =   (BYTE*)FreeImage_GetBits(dib);
    int     width   =   FreeImage_GetWidth(dib);
    int     height  =   FreeImage_GetHeight(dib);

    int     pitch   =   width*4;
    BYTE*   row     =   new BYTE[width*4];
    for(int j = 0; j < height / 2; j++)
    {
        memcpy(row,pixels + j * pitch,pitch );

        memcpy(pixels + j * pitch,pixels + (height - j - 1) * pitch,pitch );

        memcpy(pixels + (height - j - 1) * pitch,row,pitch );

    }
    delete  []row;

    IMAGE*  image   =   new IMAGE(NULL,width,height,pixels);
    FreeImage_Unload(dib);

    return  image;
}
