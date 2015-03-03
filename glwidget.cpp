/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <QtGlobal>
#include <math.h>
#include "gl/GLU.h"
//#include <glu.h>
#include "glwidget.h"
#include <QDebug>
#include <QDateTime>

#define PI 3.1459

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
//    gear1 = 0;
//    gear2 = 0;
//    gear3 = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    m_panX = 0;
    m_panY = 0;
    m_time = 0;
    gear1Rot = 0;
    zoomFactor = -30;

//    qDebug() << QColor::colorNames().size();

    for(int i = 0; i < 10; i++)
    {
        pen.append(new Pendulum());
        pen.last()->pt.setX(0);
        pen.last()->pt.setY(0);
        pen.last()->pt.setZ(0 + i*2);
        pen.last()->cord_length = 5 + qreal(i)/5;
        pen.last()->radius = .2 + .01*i;
        pen.last()->theta0 = 15 + i;
    }
    center.setX((pen.first()->pt + pen.last()->pt).x()/2.);
    center.setY((pen.first()->pt + pen.last()->pt).y()/2.);
    center.setZ((pen.first()->pt + pen.last()->pt).z()/2.);

    foreach(Pendulum * p, pen)
    {
//        p->cord_length = 2;
        p->hovered = false;
        p->phase_offset = 0;
//        p->radius = 1;
//        p->theta0 = 30;
    }
    grabKeyboard();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(advanceGears()));
    timer->start(20);
}

GLWidget::~GLWidget()
{
    makeCurrent();
//    glDeleteLists(gear1, 1);
//    glDeleteLists(gear2, 1);
//    glDeleteLists(gear3, 1);
    foreach( Pendulum * p, pen)
    {
        glDeleteLists(p->glQuadric, 1);
    }
}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setReflectance(GLfloat * glcolor, QColor color)
{
    glcolor[0] = color.redF();
    glcolor[2] = color.blueF();
    glcolor[1] = color.greenF();
    glcolor[3] = color.alphaF();
//    glcolor[0] = 0.8f;
//    glcolor[1] = 0.1f;
//    glcolor[2] = 0.0f;
//    glcolor[3] = 1.0f;
}

void GLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;
    static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
//    static const GLfloat reflectance1[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
//    static const GLfloat reflectance2[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
//    static const GLfloat reflectance3[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    QList <QColor> colors;
    colors << Qt::red
           << QColor("orange")
           << Qt::yellow
           << Qt::green
           << Qt::blue
           << QColor("violet");
//    int color_index = 0;
    qsrand((int)QDateTime::currentMSecsSinceEpoch());
    foreach(Pendulum * p, pen)
    {
        setReflectance(p->reflectance, colors.at(qrand()% colors.size()));
//                       QColor(QColor::colorNames().at(
//                                  qrand()%QColor::colorNames().size())));
        p->glQuadric = makePendulum(*p);
    }

//    gear1 = makeGear(reflectance1, 1.0, 4.0, 1.0, 0.7, 20);
//    gear2 = makeGear(reflectance2, 0.5, 2.0, 2.0, 0.7, 10);
//    gear3 = makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 10);

    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glPushMatrix();
    glTranslated(m_panX, m_panY, zoomFactor);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    glTranslated(center.x(), center.y(), center.z());
    m_time += 0.05;
//    qDebug() <<  cos(sqrt(9.8/10)*t)*theta0;
    glRotated(+180.0, 1.0, 0.0, 0.0);

//    int
    foreach(Pendulum * p, pen)
    {
        drawPendulum(*p, cos(sqrt(9.8/p->cord_length)*(m_time + p->phase_offset))*p->theta0);
    }

//    drawPendulum(gear2, +3.1, -2.0, 0.0, -2.0 * (cos(sqrt(9.8/5)*t)*theta0) - 9.0);

//    drawPendulum(gear3, -3.1, -1.8, -2.2, +2.0 * (cos(sqrt(9.8/10)*t)*theta0) - 2.0);

    glPopMatrix();
}

//void GLWidget::resizeGL(int width, int height)
//{
//    int side = qMin(width, height);
//    glViewport((width - side) / 2, (height - side) / 2, side, side);

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslated(0.0, 0.0, -40.0);
//}

void GLWidget::resizeGL(int width, int height)
{
//    qDebug() << Q_FUNC_INFO;
    if(false)
    {
        // make the viewport a fitted square
        int side = qMin(width, height);
        glViewport((width - side) / 2, (height - side) / 2, side, side);
    }
    else
    {
        glViewport(0,0,width,height);//(width - side) / 2, (height - side) / 2, side, side);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // left, right, top bottom near far
    glFrustum(-1.0*(qreal)width/(qreal)height, +1.0*(qreal)width/(qreal)height, -1.0, 1.0, 5.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -40.0);
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
//    e->delta();
    zoomFactor += e->delta()/15/8;

//    qDebug() << "ZoomFactor" << zoomFactor;
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    qreal dx = event->x() - lastPos.x();
    qreal dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::MiddleButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if (event->buttons() & Qt::LeftButton) {
        m_panX += dx/16;
        m_panY -= dy/16;
    }
    lastPos = event->pos();
}

void GLWidget::advanceGears()
{
    gear1Rot += 2 * 16;
    updateGL();
}

GLuint GLWidget::makePendulum(Pendulum p)
{
//    qDebug() << Q_FUNC_INFO;
    const double Pi = 3.14159265358979323846;
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p.reflectance);

    glShadeModel(GL_FLAT);
    GLUquadric * sphere = gluNewQuadric();
    GLUquadric * cylinder = gluNewQuadric();
//    int rad = 1;
    int slices = 15;
    int stacks = slices*2/3;
//    p.radius = 1;
//    p.cord_length = 2;
//    p.phase_offset = 0;
    qreal dx = 0;
    qreal dy = p.cord_length;
    qreal dz = 0;
    glTranslated(dx, dy, dz);
    gluSphere(sphere, p.radius, slices, stacks);
    glRotated(90, 1 ,0, 0);

    GLfloat oldAlpha = p.reflectance[3];
    p.reflectance[3] = 0.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p.reflectance);

    gluCylinder(cylinder, .1, .1, p.cord_length, 4, 1);


    glEndList();

    return list;
}

void GLWidget::drawPendulum(Pendulum p,
                        GLdouble angle)
{
    glPushMatrix();
    glTranslated(p.pt.x(), p.pt.y(), p.pt.z());
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(p.glQuadric);
    glPopMatrix();
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
