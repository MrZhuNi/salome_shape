# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

"""
    Test1779.py
    Test case for issue #1779 "Wrong ExtrusionCut result after loading dump"
"""

from salome.shaper import model
from SketchAPI import *
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "width_hole", "24.5")
ParamH = model.addParameter(Part_1_doc, "height", "6.5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-22.07107063584519, -12.07106498788576, -38.67766953846773, 4.535533914736781)
SketchLine_1.setName("SketchLine_13")
SketchLine_1.result().setName("SketchLine_13")
SketchLine_2 = Sketch_1.addLine(-46.85786524463795, 6.000000076973941, -42.21320335935584, 6.000000076973941)
SketchLine_2.setName("SketchLine_14")
SketchLine_2.result().setName("SketchLine_14")
SketchLine_3 = Sketch_1.addLine(-5, -5, -5, 26)
SketchLine_3.setName("SketchLine_2")
SketchLine_3.result().setName("SketchLine_2")
SketchArc_1 = Sketch_1.addArc(-15, -5, -5, -5, -22.07107063584519, -12.07106498788576, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_3")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchArc_2 = Sketch_1.addArc(-49.99999999999999, 6.483314850521824, -67.33333268454413, -3.4944393079507, -62.30717627979378, 22.24825735019753, True)
SketchLine_4 = Sketch_1.addLine(-62.30717627979378, 22.24825735019753, -37.30717688933122, 41.76494210080364)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_8")
SketchArc_3 = Sketch_1.addArc(-25, 26.00000007697395, -37.30717688933122, 41.76494210080364, -5, 26, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_9")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_3.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_10")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_3.results()[1], 20)
SketchConstraintRadius_1.setName("SketchConstraintRadius_2")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 31)
SketchCircle_1 = Sketch_1.addCircle(-25, 26.00000007697395, 10)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_3.center(), SketchCircle_1.center())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_11")
SketchLine_5 = Sketch_1.addLine(-25, 26.00000007697395, -9.394737677428997, 26.00000007697395)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_12")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_13")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_1.setName("SketchConstraintHorizontal_2")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_1.results()[1], 10)
SketchConstraintRadius_2.setName("SketchConstraintRadius_3")
SketchCircle_2 = Sketch_1.addCircle(-18, 1.000000076973949, 6.5)
SketchLine_6 = Sketch_1.addLine(-15, -5, -5, -5)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_14")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_6.endPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_15")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_16")
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_2.setName("SketchConstraintHorizontal_3")
SketchLine_7 = Sketch_1.addLine(-25, 26.00000007697395, -25, 49.221836288079)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_7.startPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_17")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_7.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_18")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_19")
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_7.result(), 25)
SketchCircle_3 = Sketch_1.addCircle(-43.99999999999999, 21.00000007697395, 6.5)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_3.center(), SketchLine_7.result(), 19)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_2.center(), SketchLine_5.result(), 25)
SketchConstraintDistance_3.setName("SketchConstraintDistance_4")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchCircle_2.center(), SketchLine_7.result(), 7)
SketchConstraintDistance_4.setName("SketchConstraintDistance_5")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_3.result())
SketchConstraintTangent_1.setName("SketchConstraintTangent_2")
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_3.results()[1])
SketchConstraintTangent_2.setName("SketchConstraintTangent_3")
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_4.result())
SketchConstraintTangent_3.setName("SketchConstraintTangent_4")
SketchLine_8 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_8.result(), 5)
SketchConstraintDistance_5.setName("SketchConstraintDistance_8")
SketchLine_9 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_9.result(), 5)
SketchConstraintDistance_6.setName("SketchConstraintDistance_9")
SketchPoint_1 = Sketch_1.addPoint(-59.46448692526128, -4.535554660152811)
SketchArc_4 = Sketch_1.addArc(-63, -0.9999999230260515, -67.33333268454413, -3.4944393079507, -59.46448692526128, -4.535554660152811, False)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_21")
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_4.results()[1], 5)
SketchConstraintRadius_3.setName("SketchConstraintRadius_4")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchPoint_1.coordinates())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_22")
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchCircle_2.results()[1], 6.5)
SketchConstraintRadius_4.setName("SketchConstraintRadius_5")
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchConstraintRadius_5.setName("SketchConstraintRadius_6")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_3.results()[1], SketchCircle_2.results()[1])
SketchArc_5 = Sketch_1.addArc(-42.21320351257037, 1.000000076973942, -38.67766953846773, 4.535533914736781, -42.21320335935584, 6.000000076973941, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_27")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_2.endPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_28")
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_1.result())
SketchConstraintTangent_4.setName("SketchConstraintTangent_7")
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_2.result())
SketchConstraintTangent_5.setName("SketchConstraintTangent_8")
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchArc_5.results()[1], 5)
SketchConstraintRadius_6.setName("SketchConstraintRadius_7")
SketchPoint_2 = Sketch_1.addPoint(-46.85786524463795, 6.000000076973941)
SketchPoint_2.setName("SketchPoint_3")
SketchPoint_2.result().setName("SketchPoint_3")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.startPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_38")
SketchLine_10 = Sketch_1.addLine(-46.85786524463795, 6.000000076973941, -46.85786524463795, -6.278575593100925)
SketchLine_10.setName("SketchLine_22")
SketchLine_10.result().setName("SketchLine_22")
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_40")
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_10.startPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_41")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintVertical_3.setName("SketchConstraintVertical_5")
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_3.setName("SketchConstraintHorizontal_8")
SketchArc_6 = Sketch_1.addArc(SketchLine_2.startPoint(), -50.3933775416084, 4.535554723500073, False)
model.do()
SketchArc_6 = SketchAPI_Arc(model.lastSubFeature(featureToCompositeFeature(Sketch_1.feature()), "SketchArc"))
SketchLine_11 = Sketch_1.addLine(-50.3933775416084, 4.535554723500073, -59.46448692526128, -4.535554660152811)
SketchLine_11.setName("SketchLine_21")
SketchLine_11.result().setName("SketchLine_21")
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_11.startPoint(), SketchArc_6.endPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_30")
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchArc_6.results()[1], SketchLine_11.result())
SketchConstraintTangent_7.setName("SketchConstraintTangent_10")
SketchConstraintRadius_7 = Sketch_1.setRadius(SketchArc_6.results()[1], 5)
SketchConstraintRadius_7.setName("SketchConstraintRadius_8")
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchArc_3.center(), SketchLine_2.result(), 20)
SketchConstraintDistance_7.setName("SketchConstraintDistance_10")
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchArc_4.center(), SketchLine_5.result(), 27)
SketchConstraintDistance_8.setName("SketchConstraintDistance_12")
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_11.endPoint())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_31")
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchArc_4.center(), SketchLine_7.result(), 38)
SketchConstraintDistance_9.setName("SketchConstraintDistance_13")
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchCircle_3.center(), SketchLine_5.result(), 5)
SketchConstraintDistance_10.setName("SketchConstraintDistance_14")
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_11.result())
SketchConstraintTangent_8.setName("SketchConstraintTangent_11")
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_4.results()[1])
SketchConstraintTangent_9.setName("SketchConstraintTangent_12")
SketchConstraintRadius_8 = Sketch_1.setRadius(SketchArc_2.results()[1], 20)
SketchConstraintRadius_8.setName("SketchConstraintRadius_9")
SketchCircle_4 = Sketch_1.addCircle(-58.99999999999999, 10.00000007697395, 6.5)
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchCircle_4.center(), SketchLine_7.result(), 34)
SketchConstraintDistance_11.setName("SketchConstraintDistance_15")
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchCircle_4.center(), SketchLine_5.result(), 16)
SketchConstraintDistance_12.setName("SketchConstraintDistance_16")
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchCircle_2.results()[1], SketchCircle_4.results()[1])
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_32")
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_1.result(), SketchLine_2.result(), 135)
SketchConstraintTangent_10 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
SketchConstraintTangent_10.setName("SketchConstraintTangent_13")
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_11.result(), SketchLine_10.result(), 45)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2f-SketchLine_2f-SketchArc_3_2f-SketchLine_5r-SketchArc_2_2f-SketchArc_4_2f-SketchLine_21r-SketchArc_6_2r-SketchLine_14f-SketchArc_5_2r-SketchLine_13r-SketchCircle_2_2r-SketchCircle_1_2r-SketchCircle_4_2r-SketchCircle_3_2r")], model.selection(), "height", 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"))
SketchLine_12 = Sketch_2.addLine(38.24999999999999, 6.5, 13.75, 6.5)
SketchLine_13 = Sketch_2.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face_1]"))
SketchLine_13.setName("SketchLine_15")
SketchLine_13.result().setName("SketchLine_15")
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.result())
SketchLine_14 = Sketch_2.addLine(13.75, 6.5, 13.75, 4.5)
SketchLine_14.setName("SketchLine_16")
SketchLine_14.result().setName("SketchLine_16")
SketchLine_15 = Sketch_2.addLine(13.75, 4.5, 38.24999999999999, 4.5)
SketchLine_15.setName("SketchLine_17")
SketchLine_15.result().setName("SketchLine_17")
SketchLine_16 = Sketch_2.addLine(38.24999999999999, 4.5, 38.24999999999999, 6.5)
SketchLine_16.setName("SketchLine_18")
SketchLine_16.result().setName("SketchLine_18")
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_33")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_34")
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_35")
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_12.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_16.result())
SketchConstraintVertical_5.setName("SketchConstraintVertical_6")
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_14.result(), 2)
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_12.result(), "width_hole")
SketchConstraintDistance_13 = Sketch_2.setDistance(SketchLine_13.endPoint(), SketchLine_16.result(), "width_hole/2")
SketchConstraintDistance_13.setName("SketchConstraintDistance_17")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_12r-SketchLine_16f-SketchLine_17f-SketchLine_18f")], model.selection(), 0, 100, [model.selection("SOLID", "Extrusion_1_1")])
model.do()

# check ExtrusionCut_1 is valid
assert(ExtrusionCut_1.feature().error() == "")

# change parameter and check validity again
ParamH.setValue(10)
model.do()
assert(ExtrusionCut_1.feature().error() == "")

# revert parameter
ParamH.setValue(6.5)
model.do()
assert(ExtrusionCut_1.feature().error() == "")

model.end()
