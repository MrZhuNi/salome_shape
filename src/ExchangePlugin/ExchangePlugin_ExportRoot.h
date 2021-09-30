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

#ifndef EXCHANGEPLUGIN_EXPORTROOT_H_
#define EXCHANGEPLUGIN_EXPORTROOT_H_

namespace ExchangePlugin_ExportRoot
{
  /// Returns the coordinates of the center and the dimensions of a box
  /// \param[in] theFeature a feature to obtain AttributeRefAttr
  /// \param[out] theCenter coordinates of the center of the box
  /// \param[out] theDimensions dimensions of the box
  EXCHANGEPLUGIN_EXPORT std::vector<double> computeBox(FeaturePtr theFeature);
}

#endif /* EXCHANGEPLUGIN_EXPORTROOT_H_ */
