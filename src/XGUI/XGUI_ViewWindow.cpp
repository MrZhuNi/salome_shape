#include "XGUI_ViewWindow.h"
#include "XGUI_ViewPort.h"
#include "XGUI_Viewer.h"
#include "XGUI_Tools.h"
#include "XGUI_RubberBand.h"

#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QResizeEvent>
#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>
#include <QTime>

#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#define BORDER_SIZE 2

const char* imageZoomCursor[] = {
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
".#######........................",
"..aaaaaaa.......................",
"................................",
".............#####..............",
"...........##.aaaa##............",
"..........#.aa.....a#...........",
".........#.a.........#..........",
".........#a..........#a.........",
"........#.a...........#.........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
".........#...........#.a........",
".........#a..........#a.........",
".........##.........#.a.........",
"........#####.....##.a..........",
".......###aaa#####.aa...........",
"......###aa...aaaaa.......#.....",
".....###aa................#a....",
"....###aa.................#a....",
"...###aa...............#######..",
"....#aa.................aa#aaaa.",
".....a....................#a....",
"..........................#a....",
"...........................a....",
"................................",
"................................",
"................................",
"................................"};

const char* imageRotateCursor[] = {
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
"................................",
"................................",
"........#.......................",
".......#.a......................",
"......#######...................",
".......#aaaaa#####..............",
"........#..##.a#aa##........##..",
".........a#.aa..#..a#.....##.aa.",
".........#.a.....#...#..##.aa...",
".........#a.......#..###.aa.....",
"........#.a.......#a..#aa.......",
"........#a.........#..#a........",
"........#a.........#a.#a........",
"........#a.........#a.#a........",
"........#a.........#a.#a........",
".........#.........#a#.a........",
"........##a........#a#a.........",
"......##.a#.......#.#.a.........",
"....##.aa..##.....##.a..........",
"..##.aa.....a#####.aa...........",
"...aa.........aaa#a.............",
"................#.a.............",
"...............#.a..............",
"..............#.a...............",
"...............a................",
"................................",
"................................",
"................................",
"................................",
"................................"};

