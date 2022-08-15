# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
#  SuggestConstraintsFeature class definition

import ModelAPI
#import ExchangeAPI
import EventsAPI

import salome

from salome.shaper import model

import os

## @ingroup Plugins
#  Feature to suggest constraints using a Machine Learning model
class SuggestConstraintsFeature(model.Feature):
#class SuggestConstraintsFeature(ModelAPI.ModelAPI_Feature):

    ## The constructor.
    def __init__(self):
        #ModelAPI.ModelAPI_Feature.__init__(self)
        model.Feature.__init__(self)
        pass

    @staticmethod
    ## Export kind. Static.
    def ID():
        return "SuggestConstraints"

    ## Returns the kind of a feature.
    def getKind(self):
        return SuggestConstraintsFeature.ID()

    ## True: This feature is action: has no property panel and executes immediately.
    ## False: Otherwise.
    def isAction(self):
        return False
        #TODO
        #return True

    def isMacro(self):
        """
        Override Feature.isMacro().
        Rectangle feature is macro: removes itself on the creation transaction finish.
        """
        return True

    ## Init attributes
    def initAttributes(self):
        #TODO
        pass

    ## Export the results, groups and fields via XAO
    def convertToSAM(self):
        # if part-set is active, iterate also parts
        #for isPart in (True, False):
        print("SuggestConstraintsFeature: convertToSAM")
        #TODO
        pass

    ## Exports all shapes and groups into the GEOM module.
    def execute(self):
        #aSession = ModelAPI.ModelAPI_Session.get()
        ## Get active document
        #self.Part = aSession.activeDocument()

        print("SuggestConstraintsFeature: execute")
        self.convertToSAM()
        # TODO: Lot3 and Lot4

        pass
