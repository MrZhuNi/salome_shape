// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <XGUI_ColorDialog.h>

#include <ModelAPI_Tools.h>

#include <QtxColorButton.h>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_ColorDialog::XGUI_ColorDialog(QWidget* theParent)
  : QDialog(theParent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
  setWindowTitle(tr("Color"));
  QGridLayout* aLay = new QGridLayout(this);

  QRadioButton* aRandomChoiceBtn = new QRadioButton(this);
  QRadioButton* aColorChoiceBtn = new QRadioButton(this);
  aColorChoiceBtn->setChecked(true);
  myButtonGroup = new QButtonGroup(this);
  myButtonGroup->setExclusive(true);
  myButtonGroup->addButton(aColorChoiceBtn, 0);
  myButtonGroup->addButton(aRandomChoiceBtn, 1);

  aLay->addWidget(aColorChoiceBtn, 0, 0);
  aLay->addWidget(aRandomChoiceBtn, 1, 0);

  myColorButton = new QtxColorButton(this);
  myColorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  aLay->addWidget(myColorButton, 0, 1);

  QLabel* aRandomLabel = new QLabel(tr("Random"), this);
  aLay->addWidget(aRandomLabel, 1, 1);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 2, 0, 1, 2);
}

bool XGUI_ColorDialog::isRandomColor() const
{
  return myButtonGroup->checkedId() == 1;
}

void XGUI_ColorDialog::setColor(const std::vector<int>& theValue)
{
  if (theValue.size() != 3)
    return;

  myColorButton->setColor(QColor(theValue[0], theValue[1], theValue[2]));
}

std::vector<int> XGUI_ColorDialog::getColor() const
{
  QColor aColorResult = myColorButton->color();

  std::vector<int> aValues;
  aValues.push_back(aColorResult.red());
  aValues.push_back(aColorResult.green());
  aValues.push_back(aColorResult.blue());

  return aValues;
}

std::vector<int> XGUI_ColorDialog::getRandomColor() const
{
  std::vector<int> aValues;
  if (isRandomColor()) {
    ModelAPI_Tools::findRandomColor(aValues);
  }
  return aValues;
}

