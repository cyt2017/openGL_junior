#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initData();

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timer->start(20);
}

MainWindow::~MainWindow()
{
    delete []_buffer;
    timer->deleteLater();
    delete ui;
}

void MainWindow::initData()
{
    _width = 800;
    _height= 600;
    _color = CELL::Rgba(255,0,0);
    _buffer= new uint[_width*_height];
    if(_buffer == NULL)
    {
        return ;
    }
    memset(_buffer,0,_width*_height);

    _texture = NULL;

    _matModel                   =   CELL::matrix4(1);
    _matView                    =   CELL::matrix4(1);
    _matProj                    =   CELL::matrix4(1);
    _matProjView                =   CELL::matrix4(1);

    _defaultColorArray[0] = CELL::Rgba(255,0,0);
    _defaultColorArray[0] = CELL::Rgba(0,255,0);
    _defaultColorArray[0] = CELL::Rgba(0,0,255);

    _detaultUVArray[0] = CELL::float2(0,0);
    _detaultUVArray[1] = CELL::float2(0,1);
    _detaultUVArray[2] = CELL::float2(1,1);

    memset(&_poitionPointer,0,  sizeof(_poitionPointer));
    memset(&_colorPointer,  0,  sizeof(_colorPointer));
    memset(&_uvPointer,     0,  sizeof(_uvPointer));

    _defaultColorPointer._size  =   4;
    _defaultColorPointer._type  =   DT_BYTE;
    _defaultColorPointer._stride=   sizeof(CELL::Rgba);
    _defaultColorPointer._data  =   _defaultColorArray;

    _defaultUVPointer._size     =   2;
    _defaultUVPointer._type     =   DT_FLOAT;
    _defaultUVPointer._stride   =   sizeof(CELL::float2);
    _defaultUVPointer._data     =   _detaultUVArray;
}

void MainWindow::bindTexture(IMAGE *image)
{
    _texture    =   image;
}

void MainWindow::textureCoordPointer(int size, DATETYPE type, int stride, const void *data)
{
    _uvPointer._size        =   size;
    _uvPointer._type        =   type;
    _uvPointer._stride      =   stride;
    _uvPointer._data        =   data;
}

void MainWindow::colorPointer(int size, DATETYPE type, int stride, const void *data)
{
    _colorPointer._size     =   size;
    _colorPointer._type     =   type;
    _colorPointer._stride   =   stride;
    _colorPointer._data     =   data;
}

void MainWindow::vertexPointer(int size, DATETYPE type, int stride, const void *data)
{
    _poitionPointer._size   =   size;
    _poitionPointer._type   =   type;
    _poitionPointer._stride =   stride;
    _poitionPointer._data   =   data;
}

void MainWindow::loadMatrix(const CELL::matrix4 &mat)
{
    _matModel   =   mat;
}

void MainWindow::loadIdentity()
{
    _matModel   =   CELL::matrix4(1);
}

void MainWindow::loadProjMatrix(const CELL::matrix4 &mat)
{
    _matProj    =   mat;
}

void MainWindow::loadProjIdentity()
{
    _matProj    =   CELL::matrix4(1);
}

void MainWindow::loadViewMatrix(const CELL::matrix4 &mat)
{
    _matView    =   mat;
}

void MainWindow::loadViewIdentity()
{
    _matView    =   CELL::matrix4(1);
}

void MainWindow::setPerspective(float fovy, float aspect, float zNear, float zFar)
{
    _matProj    =   CELL::perspective<float>(fovy,aspect,zNear,zFar);
}

void MainWindow::lookat(const CELL::float3 &eye, const CELL::float3 &center, const CELL::float3 &up)
{
    _matView    =   CELL::lookAt(eye,center,up);
}

void MainWindow::setViewPort(int x, int y, int w, int h)
{
    _viewPort.x =   w;
    _viewPort.y =   h;
}

void MainWindow::displayImage()
{
    uchar*dataImg = new uchar[_width*_height*4];
    if(dataImg ==NULL)
    {
        return ;
    }
    memset(dataImg,0,_width*_height*4);

    memcpy(dataImg,_buffer,_width*_height*4);
    QImage img = QImage(dataImg,_width,_height,_width*4,QImage::Format_ARGB32);
    ui->label->setPixmap(QPixmap::fromImage(img));
}


