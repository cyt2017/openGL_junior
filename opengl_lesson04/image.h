#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include "tool/CELLMath.hpp"
#include <FreeImage.h>

class IMAGE : public QObject
{
    Q_OBJECT
public:
    explicit IMAGE(QObject *parent = 0,int w=800,int h=600,void* data=NULL);
    ~IMAGE();

    int     _width;
    int     _height;
    uint*   _pixel;
    int     _wrapType;

    void    setWrapType(int type);
    int     w() const;
    int     h() const;
    CELL::Rgba setPixel(int x,int y) const;
    CELL::Rgba pixelUV(float u,float v);

    static  IMAGE*  loadFromFile(const char*);

signals:

public slots:
};

#endif // IMAGE_H
