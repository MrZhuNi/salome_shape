#ifndef XGUI_DataTreeModel_H
#define XGUI_DataTreeModel_H

#include "XGUI.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultPart.h>

#include <QAbstractItemModel>
#include <QColor>

/**\class XGUI_FeaturesModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with features data.
 */
class XGUI_EXPORT XGUI_FeaturesModel : public QAbstractItemModel
{
 public:
  XGUI_FeaturesModel(QObject* theParent)
      : QAbstractItemModel(theParent),
        myItemsColor(Qt::black)
  {
  }

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  virtual ObjectPtr object(const QModelIndex& theIndex) const = 0;

  //! Returns QModelIndex which corresponds to the given feature
  //! If the feature is not found then index is not valid
  virtual QModelIndex objectIndex(const ObjectPtr& theFeature) const = 0;

  //! Returns parent index of the given feature
  virtual QModelIndex findParent(const ObjectPtr& theObject) const = 0;

  //! Returns index corresponded to the group
  virtual QModelIndex findGroup(const std::string& theGroup) const = 0;

  void setItemsColor(const QColor& theColor)
  {
    myItemsColor = theColor;
  }

  QColor itemsColor() const
  {
    return myItemsColor;
  }

 protected:
  QColor myItemsColor;
};

/**\class XGUI_PartModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with parts data.
 */
class XGUI_PartModel : public XGUI_FeaturesModel
{
 public:
  XGUI_PartModel(QObject* theParent)
      : XGUI_FeaturesModel(theParent)
  {
  }

  void setPartId(int theId)
  {
    myId = theId;
  }

  //! Returns true if the given document is a sub-document of this tree
  virtual bool hasDocument(const DocumentPtr& theDoc) const = 0;

  //! Return a Part object
  virtual ResultPartPtr part() const = 0;

 protected:
  //! Id of the current part object in the document
  int myId;
};

#endif
