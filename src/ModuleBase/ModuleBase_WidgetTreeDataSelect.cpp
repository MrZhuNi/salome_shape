// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//


#include <ModuleBase_WidgetTreeDataSelect.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ITreeNode.h>

#include <QVBoxLayout>
#include <QTreeView>

#include <cassert>



QVariant ModuleBase_CheckDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return aNode->data(1, theRole);
  }
  return QVariant();
}

QModelIndex ModuleBase_CheckDataModel::index(int theRow, int theCol,
  const QModelIndex& theParent) const
{
  ModuleBase_ITreeNode* aParentNode = (theParent.isValid()) ?
    (ModuleBase_ITreeNode*)theParent.internalPointer() : myRoot;
  ModuleBase_ITreeNode* aSubNode = aParentNode->subNode(theRow);
  assert(aSubNode);
  return createIndex(theRow, theCol, aSubNode);
}

QModelIndex ModuleBase_CheckDataModel::parent(const QModelIndex& theIndex) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    ModuleBase_ITreeNode* aParent = aNode->parent();
    if (aParent == myRoot) {
      return QModelIndex();
    }
    else {
      int aRow = aParent->parent()->nodeRow(aNode);
      return createIndex(aRow, 0, aNode);
    }
  }
  return QModelIndex();
}

int ModuleBase_CheckDataModel::rowCount(const QModelIndex& theParent) const
{
  ModuleBase_ITreeNode* aParentNode = (theParent.isValid()) ?
    (ModuleBase_ITreeNode*)theParent.internalPointer() : myRoot;
  return aParentNode->childrenCount();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
ModuleBase_WidgetTreeDataSelect::ModuleBase_WidgetTreeDataSelect(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop,
  const Config_WidgetAPI* theData)
  :ModuleBase_ModelWidget(theParent, theData), myWorkshop(theWorkshop)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);

  myTreeView = new QTreeView(this);
  myTreeView->setModel(new ModuleBase_CheckDataModel(this, myWorkshop->dataTreeRoot()));
  myTreeView->setHeaderHidden(true);
  myTreeView->setColumnWidth(0, 200);
  aLayout->addWidget(myTreeView);
}

ModuleBase_WidgetTreeDataSelect::~ModuleBase_WidgetTreeDataSelect()
{
}


QList<QWidget*> ModuleBase_WidgetTreeDataSelect::getControls() const
{
  QList<QWidget*> aList;
  aList << myTreeView;
  return aList;
}

bool ModuleBase_WidgetTreeDataSelect::storeValueCustom()
{
  return true;
}

bool ModuleBase_WidgetTreeDataSelect::restoreValueCustom()
{
  return true;
}