const char* imageCrossCursor[] = {
  "32 32 3 1",
  ". c None",
  "a c #000000",
  "# c #ffffff",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "...............#................",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  ".......#################........",
  "........aaaaaaa#aaaaaaaaa.......",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "................a...............",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................"};


//**************************************************************************
void ViewerToolbar::paintEvent( QPaintEvent* theEvent)
{
    //QTime aTime;
    //aTime.start();
    QRect aRect = rect();
    QRect aVPRect = myVPort->rect();
    QPoint aGlobPnt = mapToGlobal(aRect.topLeft());
    QPoint aPnt = myVPort->mapFromGlobal(aGlobPnt);

    QRect aImgRect(QRect(aPnt.x(), aPnt.y() + aVPRect.height() - aRect.height(), aRect.width(), aRect.height()));
    QPainter(this).drawImage(aRect, myVPort->dumpView(aImgRect, false));
    //QString aMsg = QString("### Painted in %1").arg(aTime.elapsed());
    //qDebug(qPrintable(aMsg));
}

//**************************************************************************
void ViewerLabel::paintEvent( QPaintEvent* theEvent)
{
    QRect aRect = rect();
    QRect aVPRect = myVPort->rect();
    QPoint aGlobPnt = mapToGlobal(aRect.topLeft());
    QPoint aPnt = myVPort->mapFromGlobal(aGlobPnt);

    QRect aImgRect(QRect(aPnt.x(), aPnt.y() + aVPRect.height() - aRect.height(), aRect.width(), aRect.height()));
    QPainter(this).drawImage(aRect, myVPort->dumpView(aImgRect, false));
    QLabel::paintEvent(theEvent);
}

//**************************************************************************
//**************************************************************************
//**************************************************************************
XGUI_ViewWindow::XGUI_ViewWindow(XGUI_Viewer* theViewer, 
                                 V3d_TypeOfView theType):
QFrame(),
    myViewer(theViewer),
    myMoving(false),
    MinimizeIco(":pictures/wnd_minimize.png"),
    MaximizeIco(":pictures/wnd_maximize.png"),
    CloseIco(":pictures/wnd_close.png"),
    RestoreIco(":pictures/wnd_restore.png"),
    myInteractionStyle(XGUI::STANDARD),
    myRectBand(0),
    myIsKeyFree(false),
    my2dMode(XGUI::No2dMode),
    myCurrPointType(XGUI::GRAVITY),
    myPrevPointType(XGUI::GRAVITY),
    myRotationPointSelection(false)
{
    mySelectedPoint = gp_Pnt(0.,0.,0.);
    setFrameStyle(QFrame::Raised);
    setFrameShape(QFrame::Panel);
    setLineWidth(BORDER_SIZE);
    setMouseTracking(true);

    QVBoxLayout* aLay = new QVBoxLayout(this);
    aLay->setContentsMargins(BORDER_SIZE,BORDER_SIZE,BORDER_SIZE,BORDER_SIZE);
    myViewPort = new XGUI_ViewPort(this, myViewer->v3dViewer(), theType);
    myViewPort->installEventFilter(this);
    aLay->addWidget(myViewPort);

    myPicture = new QLabel();
    myPicture->setFrameStyle(QFrame::Sunken);
    myPicture->setFrameShape(QFrame::Panel);
    myPicture->setMouseTracking(true);
    myPicture->installEventFilter(this);
    myPicture->hide();

    QStringList aPictures;
    aPictures<<":pictures/occ_view_camera_dump.png"<<":pictures/occ_view_style_switch.png";
    aPictures<<":pictures/occ_view_triedre.png"<<":pictures/occ_view_fitall.png";
    aPictures<<":pictures/occ_view_fitarea.png"<<":pictures/occ_view_zoom.png";
    aPictures<<":pictures/occ_view_pan.png"<<":pictures/occ_view_glpan.png";
    aPictures<<":pictures/occ_view_rotate.png"<<":pictures/occ_view_front.png";
    aPictures<<":pictures/occ_view_back.png"<<":pictures/occ_view_left.png";
    aPictures<<":pictures/occ_view_right.png"<<":pictures/occ_view_top.png";
    aPictures<<":pictures/occ_view_bottom.png"<<":pictures/occ_view_clone.png";

    QStringList aTitles;
    aTitles << "Dump view" << "Mouse style switch" << "Show trihedron" << "Fit all";
    aTitles << "Fit area" << "Zoom" << "Panning" << "Global panning" << "Rotate";
    aTitles << "Front" << "Back" << "Left" << "Right" << "Top" << "Bottom" << "Clone view";

    myGripWgt = new ViewerLabel(this, myViewPort);
    myGripWgt->setPixmap(QPixmap(":pictures/wnd_grip.png"));
    myGripWgt->setGeometry(BORDER_SIZE + 2, BORDER_SIZE + 2, 19, 32);
    myGripWgt->setMouseTracking(true);
    myGripWgt->installEventFilter(this);
    connect(myViewPort, SIGNAL(vpTransformed()), myGripWgt, SLOT(update()));
    connect(myViewPort, SIGNAL(vpUpdated()), myGripWgt, SLOT(update()));

    myViewBar = new ViewerToolbar(this, myViewPort);

    QAction* aBtn;
    for (int i = 0; i < aTitles.length(); i++) {
        aBtn = new QAction(QIcon(aPictures.at(i)), aTitles.at(i), myViewBar);
        myViewBar->addAction(aBtn);
    }
    connect(myViewPort, SIGNAL(vpTransformed()), myViewBar, SLOT(update()));
    connect(myViewPort, SIGNAL(vpUpdated()), myViewBar, SLOT(update()));

    myWindowBar = new ViewerToolbar(this, myViewPort);
    connect(myViewPort, SIGNAL(vpTransformed()), myWindowBar, SLOT(update()));
    connect(myViewPort, SIGNAL(vpUpdated()), myWindowBar, SLOT(update()));

    myMinimizeBtn = new QAction(myWindowBar);
    myMinimizeBtn->setIcon(MinimizeIco);
    myWindowBar->addAction(myMinimizeBtn);
    connect(myMinimizeBtn, SIGNAL(triggered()), SLOT(onMinimize()));

    myMaximizeBtn = new QAction(myWindowBar);
    myMaximizeBtn->setIcon(MaximizeIco);
    myWindowBar->addAction(myMaximizeBtn);
    connect(myMaximizeBtn, SIGNAL(triggered()), SLOT(onMaximize()));

    aBtn = new QAction(myWindowBar);
    aBtn->setIcon(CloseIco);
    myWindowBar->addAction(aBtn);
    connect(aBtn, SIGNAL(triggered()), SLOT(onClose()));

    myViewBar->hide();
    myWindowBar->hide();
    myGripWgt->hide();
}

//****************************************************************
XGUI_ViewWindow::~XGUI_ViewWindow()
{
}

//****************************************************************
void XGUI_ViewWindow::resizeEvent(QResizeEvent* theEvent)
{
    QSize aSize = theEvent->size();
    QSize aWndBarSize = myWindowBar->sizeHint();
    QSize myViewBarSize = myViewBar->sizeHint();

    myWindowBar->move(aSize.width() - aWndBarSize.width() - BORDER_SIZE - 4, BORDER_SIZE + 2);
    int aViewBarWidth = aSize.width() - aWndBarSize.width() - myGripWgt->width() - 8;
    if (aViewBarWidth > myViewBarSize.width())
        aViewBarWidth = myViewBarSize.width();
    myViewBar->setGeometry(BORDER_SIZE + 18, BORDER_SIZE + 2, aViewBarWidth, myViewBarSize.height());
}

//****************************************************************
void XGUI_ViewWindow::changeEvent(QEvent* theEvent)
{

    if (theEvent->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            if (!myPicture->parentWidget()) {
                QMdiSubWindow* aParent = static_cast<QMdiSubWindow*>(parentWidget());
                QMdiArea* aMDIArea = aParent->mdiArea();
                myPicture->setParent(aMDIArea);
            }
            myPicture->move(parentWidget()->x(), parentWidget()->y());
            myPicture->show();
        } else {
            myPicture->hide();
            if (isMaximized()) {
                myMinimizeBtn->setIcon(MinimizeIco);
                myMaximizeBtn->setIcon(RestoreIco);
            }
        }
    } else
        QWidget::changeEvent(theEvent);
}

