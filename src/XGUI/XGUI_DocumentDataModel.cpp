#include "XGUI_DocumentDataModel.h"
#include "XGUI_PartDataModel.h"
#include "XGUI_Workshop.h"
#include "XGUI_Tools.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>

#include <Events_Loop.h>

#include <Config_FeatureMessage.h>

#include <QIcon>
#include <QString>
#include <QBrush>

#include <set>

#define ACTIVE_COLOR QColor(0,72,140)
#define PASSIVE_COLOR Qt::black

XGUI_DocumentDataModel::XGUI_DocumentDataModel(QObject* theParent)
    : QAbstractItemModel(theParent),
      myActivePart(0)
{
  // Register in event loop
  //Events_Loop* aLoop = Events_Loop::loop();
  //aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  //aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  //aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  // Create a top part of data tree model
  myModel = new XGUI_TopDataModel(this);
  myModel->setItemsColor(ACTIVE_COLOR);
}

XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
  clearModelIndexes();
}

void XGUI_DocumentDataModel::processEvent(const Events_Message* theMessage)
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();

  // Created object event *******************
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    const ModelAPI_ObjectUpdatedMessage* aUpdMsg =
        dynamic_cast<const ModelAPI_ObjectUpdatedMessage*>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature && (!aFeature->isInHistory()))
        continue;

      DocumentPtr aDoc = aObject->document();
      if (aDoc == aRootDoc) {  // If root objects
        if (aObject->groupName() == ModelAPI_ResultPart::group()) {  // Update only Parts group
            // Add a new part
          int aStart = myPartModels.size();
          XGUI_PartDataModel* aModel = new XGUI_PartDataModel(this);
          aModel->setPartId(myPartModels.count());
          myPartModels.append(aModel);
          insertRow(aStart, partFolderNode());
        } else {  // Update top groups (other except parts
          QModelIndex aIndex = myModel->findParent(aObject);
          int aStart = myModel->rowCount(aIndex) - 1;
          if (aStart < 0)
            aStart = 0;
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          insertRow(aStart, aIndex);
        }
      } else {  // if sub-objects of first level nodes
        XGUI_PartModel* aPartModel = 0;
        QList<XGUI_PartModel*>::const_iterator aIt;
        for (aIt = myPartModels.constBegin(); aIt != myPartModels.constEnd(); ++aIt) {
          if ((*aIt)->hasDocument(aDoc)) {
            aPartModel = (*aIt);
            break;
          }
        }
        if (aPartModel) {
          QModelIndex aIndex = aPartModel->findParent(aObject);
          int aStart = aPartModel->rowCount(aIndex);  // check this index
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          insertRow(aStart, aIndex);
        }
      }
    }
    // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    const ModelAPI_ObjectDeletedMessage* aUpdMsg =
        dynamic_cast<const ModelAPI_ObjectDeletedMessage*>(theMessage);
    DocumentPtr aDoc = aUpdMsg->document();
    std::set<std::string> aGroups = aUpdMsg->groups();

    std::set<std::string>::const_iterator aIt;
    for (aIt = aGroups.begin(); aIt != aGroups.end(); ++aIt) {
      std::string aGroup = (*aIt);
      if (aDoc == aRootDoc) {  // If root objects
        if (aGroup == ModelAPI_ResultPart::group()) {  // Update only Parts group
          int aStart = myPartModels.size() - 1;
          removeSubModel(aStart);
          removeRow(aStart, partFolderNode());
          if (myActivePart && (!isPartSubModel(myActivePart))) {
            myActivePart = 0;
            myActivePartIndex = QModelIndex();
            myModel->setItemsColor(ACTIVE_COLOR);
          }
        } else {  // Update top groups (other except parts
          QModelIndex aIndex = myModel->findGroup(aGroup);
          int aStart = myModel->rowCount(aIndex);
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          removeRow(aStart, aIndex);
        }
      } else {
        XGUI_PartModel* aPartModel = 0;
        QList<XGUI_PartModel*>::const_iterator aIt;
        for (aIt = myPartModels.constBegin(); aIt != myPartModels.constEnd(); ++aIt) {
          if ((*aIt)->hasDocument(aDoc)) {
            aPartModel = (*aIt);
            break;
          }
        }
        if (aPartModel) {
          QModelIndex aIndex = aPartModel->findGroup(aGroup);
          int aStart = aPartModel->rowCount(aIndex);
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          removeRow(aStart, aIndex);
        }
      }
    }
    // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    //const ModelAPI_ObjectUpdatedMessage* aUpdMsg = dynamic_cast<const ModelAPI_ObjectUpdatedMessage*>(theMessage);
    //ObjectPtr aFeature = aUpdMsg->feature();
    //DocumentPtr aDoc = aFeature->document();

    // TODO: Identify the necessary index by the modified feature
    QModelIndex aIndex;
    emit dataChanged(aIndex, aIndex);

    // Reset whole tree **************************
  } else {
    rebuildDataTree();
  }
}

