// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent)
: QObject(theParent)
{
  // listen to Escape signal to stop the current operation
  qApp->installEventFilter(this);
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

bool XGUI_OperationMgr::hasOperation() const
{
  return (myOperations.count() > 0) && (myOperations.last() != NULL);
}

int XGUI_OperationMgr::operationsCount() const
{
  return myOperations.count();
}

bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (!canStartOperation(theOperation))
    return false;

  myOperations.append(theOperation);

  connect(theOperation, SIGNAL(stopped()), this, SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(started()), this, SIGNAL(operationStarted()));
  connect(theOperation, SIGNAL(resumed()), this, SIGNAL(operationResumed()));
  connect(theOperation, SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)),
          this, SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)));

  theOperation->start();
  return true;
}

bool XGUI_OperationMgr::abortOperation()
{
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (!aCurrentOp || !canStopOperation())
    return false; 

  aCurrentOp->abort();
  return true;
}

QStringList XGUI_OperationMgr::operationList()
{
  QStringList result;
  foreach(ModuleBase_Operation* eachOperation, myOperations) {
    result << eachOperation->id();
  }
  return result;
}

bool XGUI_OperationMgr::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::KeyRelease) {
    QKeyEvent* aKeyEvent = (QKeyEvent*)theEvent;
    if (aKeyEvent && aKeyEvent->key() == Qt::Key_Escape) {
      // TODO: this is Escape button processing when the property panel has empty content,
      // but the operation should be stopped by the Enter has been clicked
      onKeyReleased("", aKeyEvent);
      return true;
    }
  }
  return QObject::eventFilter(theObject, theEvent);
}

void XGUI_OperationMgr::resumeOperation(ModuleBase_Operation* theOperation)
{
  theOperation->resume();
}

bool XGUI_OperationMgr::canStartOperation(ModuleBase_Operation* theOperation)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp && !theOperation->isGranted(aCurrentOp))
  {
    if (canStopOperation()) {
      aCurrentOp->abort();
    } else {
      aCanStart = false;
    }
  }
  return aCanStart;
}

bool XGUI_OperationMgr::canStopOperation()
{
  int anAnswer = QMessageBox::question(0, tr("Operation launch"),
                              tr("Previous operation is not finished and will be aborted"),
                              QMessageBox::Ok, QMessageBox::Cancel);
  return anAnswer == QMessageBox::Ok;
}

void XGUI_OperationMgr::onCommitOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation) {
    if (anOperation->canBeCommitted())
      anOperation->commit();
    else
      anOperation->abort();
  }
}

void XGUI_OperationMgr::onAbortOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->abort();
}

void XGUI_OperationMgr::onOperationStopped()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  ModuleBase_Operation* anOperation = currentOperation();
  if (!aSenderOperation || !anOperation || aSenderOperation != anOperation )
    return;

  myOperations.removeAll(anOperation);
  anOperation->deleteLater();

  emit operationStopped(anOperation);

  // get last operation which can be resumed
  ModuleBase_Operation* aResultOp = 0;
  QListIterator<ModuleBase_Operation*> anIt(myOperations);
  anIt.toBack();
  while(anIt.hasPrevious())
  {
    ModuleBase_Operation* anOp = anIt.previous();
    if (anOp) {
      aResultOp = anOp;
      break;
    }
  }
  if (aResultOp)
    resumeOperation(aResultOp);
}

void XGUI_OperationMgr::onKeyReleased(const std::string& theName, QKeyEvent* theEvent)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->keyReleased(theName, theEvent);
}

void XGUI_OperationMgr::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->onWidgetActivated(theWidget);
}
