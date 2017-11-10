#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include "tool/CELLMath.hpp"
#include "image.h"


enum    DRAWMODE
{
    DM_POINTS       =   0,
    DM_LINES        =   1,
    DM_LINE_LOOP    =   2,
    DM_LINE_STRIP   =   3,
    DM_TRIANGES     =   4,
};

enum    DATETYPE
{
    DT_BYTE,
    DT_FLOAT,
    DT_DOUBLE,
};

struct  DateElementDes
{
    int         _size;
    DATETYPE    _type;
    int         _stride;
    const void* _data;
};
class   Span
{
public:
    int     _xStart;
    int     _xEnd;
    CELL::Rgba    _colorStart;
    CELL::Rgba    _colorEnd;

    CELL::float2  _uvStart;
    CELL::float2  _uvEnd;

    int     _y;

public:
    Span(int xStart,int xEnd,int y,CELL::Rgba colorStart,CELL::Rgba colorEnd,CELL::float2 uvStart,CELL::float2 uvEnd)
    {
        if (xStart < xEnd)
        {
            _xStart     =   xStart;
            _xEnd       =   xEnd;
            _colorStart =   colorStart;
            _colorEnd   =   colorEnd;

            _uvStart    =   uvStart;
            _uvEnd      =   uvEnd;

            _y          =   y;
        }
        else
        {
            _xStart     =   xEnd;
            _xEnd       =   xStart;

            _colorStart =   colorEnd;
            _colorEnd   =   colorStart;

            _uvStart    =   uvEnd;
            _uvEnd      =   uvStart;
            _y          =   y;
        }
    }
};

class   Ege
{
public:
    int     _x1;
    int     _y1;
    CELL::float2  _uv1;
    CELL::Rgba    _color1;

    int     _x2;
    int     _y2;
    CELL::float2  _uv2;
    CELL::Rgba    _color2;

    Ege(int x1,int y1,CELL::Rgba color1,CELL::float2 uv1,int x2,int y2,CELL::Rgba color2,CELL::float2 uv2)
    {
        if (y1 < y2)
        {
            _x1     =   x1;
            _y1     =   y1;
            _uv1    =   uv1;
            _color1 =   color1;

            _x2     =   x2;
            _y2     =   y2;
            _uv2    =   uv2;
            _color2 =   color2;
        }
        else
        {
            _x1     =   x2;
            _y1     =   y2;
            _uv1    =   uv2;
            _color1 =   color2;

            _x2     =   x1;
            _y2     =   y1;
            _uv2    =   uv1;
            _color2 =   color1;
        }
    }
};


struct  Vertex
{
    float       x,y,z;
    float       u,v;
    CELL::Rgba  color;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    uint*   _buffer;
    int     _width;
    int     _height;
    CELL::Rgba    _color;

    IMAGE*  _texture;

    CELL::matrix4 _matModel;
    CELL::matrix4 _matView;
    CELL::matrix4 _matProj;
    CELL::matrix4 _matProjView;
    CELL::float2  _viewPort;
    CELL::Frustum _frust;

    DateElementDes      _poitionPointer;
    DateElementDes      _colorPointer;
    DateElementDes      _uvPointer;

    DateElementDes      _defaultColorPointer;
    DateElementDes      _defaultUVPointer;
    CELL::Rgba          _defaultColorArray[3];
    CELL::float2        _detaultUVArray[3];

    void initData();

    void bindTexture( IMAGE* image );
    void textureCoordPointer( int size,DATETYPE type,int stride,const void* data );
    void colorPointer( int size,DATETYPE type,int stride,const void* data );
    void vertexPointer( int size,DATETYPE type,int stride,const void* data );

    void loadMatrix( const CELL::matrix4& mat );
    void loadIdentity();
    void loadProjMatrix( const CELL::matrix4& mat );
    void loadProjIdentity();
    void loadViewMatrix( const CELL::matrix4& mat );
    void loadViewIdentity();
    void setPerspective( float fovy, float aspect, float zNear, float zFar );
    void lookat( CELL::float3 const & eye,CELL::float3 const & center,CELL::float3 const & up );
    void setViewPort( int x,int y,int w,int h );

    QTimer*timer;
    void displayImage();

    CELL::float3 piplineTransform(CELL::float3 pos);
    void drawTrianle(Ege eges[3]);
    void drawEge(const Ege& e1,const Ege& e2,IMAGE* image);
    void drawSpan(const Span& span,IMAGE* image);
    void drawImage( int startX,int startY,const IMAGE* image );
    void drawArrays( DRAWMODE pri,int start,int count );

    inline void setPixelEx(unsigned x,unsigned y,CELL::Rgba color);

protected slots:
    void onTimeOut();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