void XGUI_DocumentDataModel::rebuildDataTree()
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();

  beginResetModel();
  clearModelIndexes();

  int aNbParts = aRootDoc->size(ModelAPI_ResultPart::group());
  if (myPartModels.size() != aNbParts) {  // resize internal models
    while (myPartModels.size() > aNbParts) {
      delete myPartModels.last();
      myPartModels.removeLast();
    }
    while (myPartModels.size() < aNbParts) {
      myPartModels.append(new XGUI_PartDataModel(this));
    }
    for (int i = 0; i < myPartModels.size(); i++)
      myPartModels.at(i)->setPartId(i);
  }
  endResetModel();
}

QVariant XGUI_DocumentDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant();
  switch (theIndex.internalId()) {
    case PartsFolder:
      switch (theRole) {
        case Qt::DisplayRole:
          return tr("Parts") + QString(" (%1)").arg(rowCount(theIndex));
        case Qt::DecorationRole:
          return QIcon(":pictures/constr_folder.png");
        case Qt::ToolTipRole:
          return tr("Parts folder");
        case Qt::ForegroundRole:
          if (myActivePart)
            return QBrush(PASSIVE_COLOR);
            else
            return QBrush(ACTIVE_COLOR);
            default:
            return QVariant();
          }
          break;
          case HistoryNode:
          {
            int aOffset = historyOffset();
            DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
            ObjectPtr aObj = aRootDoc->object(ModelAPI_Feature::group(), theIndex.row() - aOffset);
            FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
            if (!aFeature)
            return QVariant();
            switch (theRole) {
              case Qt::DisplayRole:
              if (aFeature)
              return aFeature->data()->name().c_str();
              else
              return QVariant();
              case Qt::DecorationRole:
              return QIcon(XGUI_Workshop::featureIcon(aFeature->getKind()));
              case Qt::ToolTipRole:
              return tr("Feature object");
              case Qt::ForegroundRole:
              if (myActivePart)
              return QBrush(PASSIVE_COLOR);
              else
              return QBrush(ACTIVE_COLOR);
              default:
              return QVariant();
            }
          }
          break;
        }
  QModelIndex aParent = theIndex.parent();
  if (aParent.isValid() && (aParent.internalId() == PartsFolder)) {
    return myPartModels.at(theIndex.row())->data(QModelIndex(), theRole);
  }
  return toSourceModelIndex(theIndex)->data(theRole);
}

QVariant XGUI_DocumentDataModel::headerData(int theSection, Qt::Orientation theOrient,
                                            int theRole) const
{
  return QVariant();
}

int XGUI_DocumentDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid()) {
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    // Size of external models
    int aVal = historyOffset();
    // Plus history size
    aVal += aRootDoc->size(ModelAPI_Feature::group());
    return aVal;
  }
  if (theParent.internalId() == PartsFolder) {
    int aSize = myPartModels.size();
    return myPartModels.size();
  }
  if (theParent.internalId() == HistoryNode) {
    return 0;
  }
  QModelIndex* aParent = toSourceModelIndex(theParent);
  const QAbstractItemModel* aModel = aParent->model();
  if (!isSubModel(aModel))
    return 0;

  /*if (isPartSubModel(aModel)) {
   if (aModel != myActivePart)
   return 0;
   }*/
  return aModel->rowCount(*aParent);
}

int XGUI_DocumentDataModel::columnCount(const QModelIndex& theParent) const
{
  return 1;
}