//****************************************************************
void XGUI_ViewWindow::onClose()
{
    if (parentWidget())
        parentWidget()->close();

}

//****************************************************************
void XGUI_ViewWindow::enterEvent(QEvent* theEvent)
{
    if (!isMinimized()) {
        myViewBar->show();
        if (!isMaximized())
            myGripWgt->show(); 
    }
    myWindowBar->show();
}

//****************************************************************
void XGUI_ViewWindow::leaveEvent(QEvent* theEvent)
{
    myViewBar->hide();
    myGripWgt->hide(); 
    myWindowBar->hide();
}

//****************************************************************
void XGUI_ViewWindow::onMinimize()
{
    QPixmap aPMap = QPixmap::fromImage(myViewPort->dumpView());
    int aW = width();
    int aH = height();
    double aR = aW / 100.;
    myPicture->setPixmap(aPMap.scaled(100,  int(aH / aR)));
    
    myLastState = isMaximized()? MaximizedState : NormalState;
    showMinimized();
}

//****************************************************************
void XGUI_ViewWindow::onMaximize()
{
    if (isMaximized()) {
        myMaximizeBtn->setIcon(MaximizeIco);
        myGripWgt->show();
        showNormal();
    } else {
        myMaximizeBtn->setIcon(RestoreIco);
        myGripWgt->hide();
        showMaximized();
    }
    myMinimizeBtn->setIcon(MinimizeIco);
}

