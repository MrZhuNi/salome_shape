"""Registration of all user-defined Python features
"""

import ModelAPI
from macros.box.feature       import BoxFeature
from macros.rectangle.feature import SketchPlugin_Rectangle


class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):
    """Implementation of features plugin.

    PythonFeaturesPlugin() -> plugin object
    """

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        ModelAPI.ModelAPI_Plugin.__init__(self)
        aSession = ModelAPI.ModelAPI_Session.get()
        aSession.registerPlugin(self)
        pass

    def createFeature(self, theFeatureID):
        """Override ModelAPI_Plugin.createFeature()"""
        aFeature = None

        if theFeatureID == BoxFeature.ID():
            aFeature = BoxFeature().__disown__()

        elif theFeatureID == SketchPlugin_Rectangle.ID():
            aFeature = SketchPlugin_Rectangle().__disown__()

        else:
            raise StandardError("No such feature %s" % theFeatureID)

        return aFeature

# The plugin object
plugin = PythonFeaturesPlugin()
plugin.__disown__()