QModelIndex XGUI_DocumentDataModel::index(int theRow, int theColumn,
                                          const QModelIndex& theParent) const
{
  QModelIndex aIndex;
  if (!theParent.isValid()) {
    int aOffs = myModel->rowCount();
    if (theRow < aOffs) {
      aIndex = myModel->index(theRow, theColumn, theParent);
      aIndex = createIndex(theRow, theColumn, (void*) getModelIndex(aIndex));
    } else {
      if (theRow == aOffs)  // Create Parts node
        aIndex = partFolderNode();
      else
        // create history node
        aIndex = createIndex(theRow, theColumn, HistoryNode);
    }
  } else {
    if (theParent.internalId() == PartsFolder) {
      aIndex = myPartModels.at(theRow)->index(0, theColumn, QModelIndex());
    } else {
      QModelIndex* aParent = (QModelIndex*) theParent.internalPointer();
      aIndex = aParent->model()->index(theRow, theColumn, (*aParent));
    }
    aIndex = createIndex(theRow, theColumn, (void*) getModelIndex(aIndex));
  }
  return aIndex;
}

QModelIndex XGUI_DocumentDataModel::parent(const QModelIndex& theIndex) const
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return QModelIndex();

  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  const QAbstractItemModel* aModel = aIndex->model();
  if (!isSubModel(aModel))
    return QModelIndex();

  if (isPartSubModel(aModel)) {
    if (!aModel->parent(*aIndex).isValid()) {
      return partFolderNode();
    }
  }

  QModelIndex aIndex1 = aModel->parent(*aIndex);
  if (aIndex1.isValid())
    return createIndex(aIndex1.row(), aIndex1.column(), (void*) getModelIndex(aIndex1));
  return aIndex1;
}

bool XGUI_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;
  return rowCount(theParent) > 0;
}

QModelIndex* XGUI_DocumentDataModel::toSourceModelIndex(const QModelIndex& theProxy) const
{
  QModelIndex* aIndexPtr = static_cast<QModelIndex*>(theProxy.internalPointer());
  return aIndexPtr;
}

QModelIndex* XGUI_DocumentDataModel::findModelIndex(const QModelIndex& theIndex) const
{
  QList<QModelIndex*>::const_iterator aIt;
  for (aIt = myIndexes.constBegin(); aIt != myIndexes.constEnd(); ++aIt) {
    QModelIndex* aIndex = (*aIt);
    if ((*aIndex) == theIndex)
      return aIndex;
  }
  return 0;
}

QModelIndex* XGUI_DocumentDataModel::getModelIndex(const QModelIndex& theIndex) const
{
  QModelIndex* aIndexPtr = findModelIndex(theIndex);
  if (!aIndexPtr) {
    aIndexPtr = new QModelIndex(theIndex);
    XGUI_DocumentDataModel* that = (XGUI_DocumentDataModel*) this;
    that->myIndexes.append(aIndexPtr);
  }
  return aIndexPtr;
}

void XGUI_DocumentDataModel::clearModelIndexes()
{
  QList<QModelIndex*>::const_iterator aIt;
  for (aIt = myIndexes.constBegin(); aIt != myIndexes.constEnd(); ++aIt)
    delete (*aIt);
  myIndexes.clear();
}

ObjectPtr XGUI_DocumentDataModel::object(const QModelIndex& theIndex) const
{
  if (theIndex.internalId() == PartsFolder)
    return ObjectPtr();
  if (theIndex.internalId() == HistoryNode) {
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    int aOffset = historyOffset();
    return aRootDoc->object(ModelAPI_Feature::group(), theIndex.row() - aOffset);
  }
  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  if (!isSubModel(aIndex->model()))
    return ObjectPtr();

  const XGUI_FeaturesModel* aModel = dynamic_cast<const XGUI_FeaturesModel*>(aIndex->model());
  return aModel->object(*aIndex);
}

bool XGUI_DocumentDataModel::insertRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginInsertRows(theParent, theRow, theRow + theCount - 1);
  //endInsertRows();

  // Update history
  QModelIndex aRoot;
  int aRow = rowCount(aRoot);
  beginInsertRows(aRoot, aRow, aRow);
  endInsertRows();

  return true;
}

bool XGUI_DocumentDataModel::removeRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginRemoveRows(theParent, theRow, theRow + theCount - 1);
  endRemoveRows();
  return true;
}

void XGUI_DocumentDataModel::removeSubModel(int theModelId)
{
  XGUI_PartModel* aModel = myPartModels.at(theModelId);
  QIntList aToRemove;
  for (int i = 0; i < myIndexes.size(); i++) {
    if (myIndexes.at(i)->model() == aModel)
      aToRemove.append(i);
  }
  int aId;
  while (aToRemove.size() > 0) {
    aId = aToRemove.last();
    delete myIndexes.at(aId);
    myIndexes.removeAt(aId);
    aToRemove.removeLast();
  }
  delete aModel;
  myPartModels.removeAt(theModelId);
}