//****************************************************************
bool XGUI_ViewWindow::processWindowControls(QObject *theObj, QEvent *theEvent)
{
    QWidget* aWgt = (theObj == myPicture)? myPicture : static_cast<QWidget*>(parentWidget());
    switch (theEvent->type()) {
    case QEvent::MouseButtonPress: 
        {
            QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
            if ((aEvent->button() == Qt::LeftButton) && (!myMoving)){
                myMoving = true;
                myMousePnt = aEvent->globalPos();
                return true;
            }
        }
        break;
    case QEvent::MouseButtonRelease: 
        {
            QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
            if ((aEvent->button() == Qt::LeftButton) && myMoving) {
                myMoving = false;
                return true;
            }
        }
        break;
    case QEvent::MouseMove: 
        {
            QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
            if (myMoving) {
                QMdiSubWindow* aParent = static_cast<QMdiSubWindow*>(parentWidget());
                QMdiArea* aMDIArea = aParent->mdiArea();
                    
                QPoint aPnt = aEvent->globalPos();
                QPoint aMDIPnt = aMDIArea->mapFromGlobal(aPnt);
                if (aMDIArea->rect().contains(aMDIPnt)) {
                    int aX = aWgt->x() + (aPnt.x() - myMousePnt.x());
                    int aY = aWgt->y() + (aPnt.y() - myMousePnt.y());
                    aWgt->move(aX, aY);
                    myMousePnt = aPnt;
                }
                return true;
            }
        }
        break;
    case QEvent::MouseButtonDblClick:
        if (theObj == myPicture) {
            myMoving = false;
            if (myLastState == MaximizedState)
                showMaximized();
            else
                showNormal();
            return true;
        }
    }
    return false;
}

//****************************************************************
bool XGUI_ViewWindow::processViewPort(QEvent *theEvent)
{
    switch(theEvent->type()) {
    case QEvent::MouseButtonPress:
        vpMousePressEvent((QMouseEvent*) theEvent);
        return true;

    case QEvent::MouseButtonRelease:
        vpMouseReleaseEvent((QMouseEvent*) theEvent);
        return true;

    case QEvent::MouseMove:
        vpMouseMoveEvent((QMouseEvent*) theEvent);
        return true;

    case QEvent::MouseButtonDblClick:
        emit mouseDoubleClicked(this, (QMouseEvent*)theEvent);
        return true;
    }
    return false;
}

//****************************************************************
bool XGUI_ViewWindow::eventFilter(QObject *theObj, QEvent *theEvent)
{
    if ((theObj == myGripWgt) || (theObj == myPicture)) {
        if (processWindowControls(theObj, theEvent) )
            return true;
    } else if (theObj == myViewPort) {
        if (processViewPort(theEvent))
            return true;
    }
    return QFrame::eventFilter(theObj, theEvent);
}

//****************************************************************
XGUI_ViewWindow::OperationType XGUI_ViewWindow::getButtonState(QMouseEvent* theEvent, 
                                XGUI::InteractionStyle theInteractionStyle)
{
  OperationType aOp = NOTHING;
  XGUI::InteractionStyle aStyle = (XGUI::InteractionStyle)theInteractionStyle;
  if( (theEvent->modifiers() == XGUI_Viewer::myStateMap[aStyle][XGUI::ZOOM]) &&
      (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::ZOOM]) )
    aOp = ZOOMVIEW;
  else if( (theEvent->modifiers() == XGUI_Viewer::myStateMap[aStyle][XGUI::PAN]) &&
           (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::PAN]) )
    aOp = PANVIEW;
  else if( (theEvent->modifiers()  == XGUI_Viewer::myStateMap[aStyle][XGUI::ROTATE]) &&
           (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::ROTATE]) &&
           (my2dMode == XGUI::No2dMode))
    aOp = ROTATE;

  return aOp;
}

