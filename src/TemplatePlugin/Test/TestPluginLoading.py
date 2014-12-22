#=========================================================================
# Initialization of the test
#=========================================================================
import os
import re
import time

from ModelAPI import *
__updated__ = "2014-12-19"

#=========================================================================
# execfile("D:\NewGEOM\sources\src\TemplatePlugin\Test\TestPluginLoading.py")
# Before test: trying to figure out how much plugins are generated
# and how much features they contain
#=========================================================================
# self_dir = os.path.dirname(os.path.realpath(__file__))
# all_plugin_files = os.listdir(os.path.join(self_dir, ".."))
self_dir = os.path.abspath("D:\\NewGEOM\\sources\\src\\TemplatePlugin")
all_plugin_files = os.listdir(self_dir)
reg_exp = re.compile(r"plugin-Template_\d+\.xml")
all_plugin_files = [x for x in all_plugin_files if reg_exp.match(x)]
plugins_count = len(all_plugin_files)
assert (plugins_count > 0)
features_count = 0
with open(os.path.join(self_dir, "plugin-Template_1.xml")) as a_plugin_file:
    reg_exp = re.compile(r"id=\"TemplateFeature_\d+_P_\d+\"")
    features_count = len(reg_exp.findall(a_plugin_file.read()))
assert (features_count > 0)
#=========================================================================
# Creation all features to load the plugin
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
aSession.startOperation()
print "Starting features creation..."
t1 = time.time()
for nplugin in xrange(1, plugins_count + 1):
    aFeatureId = "TemplateFeature_1_P_{N}".format(N=nplugin)
    aSketchCommonFeature = aDocument.addFeature(aFeatureId)

t2 = time.time()
print "Creation fnished, took: ", float(t2 - t1), "s"
pass
aSession.finishOperation()
#=========================================================================
# End of test
#=========================================================================