bool XGUI_DocumentDataModel::isSubModel(const QAbstractItemModel* theModel) const
{
  if (theModel == myModel)
    return true;
  return isPartSubModel(theModel);
}

bool XGUI_DocumentDataModel::isPartSubModel(const QAbstractItemModel* theModel) const
{
  return myPartModels.contains((XGUI_PartModel*) theModel);
}

QModelIndex XGUI_DocumentDataModel::partFolderNode() const
{
  int aPos = myModel->rowCount(QModelIndex());
  return createIndex(aPos, columnCount() - 1, PartsFolder);
}

int XGUI_DocumentDataModel::historyOffset() const
{
  // Nb of rows of top model + Parts folder
  return myModel->rowCount(QModelIndex()) + 1;
}

bool XGUI_DocumentDataModel::activatedIndex(const QModelIndex& theIndex)
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return false;

  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  if (!aIndex)
    return false;

  const QAbstractItemModel* aModel = aIndex->model();

  if (isPartSubModel(aModel)) {
    // if this is root node (Part item index)
    if (!aIndex->parent().isValid()) {
      if (myActivePart)
        myActivePart->setItemsColor(PASSIVE_COLOR);

        if (myActivePart == aModel) {
          myActivePart = 0;
          myActivePartIndex = QModelIndex();
        } else {
          myActivePart = (XGUI_PartModel*)aModel;
          myActivePartIndex = theIndex;
        }

        if (myActivePart) {
          myActivePart->setItemsColor(ACTIVE_COLOR);
          myModel->setItemsColor(PASSIVE_COLOR);
        } else
          myModel->setItemsColor(ACTIVE_COLOR);
        return true;
      }
    }
  return false;
}

ResultPartPtr XGUI_DocumentDataModel::activePart() const
{
  if (myActivePart)
    return myActivePart->part();
  return ResultPartPtr();
}

void XGUI_DocumentDataModel::deactivatePart()
{
  if (myActivePart)
    myActivePart->setItemsColor(PASSIVE_COLOR);
    myActivePart = 0;
    myActivePartIndex = QModelIndex();
    myModel->setItemsColor(ACTIVE_COLOR);
  }

Qt::ItemFlags XGUI_DocumentDataModel::flags(const QModelIndex& theIndex) const
{
  Qt::ItemFlags aFlags = QAbstractItemModel::flags(theIndex);
  if (object(theIndex)) {
    aFlags |= Qt::ItemIsEditable;
  }
  return aFlags;
}

QModelIndex XGUI_DocumentDataModel::partIndex(const ResultPartPtr& theObject) const
{
  int aRow = -1;
  XGUI_PartModel* aModel = 0;
  foreach (XGUI_PartModel* aPartModel, myPartModels)
  {
    aRow++;
    if (aPartModel->part() == theObject) {
      aModel = aPartModel;
      break;
    }
  }
  if (aModel) {
    return createIndex(aRow, 0, (void*) getModelIndex(aModel->index(0, 0, QModelIndex())));
  }
  return QModelIndex();
}

QModelIndex XGUI_DocumentDataModel::objectIndex(const ObjectPtr theObject) const
{
  // Check that this feature belongs to root document
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  DocumentPtr aDoc = theObject->document();
  if (aDoc == aRootDoc) {
    // This feature belongs to histrory or top model
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature) {
      int aId;
      int aNb = aRootDoc->size(ModelAPI_Feature::group());
      for (aId = 0; aId < aNb; aId++) {
        if (theObject == aRootDoc->object(ModelAPI_Feature::group(), aId))
          break;
      }
      if (aId < aNb)
        return index(aId + historyOffset(), 0, QModelIndex());
    } else {
      QModelIndex aIndex = myModel->objectIndex(theObject);
      return
          aIndex.isValid() ?
              createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex)) :
              QModelIndex();
    }
  } else {
    XGUI_PartModel* aPartModel = 0;
    foreach(XGUI_PartModel* aModel, myPartModels)
    {
      if (aModel->hasDocument(aDoc)) {
        aPartModel = aModel;
        break;
      }
    }
    if (aPartModel) {
      QModelIndex aIndex = aPartModel->objectIndex(theObject);
      return
          aIndex.isValid() ?
              createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex)) :
              QModelIndex();
    }
  }
  return QModelIndex();
}