//****************************************************************
void XGUI_ViewWindow::vpMousePressEvent(QMouseEvent* theEvent)
{
    myStartX = theEvent->x();
    myStartY = theEvent->y();
    XGUI::InteractionStyle anInteractionStyle = interactionStyle();

    // in "key free" interaction style zoom operation is activated by two buttons (simultaneously pressed),
    // which are assigned for pan and rotate - these operations are activated immediately after pressing 
    // of the first button, so it is necessary to switch to zoom when the second button is pressed
    bool aSwitchToZoom = false;
    if ((anInteractionStyle == XGUI::KEY_FREE) && (myOperation == PANVIEW || myOperation == ROTATE)) {
        aSwitchToZoom = getButtonState( theEvent, anInteractionStyle ) == ZOOMVIEW;
    }

    switch ( myOperation ) {
    case WINDOWFIT:
        if ( theEvent->button() == Qt::LeftButton )
            emit vpTransformationStarted ( WINDOWFIT );
        break;

    case PANGLOBAL:
        if ( theEvent->button() == Qt::LeftButton )
            emit vpTransformationStarted ( PANGLOBAL );
        break;

    case ZOOMVIEW:
        if ( theEvent->button() == Qt::LeftButton ) {
            myViewPort->startZoomAtPoint( myStartX, myStartY );
            emit vpTransformationStarted ( ZOOMVIEW );
        }
        break;

    case PANVIEW:
        if ( aSwitchToZoom ) {
            myViewPort->startZoomAtPoint( myStartX, myStartY );
            activateZoom();
        } else if ( theEvent->button() == Qt::LeftButton )
            emit vpTransformationStarted ( PANVIEW );
        break;

    case ROTATE:
        if ( aSwitchToZoom ) {
            myViewPort->startZoomAtPoint( myStartX, myStartY );
            activateZoom();
        } else if ( theEvent->button() == Qt::LeftButton ) {
            myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
            emit vpTransformationStarted ( ROTATE );
        }
        break;

    default:
        /*  Try to activate a transformation */
        OperationType aState;
        if ( interactionStyle() == XGUI::STANDARD )
            aState = getButtonState(theEvent, anInteractionStyle);
        else {
            aState = XGUI_ViewWindow::NOTHING;
            myIsKeyFree = true;
        }
        switch ( aState ) {
        case ZOOMVIEW:
            myViewPort->startZoomAtPoint( myStartX, myStartY );
            activateZoom();
            break;
        case PANVIEW:
            activatePanning();
            break;
        case ROTATE:
            activateRotation();
            myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
            break;
        default:
            if ( myRotationPointSelection ) {
                if ( theEvent->button() == Qt::LeftButton ) {
                    Handle(AIS_InteractiveContext) ic = myViewer->AISContext();
                    ic->Select();
                    for ( ic->InitSelected(); ic->MoreSelected(); ic->NextSelected() ) {
                        TopoDS_Shape aShape = ic->SelectedShape();
                        if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX ) {
                            gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
                            /*if ( mySetRotationPointDlg ) {
                                myRotationPointSelection = false;
                                mySetRotationPointDlg->setCoords(aPnt.X(), aPnt.Y(), aPnt.Z());
                            }*/
                        } else {
                            myCurrPointType = myPrevPointType;
                            break;
                        }
                    }
                    if ( ic->NbSelected() == 0 ) myCurrPointType = myPrevPointType;
                    //if ( mySetRotationPointDlg ) mySetRotationPointDlg->toggleChange();
                    ic->CloseAllContexts();
                    myOperation = NOTHING;
                    myViewPort->setCursor( myCursor );
                    myCursorIsHand = false;
                    myRotationPointSelection = false;
                }
            } else
                emit mousePressed(this, theEvent);
            break;
        }
        /* notify that we start a transformation */
        if ( transformRequested() )
            emit vpTransformationStarted ( myOperation );
    }
    if ( transformRequested() )
        setTransformInProcess( true );

    /* we may need it for sketching... */
/*    if ( l_mbPressEvent )
        delete l_mbPressEvent;
    l_mbPressEvent = new QMouseEvent( *theEvent );*/
}

//****************************************************************
void XGUI_ViewWindow::vpMouseReleaseEvent(QMouseEvent* theEvent)
{
    switch ( myOperation ) {
    case NOTHING:
        {
            int prevState = myCurSketch;
/*            if(theEvent->button() == Qt::RightButton) {
                QList<OCCViewer_ViewSketcher*>::Iterator it;
                for ( it = mySketchers.begin(); it != mySketchers.end() && myCurSketch != -1; ++it ) {
                    OCCViewer_ViewSketcher* sk = (*it);
                    if( ( sk->sketchButton() & theEvent->button() ) && sk->sketchButton() == myCurSketch )
                        myCurSketch = -1;
                }
            }
            */
            emit mouseReleased(this, theEvent);
            if (theEvent->button() == Qt::RightButton && prevState == -1) {
                QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                                          theEvent->pos(), theEvent->globalPos() );
                emit contextMenuRequested( &aEvent );
            }
        }
        break;
    case ROTATE:
        myViewPort->endRotation();
        resetState();
        break;

    case PANVIEW:
    case ZOOMVIEW:
        resetState();
        break;

    case PANGLOBAL:
        if ( theEvent->button() == Qt::LeftButton ) {
            myViewPort->setCenter( theEvent->x(), theEvent->y() );
            myViewPort->getView()->SetScale(myCurScale);
            resetState();
        }
        break;

    case WINDOWFIT:
        if ( theEvent->button() == Qt::LeftButton ) {
            myCurrX = theEvent->x();
            myCurrY = theEvent->y();
            drawRect();
            QRect rect = makeRect(myStartX, myStartY, myCurrX, myCurrY);
            if ( !rect.isEmpty() ) myViewPort->fitRect(rect);
            endDrawRect();
            resetState();
        }
        break;
    }

    // NOTE: viewer 3D detects a rectangle of selection using this event
    // so we must emit it BEFORE resetting the selection rectangle
    if ( theEvent->button() == Qt::LeftButton && myDrawRect ) {
        drawRect();
        endDrawRect();
        resetState();
        myViewPort->update();
    }