CELL::float3 MainWindow::piplineTransform(CELL::float3 pos)
{
    CELL::float4  world(pos.x,pos.y,pos.z,1);

    CELL::float4  screen  =   _matProjView * world;
    if (screen.w == 0.0f)
    {
        return false;
    }
    screen.x    /=  screen.w;
    screen.y    /=  screen.w;
    screen.z    /=  screen.w;

    // map to range 0 - 1
    screen.x    =   screen.x * 0.5f + 0.5f;
    screen.y    =   screen.y * 0.5f + 0.5f;
    screen.z    =   screen.z * 0.5f + 0.5f;

    // map to viewport
    screen.x    =   screen.x * _viewPort.x;
    screen.y    =   _height - screen.y * _viewPort.y;

    return  CELL::float3(screen.x,screen.y,screen.z);
}
void MainWindow::drawTrianle(Ege eges[3])
{
    int iMax    =   0;
    int length  =   eges[0]._y2 - eges[0]._y1;

    for (int i = 1 ;i < 3 ; ++ i)
    {
        int len =   eges[i]._y2 - eges[i]._y1;
        if (len > length)
        {
            length  =   len;
            iMax    =   i;
        }
    }
    int iShort1 =   (iMax + 1)%3;
    int iShort2 =   (iMax + 2)%3;

    drawEge(eges[iMax],eges[iShort1],_texture);
    drawEge(eges[iMax],eges[iShort2],_texture);
}

void MainWindow::drawEge(const Ege& e1,const Ege& e2,IMAGE* image)
{
    float   yOffset1    =   e1._y2 - e1._y1;
    if (yOffset1 == 0)
    {
        return;
    }

    float   yOffset     =   e2._y2 - e2._y1;
    if (yOffset == 0)
    {
        return;
    }
    float   xOffset =   e2._x2 - e2._x1;
    float   scale   =   0;
    float   step    =   1.0f/yOffset;


    float   xOffset1    =   e1._x2 - e1._x1;
    float   scale1      =   (float)(e2._y1 - e1._y1)/yOffset1;
    float   step1       =   1.0f/yOffset1;

    for (int y = e2._y1 ; y < e2._y2 ; ++ y)
    {
        int     x1      =   e1._x1 + (int)(scale1 * xOffset1);
        int     x2      =   e2._x1 + (int)(scale * xOffset);
        CELL::Rgba    color2  =   CELL::colorLerp(e2._color1,e2._color2,scale);
        CELL::Rgba    color1  =   CELL::colorLerp(e1._color1,e1._color2,scale1);

        CELL::float2  uvStart =   CELL::uvLerp(e1._uv1,e1._uv2,scale1);
        CELL::float2  uvEnd   =   CELL::uvLerp(e2._uv1,e2._uv2,scale);

        Span    span(x1,x2,y,color1,color2,uvStart,uvEnd);
        drawSpan(span,image);

        scale   +=  step;
        scale1  +=  step1;

    }
}

void MainWindow::drawSpan(const Span& span,IMAGE* image)
{
    float   length  =   span._xEnd - span._xStart;
    float   scale   =   0;
    float   step    =   1.0f/length;
    for (int x = span._xStart ; x < span._xEnd; ++ x)
    {
        CELL::Rgba    color   =   CELL::colorLerp(span._colorStart,span._colorEnd,scale);

        CELL::float2  uv      =   CELL::uvLerp(span._uvStart,span._uvEnd,scale);

        CELL::Rgba    pixel   =   image->pixelUV(uv.x,uv.y);

        //Rgba    dst     =   color + pixel;
        scale   +=  step;

        setPixelEx(x,span._y,pixel);
    }
}

void MainWindow::drawImage(int startX, int startY, const IMAGE *image)
{
    int left    =   CELL::tmax<int>(startX,0);
    int top     =   CELL::tmax<int>(startY,0);

    int right   =   CELL::tmin<int>(startX + image->w(),_width);
    int bottom  =   CELL::tmin<int>(startY + image->h(),_height);

    for (int x = left ; x <  right ; ++ x)
    {
        for (int y = top ; y <  bottom ; ++ y)
        {
            CELL::Rgba    color   =   image->setPixel(x - left,y - top);
            setPixelEx(x,y,color);
        }
    }
}

