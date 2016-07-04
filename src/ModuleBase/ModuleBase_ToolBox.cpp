// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ToolBox.cpp
// Created:     10 August 2015
// Author:      Alexandre SOLOVYOV

#include <ModuleBase_ToolBox.h>
#include <ModuleBase_ModelWidget.h>

#include <QButtonGroup>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

ModuleBase_ToolBox::ModuleBase_ToolBox( QWidget* theParent )
  : QFrame( theParent )
{
  QVBoxLayout* aMainLayout = new QVBoxLayout( this );
  aMainLayout->setMargin(0);
  aMainLayout->setSpacing(2);

  myButtonsFrame = new QFrame( this );

  myStack = new QStackedWidget( this );

  aMainLayout->addWidget( myButtonsFrame, 0 );
  aMainLayout->addWidget( myStack, 1 );

  myButtonsGroup = new QButtonGroup(this);
  myButtonsGroup->setExclusive( true );
  myButtonsLayout = new QHBoxLayout( myButtonsFrame );
  myButtonsLayout->setMargin( 0 );
  myButtonsLayout->setSpacing( 5 );
  myButtonsLayout->addStretch( 1 );

  connect( myStack, SIGNAL( currentChanged( int ) ), this, SIGNAL( currentChanged( int ) ) );
  connect( myButtonsGroup, SIGNAL( buttonPressed( int ) ), this, SLOT( onButton( int ) ) );
}

ModuleBase_ToolBox::~ModuleBase_ToolBox()
{
}

void ModuleBase_ToolBox::addItem( QWidget* thePage, const QString& theName, const QPixmap& theIcon )
{
  int anOldCount = myStack->count();

  myStack->addWidget( thePage );

  QToolButton* aButton = new QToolButton( myButtonsFrame );
  aButton->setFocusPolicy(Qt::StrongFocus);
  aButton->setCheckable( true );
  aButton->setIcon( theIcon );
  aButton->setIconSize( theIcon.size() );
  aButton->setToolTip( theName );
  aButton->setObjectName( theName );
  myButtonsGroup->addButton( aButton, anOldCount );
  myButtonsLayout->insertWidget( anOldCount, aButton );
}

int ModuleBase_ToolBox::count() const
{
  return myStack->count();
}

int ModuleBase_ToolBox::currentIndex() const
{
  return myStack->currentIndex();
}

void ModuleBase_ToolBox::setCurrentIndex( const int theIndex )
{
  myStack->setCurrentIndex( theIndex );
  myButtonsGroup->button( theIndex )->setChecked( true );
}

void ModuleBase_ToolBox::onButton( int theIndex )
{
  myStack->setCurrentIndex( theIndex );
}

bool ModuleBase_ToolBox::isOffToolBoxParent(ModuleBase_ModelWidget* theWidget)
{
  bool isOffToolBox = false;

  QList<QWidget*> aControls = theWidget->getControls();
  if (aControls.size() > 0) {
    QWidget* aFirstControl = aControls.first();

    QWidget* aWidget = aFirstControl;
    QWidget* aParent = (QWidget*)aFirstControl->parent();
    while (aParent) {
      QStackedWidget* aStackedWidget = dynamic_cast<QStackedWidget*>(aParent);
      if (aStackedWidget) {
        int anIndex = aStackedWidget->currentIndex();
        isOffToolBox = aStackedWidget->currentWidget() != aWidget;
        break;
      }
      aWidget = aParent;
      aParent = (QWidget*)aWidget->parent();
    }
  }
  return isOffToolBox;
}