/*    if ( l_mbPressEvent ) {
        delete l_mbPressEvent;
        l_mbPressEvent = 0;
    }*/
}

//****************************************************************
void XGUI_ViewWindow::vpMouseMoveEvent(QMouseEvent* theEvent)
{
    if ( myIsKeyFree && interactionStyle() == XGUI::KEY_FREE ) {
        myIsKeyFree = false;
        switch ( getButtonState( theEvent, interactionStyle() ) ) {
        case ZOOMVIEW:
            myViewPort->startZoomAtPoint( myStartX, myStartY );
            activateZoom();
            break;
        case PANVIEW:
            activatePanning();
            break;
        case ROTATE:
            activateRotation();
            myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
            break;
        default:
        break;
        }
    }

    myCurrX = theEvent->x();
    myCurrY = theEvent->y();
    switch (myOperation) {
    case ROTATE:
        myViewPort->rotate(myCurrX, myCurrY, myCurrPointType, mySelectedPoint);
        break;

    case ZOOMVIEW:
        myViewPort->zoom(myStartX, myStartY, myCurrX, myCurrY);
        myStartX = myCurrX;
        myStartY = myCurrY;
        break;

    case PANVIEW:
        myViewPort->pan(myCurrX - myStartX, myStartY - myCurrY);
        myStartX = myCurrX;
        myStartY = myCurrY;
        break;

    case PANGLOBAL:
        break;

    default:
        if ( myRotationPointSelection /*|| isSketcherStyle()*/ )  {
            emit mouseMoving( this, theEvent );
        } else {
            int aState = theEvent->modifiers();
            int aButton = theEvent->buttons();
            int anInteractionStyle = interactionStyle();
            if ( ( (anInteractionStyle == XGUI::STANDARD) &&
                   (aButton == Qt::LeftButton) && 
                   (aState == Qt::NoModifier || Qt::ShiftModifier) ) ||
                 ( (anInteractionStyle == XGUI::KEY_FREE) &&
                   (aButton == Qt::LeftButton) && 
                 ( aState == Qt::ControlModifier || aState == (Qt::ControlModifier|Qt::ShiftModifier) ) ) ) {
                myDrawRect = myEnableDrawMode;
                if ( myDrawRect ) {
                    drawRect();
                    if ( !myCursorIsHand ) {   // we are going to sketch a rectangle
                        QCursor handCursor (Qt::PointingHandCursor);
                        myCursorIsHand = true;
                        myCursor = cursor();
                        myViewPort->setCursor( handCursor );
                    }
                }
                emit mouseMoving( this, theEvent );
            } /* else if ( ( (anInteractionStyle == XGUI::STANDARD) &&
                          (aButton == Qt::RightButton) && 
                         ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) ||
                         ( (anInteractionStyle == XGUI::KEY_FREE) &&
                           (aButton == Qt::RightButton) && 
                         ( aState == Qt::ControlModifier || aState == ( Qt::ControlModifier|Qt::ShiftModifier ) ) ) ) {
                OCCViewer_ViewSketcher* sketcher = 0;
                QList<OCCViewer_ViewSketcher*>::Iterator it;
                for ( it = mySketchers.begin(); it != mySketchers.end() && !sketcher; ++it ) {
                    OCCViewer_ViewSketcher* sk = (*it);
                    if( sk->isDefault() && sk->sketchButton() == aButton )
                        sketcher = sk;
                }
                if ( sketcher && myCurSketch == -1 ) {
                    activateSketching( sketcher->type() );
                    if ( mypSketcher ) {
                        myCurSketch = mypSketcher->sketchButton();

                        if ( l_mbPressEvent )  {
                            QApplication::sendEvent( getViewPort(), l_mbPressEvent );
                            delete l_mbPressEvent;
                            l_mbPressEvent = 0;
                        }
                        QApplication::sendEvent( getViewPort(), theEvent );
                    }
                }
            } */else
                emit mouseMoving( this, theEvent );
        }
    }
}