void MainWindow::drawArrays(DRAWMODE pri, int start, int count)
{
    if (_poitionPointer._data == 0)
    {
        return;
    }

    DateElementDes  colorPointerdesc    =   _colorPointer;
    DateElementDes  uvPointerdesc       =   _uvPointer;
    if (colorPointerdesc._data == 0)
    {
        colorPointerdesc    =   _defaultColorPointer;
    }
    if (uvPointerdesc._data == 0)
    {
        uvPointerdesc       =   _defaultUVPointer;
    }
    char*   posData =   (char*)_poitionPointer._data;
    char*   cData   =   (char*)colorPointerdesc._data;
    char*   uvData  =   (char*)uvPointerdesc._data;

    _matProjView    =   _matProj * _matView;

    CELL::matrix4 matPVT  =   _matProjView.transpose();

    _frust.loadFrustum(matPVT);

    for(int i = start ;i < start + count; i += 3)
    {
        float*  fData   =   (float*)posData;
        CELL::float3  p01 (fData[0],fData[1],fData[2]);

        posData +=  _poitionPointer._stride;
        fData   =   (float*)(posData);


        CELL::float3  p11 (fData[0],fData[1],fData[2]);
        posData +=  _poitionPointer._stride;
        fData   =   (float*)(posData);

        CELL::float3  p21 (fData[0],fData[1],fData[2]);
        posData +=  _poitionPointer._stride;

        p01     =   p01 * _matModel;
        p11     =   p11 * _matModel;
        p21     =   p21 * _matModel;

//        if (   _frust.pointInFrustum(p01)
//               || _frust.pointInFrustum(p11)
//               || _frust.pointInFrustum(p21)
//               )
        if(true)
        {

            p01     =   piplineTransform(p01);
            p11     =   piplineTransform(p11);
            p21     =   piplineTransform(p21);
            //! 转化为屏幕坐标
            CELL::int2    p0(p01.x,p01.y);
            CELL::int2    p1(p11.x,p11.y);
            CELL::int2    p2(p21.x,p21.y);

            CELL::Rgba    c0 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;
            CELL::Rgba    c1 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;
            CELL::Rgba    c2 (*(CELL::Rgba*)cData);
            cData   +=  _colorPointer._stride;

            float*  pUV     =   (float*)uvData;
            CELL::float2  uv0 (pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;
            pUV     =   (float*)uvData;
            CELL::float2  uv1(pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;
            pUV     =   (float*)uvData;
            CELL::float2  uv2(pUV[0],pUV[1]);
            uvData  +=  _uvPointer._stride;


            Ege eges[3]  =
            {
                Ege(p0.x,p0.y,c0,  uv0, p1.x,p1.y,c1,  uv1),
                Ege(p1.x,p1.y,c1,  uv1, p2.x,p2.y,c2,  uv2),
                Ege(p2.x,p2.y,c2,  uv2, p0.x,p0.y,c0,  uv0),
            };
            drawTrianle(eges);

            if (_colorPointer._data == 0)
            {
                cData   =   (char*)colorPointerdesc._data;
            }
            if (_uvPointer._data == 0 )
            {
                uvData  =   (char*)uvPointerdesc._data;
            }
        }
    }
}
void MainWindow::setPixelEx(unsigned x,unsigned y,CELL::Rgba color)
{
    _buffer[y * _width + x]   =   color._color;
}

void MainWindow::onTimeOut()
{
    int width = _width;
    int height = _height;
    setViewPort(0,0,width,height);
    setPerspective(60,(float)(width)/(float)(height),0.1,30);
    IMAGE*image = IMAGE::loadFromFile("bg.png");
    IMAGE*image1 = IMAGE::loadFromFile("scale.jpg");

    drawImage(0,0,image);

    Vertex  vertexs[]   =
    {
        {0.0f,  1.0f,   -3.0f,   0.0f,   0.0f,   CELL::Rgba(255,0,0,255)},
        {-1.0f, -1.0f, -3.0f,   1.0f,   1.0f,   CELL::Rgba(0,255,0,255)},
        {1.0f, -1.0f,  -3.0f,   1.0f,   0.0f,   CELL::Rgba(0,0,255,255)},
    };

    CELL::matrix4   matScale;
    CELL::matrix4   matRot;
    CELL::matrix4   matAll;
    CELL::matrix4   matTrans;
    static  float   angle   =   0;
    matRot.rotateZ(angle);
    angle  +=   1.0f;

    matScale.scale(0.5,.5,1);

    matTrans.translate(3,0,0);

    matAll  =  matRot *  matScale * matTrans;
    loadMatrix(matAll);

    image1->setWrapType(1);

    bindTexture(image1);

    vertexPointer(2,DT_FLOAT,      sizeof(Vertex),&vertexs[0].x);
    textureCoordPointer(2,DT_FLOAT,sizeof(Vertex),&vertexs[0].u);
    colorPointer(4,DT_BYTE,        sizeof(Vertex),&vertexs[0].color);

    drawArrays(DM_TRIANGES,0,3);
    displayImage();
}
