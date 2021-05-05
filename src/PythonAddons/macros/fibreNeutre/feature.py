# -*- coding: utf-8 -*-
# Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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

"""Obtention des surfaces médianes à partir d'une CAO contenue dans un fichier XAO

On sait traiter les faces :
  . planes
  . cylindriques
  . sphériques
  . toriques
  . coniques

Author: Gérald NICOLAS
"""
__revision__ = "V01.03"

import os
import tempfile
import numpy as np

import salome
from salome.shaper import model
from salome.geom import geomBuilder
from salome.geom import geomtools
from salome.kernel.studyedit import getStudyEditor
import SALOMEDS

import ModelAPI

from macros.fibreNeutre.surfaceMediane import SurfaceMediane

class fibreNeutre(model.Feature):
    """Création des fibres neutres"""

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "fibreNeutre"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file XAO."""
        return "file_path"

    def getKind(self):
        """Override Feature.getKind()"""
        return fibreNeutre.ID()


# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
    #     Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())


# Execution

    def execute(self):
        """F.execute() -- execute the Feature"""
        apath    = self.string(self.FILE_ID())
        filepath = apath.value()
        #print("filepath : '{}'".format(filepath))
        if filepath != "" :
            if os.path.isfile(filepath):
                geompy = geomBuilder.New()
                (_, objet, _, _, _) = geompy.ImportXAO(filepath)
                #print ("Lecture de {}".format(objet.GetName()))
                geompy.addToStudy( objet, objet.GetName() )

                l_options = list()
                #l_options.append("-v")
                #l_options.append("-vmax")
                s_med = SurfaceMediane(l_options)
                erreur, message = s_med.surf_objet_geom (objet)
                del s_med
                if erreur:
                    self.setError(message)
            else:
                self.setError("The file '{}' does not exist".format(filepath))

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        fibreNeutre feature is macro: removes itself on the creation transaction
        finish.
        """
        return True