/*!
  \brief Draw rubber band rectangle.
*/
void XGUI_ViewWindow::drawRect()
{
    if ( !myRectBand ) {
        myRectBand = new XGUI_RectRubberBand( myViewPort );
    }

    myRectBand->setUpdatesEnabled ( false );
    QRect aRect = makeRect(myStartX, myStartY, myCurrX, myCurrY);
    myRectBand->initGeometry( aRect );

    if ( !myRectBand->isVisible() )
        myRectBand->show();

    myRectBand->setUpdatesEnabled ( true );
}

/*!
  \brief Clear rubber band rectangle on the end on the dragging operation.
*/
void XGUI_ViewWindow::endDrawRect()
{
    if ( myRectBand ) {
        myRectBand->clearGeometry();
        myRectBand->hide();
    }
}

void XGUI_ViewWindow::activateZoom()
{
    if ( !transformRequested() && !myCursorIsHand )
        myCursor = cursor();                /* save old cursor */

    if ( myOperation != ZOOMVIEW ) {
        QPixmap zoomPixmap (imageZoomCursor);
        QCursor zoomCursor (zoomPixmap);
        if( setTransformRequested ( ZOOMVIEW ) )
            myViewPort->setCursor( zoomCursor );
    }
}

bool XGUI_ViewWindow::transformRequested() const
{
  return ( myOperation != NOTHING );
}

/*!
  \brief Start delayed viewer operation.
*/
bool XGUI_ViewWindow::setTransformRequested( OperationType op )
{
  bool ok = transformEnabled( op );
  myOperation = ok ? op : NOTHING;
  myViewPort->setMouseTracking( myOperation == NOTHING );  
  return ok;
}

/*!
  Set enabled state of transformation (rotate, zoom, etc)
*/
void XGUI_ViewWindow::setTransformEnabled( const OperationType id, const bool on )
{
    if ( id != NOTHING ) myStatus.insert( id, on );
}

/*!
  \return enabled state of transformation (rotate, zoom, etc)
*/
bool XGUI_ViewWindow::transformEnabled( const OperationType id ) const
{
    return myStatus.contains( id ) ? myStatus[ id ] : true;
}


/*!
  \brief Start panning operation.

  Sets the corresponding cursor for the widget.
*/
void XGUI_ViewWindow::activatePanning()
{
    if ( !transformRequested() && !myCursorIsHand )
        myCursor = cursor();                // save old cursor

    if ( myOperation != PANVIEW ) {
        QCursor panCursor (Qt::SizeAllCursor);
        if( setTransformRequested ( PANVIEW ) )
            myViewPort->setCursor( panCursor );
    }
}

/*!
  \brief Start rotation operation

  Sets the corresponding cursor for the widget.
*/
void XGUI_ViewWindow::activateRotation()
{
    if ( !transformRequested() && !myCursorIsHand )
        myCursor = cursor();                // save old cursor

    if ( myOperation != ROTATE ) {
        QPixmap rotatePixmap (imageRotateCursor);
        QCursor rotCursor (rotatePixmap);
        if( setTransformRequested ( ROTATE ) )
        myViewPort->setCursor( rotCursor );
    }
}

/*!
  \brief Reset the viewport to its initial state
  ( no transformations in process etc. )
*/
void XGUI_ViewWindow::resetState()
{
    myDrawRect = false;

    if ( myRotationPointSelection ) {
        QCursor handCursor (Qt::PointingHandCursor);
        myViewPort->setCursor( handCursor );
    } else {
        if ( transformRequested() || myCursorIsHand )
            myViewPort->setCursor( myCursor );
        myCursorIsHand = false;
    }

    if ( transformRequested() )
        emit vpTransformationFinished (myOperation);

    setTransformInProcess( false );
    setTransformRequested( NOTHING );
}
