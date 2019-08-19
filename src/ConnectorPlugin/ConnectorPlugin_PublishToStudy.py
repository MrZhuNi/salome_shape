# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

## @package Plugins
#  PublishToStudy class definition

import ModelAPI
import ExchangeAPI
import EventsAPI

import salome
from salome.geom import geomBuilder

from salome.shaper import model


## @ingroup Plugins
#  Feature to export all shapes and groups into the GEOM module
class PublishToStudyFeature(ModelAPI.ModelAPI_Feature):

    ## The constructor.
    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)
        pass

    ## Export kind. Static.
    @staticmethod
    def ID():
        return "PublishToStudy"

    @staticmethod
    def TREE_ID():
        """Returns ID tree control."""
        return "DataTree"

    ## Returns the kind of a feature.
    def getKind(self):
        return PublishToStudyFeature.ID()

    ## This feature has no attributes, as it is action.
    def initAttributes(self):
        self.data().addAttribute(self.TREE_ID(), ModelAPI.ModelAPI_AttributeRefList_typeId())

    ## Exports all shapes and groups into the GEOM module.
    def execute(self):
        pass
