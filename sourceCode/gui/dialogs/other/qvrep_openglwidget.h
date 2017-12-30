// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2017 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.4.0 rev. 1 on April 5th 2017

#ifndef QVREP_OPENGLWIDGET_H
#define QVREP_OPENGLWIDGET_H

#ifdef USING_QOPENGLWIDGET
#include <QOpenGLWidget>
#else
#include <QGLWidget>
#endif
#include <QString>

struct SModelThumbnailInfo;

struct SMouseOrKeyboardOrResizeEvent
{
    int eventType;  // 0=mouse press, 1=mouse release, 2=mouse dbl click,
                    // 3=mouse move, 4=mouse wheel, 5=key press,
                    // 6=key release, 7=resize, 8=timer
    int button;
    int x;
    int y;
    bool ctrlDown;
    bool shiftDown;
    int wheelDelta;
    int key;
    int specialKey;
    QString unicodeText;
};

#ifdef USING_QOPENGLWIDGET
class COpenglWidget : public QOpenGLWidget
#else
class COpenglWidget : public QGLWidget
#endif
{
    Q_OBJECT
public:
    explicit COpenglWidget(QWidget *parent = 0);
    ~COpenglWidget();

    void makeContextCurrent();

    void dragEnterEvent(QDragEnterEvent* dEvent);
    void dragLeaveEvent(QDragLeaveEvent* dEvent);
    void dragMoveEvent(QDragMoveEvent* dEvent);
    void dropEvent(QDropEvent* dEvent);

    SModelThumbnailInfo* getModelDragAndDropInfo();
    void clearModelDragAndDropInfo();


protected:
    void _setCtrlAndShiftKeyState(bool ctrlDown,bool shiftDown);
    void _computeMousePos(int inX,int inY,int& outX,int& outY);
    void _handleMouseAndKeyboardAndResizeEvents(void* event,int t);

    void mousePressEvent(QMouseEvent* mEvent);
    void _mousePressEvent(SMouseOrKeyboardOrResizeEvent e);
    void mouseReleaseEvent(QMouseEvent* mEvent);
    void _mouseReleaseEvent(SMouseOrKeyboardOrResizeEvent e);
    void mouseDoubleClickEvent(QMouseEvent* mEvent);
    void _mouseDoubleClickEvent(SMouseOrKeyboardOrResizeEvent e);
    void mouseMoveEvent(QMouseEvent* mEvent);
    void _mouseMoveEvent(SMouseOrKeyboardOrResizeEvent e);

    void wheelEvent(QWheelEvent* wEvent);
    void _wheelEvent(SMouseOrKeyboardOrResizeEvent e);

    bool focusNextPrevChild(bool next);
    void keyPressEvent(QKeyEvent* kEvent);
    void _keyPressEvent(SMouseOrKeyboardOrResizeEvent e);
    void keyReleaseEvent(QKeyEvent* kEvent);
    void _keyReleaseEvent(SMouseOrKeyboardOrResizeEvent e);

    void resizeEvent(QResizeEvent* rEvent);
    void _resizeEvent(SMouseOrKeyboardOrResizeEvent e);


    void initializeGL();
    void paintGL();

    void paintEvent(QPaintEvent* event);

    int _lastGlobalMousePos[2];
    SModelThumbnailInfo* _modelDragAndDropInfo;
signals:

private slots:
    void _timer100ms_fire();

};
#endif // QVREP_OPENGLWIDGET_H
