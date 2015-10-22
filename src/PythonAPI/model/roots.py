"""Abstract root classes of user-defined Python features producing a Body
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI



class Feature(ModelAPI.ModelAPI_Feature):
    """Base class of user-defined Python features."""

    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)

    def addRealInput (self, inputid):
        self.data().addAttribute(inputid,
                                 ModelAPI.ModelAPI_AttributeDouble_typeId())

    def getRealInput (self, inputid):
        return self.data().real(inputid).value()

    def addResult (self, result):
        shape = result.shape()
        body = self.document().createBody(self.data())
        body.store(shape)
        self.setResult(body)


class Interface():
    """Base class of hight level Python interfaces to features."""

    def __init__(self, feature):
        self._feature = feature



    def setRealInput (self, inputid, value):
        self._feature.data().real(inputid).setValue(value)

    def areInputValid (self):
        validators = ModelAPI.ModelAPI_Session.get().validators()
        return validators.validate(self._feature)

    def execute (self):
        self._feature.execute()
