# -*- coding: utf-8 -*-
# Copyright (C) 2016-2020  CEA/DEN, EDF R&D
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

"""pipeNetwork Feature
Author: Nathalie Gore
Remarque : la fonction de partitionnement pour un futur maillage en hexa est désactivée.
"""

__revision__ = "V02.06"

from salome.shaper import model
import ModelAPI
from GeomAPI import *

# Si erreur :

def raiseException(texte):
    """En cas d'erreur"""
    print (texte)

class pipeNetwork(model.Feature):
    """Creation of a network of pipes"""
    lfeatures = list()
    folder = None
    isHexa = False
    twopartwo = "2par2"
    parligne = "par_ligne"
    radius = 0.5
    infoPoints = dict()
    connectivities = dict()

    _verbose = False

# Feature initializations

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        model.Feature.__init__(self)

    @staticmethod
    def ID():
        """Return Id of the Feature."""
        return "pipeNetwork"

    @staticmethod
    def FILE_ID():
        """Returns ID of the file select parameter."""
        return "file_path"

    #@staticmethod
    #def HEXAS_ID():
        #"""Returns ID of the radius parameter."""
        #return "blocking"

    def getKind(self):
        """Override Feature.getKind()"""
        return pipeNetwork.ID()


# Initialization of the dialog panel

    def initAttributes(self):
        """Override Feature.initAttributes()"""
        # Creating the input argument of the feature
        self.data().addAttribute(self.FILE_ID(), ModelAPI.ModelAPI_AttributeString_typeId())
        #self.data().addAttribute(self.HEXAS_ID(), ModelAPI.ModelAPI_AttributeBoolean_typeId())


# Retrieve parent pipe

    def decodingCode(self, code):
        """decodingCode"""
        splitCode = code.split(".")
        if len(splitCode) <= 1:
            previousCode = ""
        else:
            previousCode = code[:len(code)-len(splitCode[-1])-1]
        return previousCode

    def readNodeInfo(self, line):
        """Lecture des noeuds

La ligne est formée des informations :
. l'identifiant du noeud
. si les coordonnées sont données en absolu : "-" suivi des 3 coordonnées
. si les coordonnées sont données en relatif : l'identifiant du noeud de départ, suivi des 3 coordonnées de la translation
        """
        #print(line)
        texte = line
        splitLine = line.split(" ")
        if ( len(splitLine) != 5 ):
            diagno = 1
        elif splitLine[0] in self.infoPoints:
            texte += "\nThis node was already declared."
            diagno = 2
        elif ( splitLine[1] not in self.infoPoints ) and ( splitLine[1] != "-" ):
            texte += "\nThe starting point was not seen before."
            diagno = 3
        else:
            diagno = 0
            self.infoPoints[splitLine[0]] = dict()
            self.infoPoints[splitLine[0]]["Ref"] = splitLine[1]
            if splitLine[1] == "-":
                self.infoPoints[splitLine[0]]["X"] = float(splitLine[2])
                self.infoPoints[splitLine[0]]["Y"] = float(splitLine[3])
                self.infoPoints[splitLine[0]]["Z"] = float(splitLine[4])
            else :
                self.infoPoints[splitLine[0]]["X"] = self.infoPoints[splitLine[1]]["X"] + float(splitLine[2])
                self.infoPoints[splitLine[0]]["Y"] = self.infoPoints[splitLine[1]]["Y"] + float(splitLine[3])
                self.infoPoints[splitLine[0]]["Z"] = self.infoPoints[splitLine[1]]["Z"] + float(splitLine[4])
            self.infoPoints[splitLine[0]]["isEnd"] = False
        #print ("Retour de readNodeInfo = {}".format(diagno))
        return diagno, texte

    def readConnectivity(self, line, method):
        """Lecture des connectivités"""
        splitLine = line.split(" ")
        print(line)
        diagno = 0
        if method == self.twopartwo :
            if self.connectivities == {} :
                print("nouvelle ligne - Cas 1")
                self.newConnectivity(splitLine[0], splitLine)
            else :
                # Recherche si ligne déjà existante ou si nouvelle ligne
                print("Lignes existantes")
                for key, val in self.connectivities.items():
                    print(key, " ******* ",val)
                    if val['chainage'][-1] == splitLine[0]:
                        # La ligne existe
                        val['chainage'].append(splitLine[1])
                        return diagno
                # La ligne n'existe pas
                print("nouvelle ligne - Cas 2")
                self.newConnectivity(splitLine[0], splitLine)
        else :
            self.newConnectivity(splitLine[0], splitLine)
        #print ("Retour de readConnectivity = {}".format(diagno))
        return diagno

    def newConnectivity(self, key, value):
        """newConnectivity"""
        self.connectivities[key] = dict()
        self.connectivities[key]['chainage'] = value

    def readFillet(self, line):
        """Décodage des caractéristiques de la connection entre deux tuyaux

La ligne est formée de deux informations :
. l'identifiant du noeud
. la caractérisation de la connection : "angular_connection" ou "radius=xxx"
        """
        splitLine = line.split(" ")
        if len(splitLine) != 2:
            print(line)
            diagno = 1
        elif not splitLine[0] in self.infoPoints:
            print(line)
            diagno = 2
        elif splitLine[1] == "angular_connection":
            self.infoPoints[splitLine[0]]["Fillet"] = "angular_connection"
            diagno = 0
        elif splitLine[1][:7] == "radius=":
            self.infoPoints[splitLine[0]]["Fillet"] = "radius"
            self.infoPoints[splitLine[0]]["Radius"] = float(splitLine[1][7:])
            diagno = 0
        else:
            print(line)
            diagno = 3
        #print ("Retour de readFillet = {}".format(diagno))
        return diagno

    def retrieveSubshapesforWire(self, copy, key, ind):
        """retrieveSubshapesforWire"""
        exp = GeomAPI_ShapeExplorer(copy.defaultResult().shape(), GeomAPI_Shape.EDGE)

        end = False
        subshapesForWire = list()
        currentInd = 0
        isPipe = True
        print("Current chainage : ", self.connectivities[key]['chainage'][ind:])
        print("Indice de démarrage = ", ind)

        while exp.more() and not end :
            print("Analyse Edge n°", currentInd)
            print(" => ", self.connectivities[key]['chainage'][currentInd], " - ", self.connectivities[key]['chainage'][currentInd+1])
            print(" ==> ", self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"], " - ", self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isAngular"])
            cur = exp.current().edge()
            if currentInd < ind:
                print("Edge non prise en compte")
                print("test si fillet : ", currentInd+1, ind, self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"])
                if currentInd+1 <= ind and self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"] == "radius" and not self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"]:
                    print("Fillet à ne pas prendre en compte")
                    exp.next()
                    cur = exp.current().edge()
            else :
                subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                print("Mode normal - Nb segments dans le wire : ", len(subshapesForWire))
                # Cas du fillet : on récupère l'edge suivante
                if self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["isAngular"] or self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isAngular"]:
                    end = True
                    print("Nb segments dans le wire : ", len(subshapesForWire))
                    if len(subshapesForWire) == 1:
                        print("Coude droit en cours")
                        currentInd += 1
                        isPipe = False
                    else :
                        print("Coude droit à venir")
                        subshapesForWire = subshapesForWire[:-1]
                elif self.infoPoints[self.connectivities[key]['chainage'][currentInd]]["Fillet"] == "radius":
                    print("Ajout edge start Fillet")
                    exp.next()
                    cur = exp.current().edge()
                    subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                    #currentInd = currentInd+1
                    print("Mode Fillet - Nb segments dans le wire : ", len(subshapesForWire))
                elif self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["Fillet"] == "radius":
                    print("Ajout edge end Fillet")
                    exp.next()
                    cur = exp.current().edge()
                    subshapesForWire.append(model.selection(copy.defaultResult(), cur))
                    print("Mode Fillet - Nb segments dans le wire : ", len(subshapesForWire))
                else :
                    if self.infoPoints[self.connectivities[key]['chainage'][currentInd+1]]["isEnd"]:
                        print("Fin detectee")
                        currentInd = currentInd+1
                        end = True
                    else :
                        print("Branchement")
            if not end:
                currentInd = currentInd+1
            exp.next()
            print("End = ", end, self.connectivities[key]['chainage'][currentInd])

        return subshapesForWire, currentInd, isPipe, self.connectivities[key]['chainage'][currentInd]

    def retrieveLastElement(self, obj, typeOfElement):
        """retrieveLastElement"""
        exp = GeomAPI_ShapeExplorer(obj.defaultResult().shape(), typeOfElement)
        while exp.more():
            cur = None
            if typeOfElement == GeomAPI_Shape.VERTEX :
                cur = exp.current().vertex()
            elif typeOfElement == GeomAPI_Shape.EDGE :
                cur = exp.current().edge()
            elif typeOfElement == GeomAPI_Shape.FACE :
                cur = exp.current().face()
            elif typeOfElement == GeomAPI_Shape.SOLID :
                cur = exp.current().solid()
            if cur is not None:
                exp.next()
                cur = model.selection(obj.defaultResult(), cur)
        return cur

    def retrieveFirstElement(self, obj, typeOfElement):
        """retrieveFirstElement"""
        exp = GeomAPI_ShapeExplorer(obj.defaultResult().shape(), typeOfElement)
        cur = None
        if typeOfElement == GeomAPI_Shape.VERTEX :
            cur = exp.current().vertex()
        elif typeOfElement == GeomAPI_Shape.EDGE :
            cur = exp.current().edge()
        elif typeOfElement == GeomAPI_Shape.FACE :
            cur = exp.current().face()
        elif typeOfElement == GeomAPI_Shape.SOLID :
            cur = exp.current().solid()
        if cur is not None:
            exp.next()
            cur = model.selection(obj.defaultResult(), cur)
        return cur

    def createPipe(self, part, connectivityInfos):
        """createPipe"""
        lPipes = list()
        startFace = None
        fuse = None
        for ind in range(len(connectivityInfos['paths'])):
            print("Step = ", ind)
            if ind == 0:
                startFace = connectivityInfos['sketch']
            if connectivityInfos['isPipe'][ind] :
                pipe = model.addPipe(part, [startFace], connectivityInfos['paths'][ind].result())
            else :
                # recherche du plan
                if self.infoPoints[connectivityInfos['ends'][ind]]['isAngular']:
                    pipe = model.addExtrusion(part, [startFace], model.selection(), self.infoPoints[connectivityInfos['ends'][ind]]['plane'], 0, model.selection(), 0, "Faces|Wires")
                else :
                    # le plan cible n'existe pas
                    edge = model.addAxis(part, self.infoPoints[connectivityInfos['starts'][ind]]['point'], self.infoPoints[connectivityInfos['ends'][ind]]['point'])
                    edge.execute(True)
                    self.lfeatures.append(edge)# self.retrieveFirstElement(connectivityInfos['paths'][ind], GeomAPI_Shape.EDGE)
                    point = self.retrieveLastElement(connectivityInfos['paths'][ind], GeomAPI_Shape.VERTEX)
                    plane = model.addPlane(part, edge.result(), point, True)
                    plane.execute(True)
                    self.lfeatures.append(plane)
                    pipe = model.addExtrusion(part, [startFace], edge.result(), plane.result(), 0, model.selection(), 0, "Faces|Wires")
            pipe.execute(True)
            self.lfeatures.append(pipe)
            lPipes.append(pipe.result())
            if ind < len(connectivityInfos['paths'])-1:
                copy = model.addCopy(part, [model.selection(pipe.defaultResult())], 1)
                copy.execute(True)
                self.lfeatures.append(copy)
                startFace = self.retrieveLastElement(copy, GeomAPI_Shape.FACE)

        if len(lPipes) > 1 :
            fuse = model.addFuse(part, lPipes, False)
            fuse.execute(True)
            self.lfeatures.append(fuse)
        else :
            return pipe
        return fuse

#==========================================================
# Création des différents éléments
    def createPoints(self, part):
        """Création des points"""
        print("========================= Creation des noeuds =========================")
        for key, value in self.infoPoints.items():
            if self._verbose:
                print("key = ", key)
            point = model.addPoint(part, value['X'], value['Y'], value['Z'])
            point.execute(True)
            self.lfeatures.append(point)
            value["point"] = point.result()

    def createPolylines(self, part):
        """Création des polylines"""
        print("========================= Creation des polylines =========================")
        for key, value in self.connectivities.items():
            if self._verbose:
                print("key = ", key)
            lPoints = list()
            for id_noeud in value['chainage']:
                lPoints.append(self.infoPoints[id_noeud]["point"])
            polyline = model.addPolyline3D(part, lPoints, False)
            polyline.execute(True)
            self.lfeatures.append(polyline)
            value["polyline"] = polyline

    def createFillets(self, part):
        """Création des fillets"""
        print("========================= Creation des fillets =========================")
        for key, value in self.connectivities.items():
            if self._verbose:
                print("key = ", key)
            # recherche des noeuds fillets
            value["fillet"] = value["polyline"]
            for id_noeud in value['chainage']:
                if self.infoPoints[id_noeud]["Fillet"] == "radius" :
                    print(self.infoPoints[id_noeud])
                    fillet1D = model.addFillet(part, [model.selection("VERTEX", (self.infoPoints[id_noeud]["X"],self.infoPoints[id_noeud]["Y"],self.infoPoints[id_noeud]["Z"]))], self.infoPoints[id_noeud]["Radius"])
                    fillet1D.execute(True)
                    self.lfeatures.append(fillet1D)
                    value["fillet"] = fillet1D

    def searchRightConnections(self, part):
        """Recherche des coudes droits"""
        print("========================= Recherche des coudes droits =========================")
        for key, value in self.connectivities.items():
            if self._verbose:
                print("key = ", key, value['chainage'])
            # recherche des noeuds fillets
            for ind, id_noeud in enumerate(value['chainage']):
                #print("Info sur : " id_noeud, " => ", self.infoPoints[id_noeud]["Fillet"])
                if ind == 0 or ind == len(value['chainage'])-1 :
                    self.infoPoints[id_noeud]["isAngular"] = False
                else :
                    if self.infoPoints[id_noeud]["Fillet"] == "radius" :
                        self.infoPoints[id_noeud]["isAngular"] = False
                    else :
                        if id_noeud in self.connectivities:
                            self.infoPoints[id_noeud]["isAngular"] = False
                        else :
                            self.infoPoints[id_noeud]["isAngular"] = True
                            print("========================= Création du plan =========================")
                            # Axe d'extrusion
                            print(ind-1, ind, ind+1)
                            print(value["chainage"][ind-1], id_noeud, value["chainage"][ind+1])
                            print(self.infoPoints[value["chainage"][ind-1]]["point"])

                            tmpPlane = model.addPlane(part, self.infoPoints[value["chainage"][ind-1]]["point"], self.infoPoints[id_noeud]["point"], self.infoPoints[value["chainage"][ind+1]]["point"])
                            tmpPlane.execute(True)
                            self.lfeatures.append(tmpPlane)
                            axis =  model.addAxis(part, tmpPlane.result(), self.infoPoints[id_noeud]["point"])
                            axis.execute(True)
                            self.lfeatures.append(axis)
                            self.infoPoints[id_noeud]["axis"] = axis.result()

                            # Edge a extruder
                            tmpEdge = model.addEdge(part, self.infoPoints[id_noeud]["point"], self.infoPoints[value["chainage"][ind+1]]["point"])
                            tmpEdge.execute(True)
                            self.lfeatures.append(tmpEdge)
                            length = model.measureDistance(part, self.infoPoints[value["chainage"][ind-1]]["point"], self.infoPoints[id_noeud]["point"])
                            point =  model.addPoint(part, tmpEdge.result(), length, False, False)
                            point.execute(True)
                            self.lfeatures.append(point)
                            baseEdge = model.addEdge(part, self.infoPoints[value["chainage"][ind-1]]["point"], point.result())
                            baseEdge.execute(True)
                            self.lfeatures.append(baseEdge)
                            middlePoint = model.addPoint(part, baseEdge.result(), 0.5, True, False)
                            middlePoint.execute(True)
                            self.lfeatures.append(middlePoint)
                            Edge = model.addEdge(part, self.infoPoints[id_noeud]["point"], middlePoint.result())
                            Edge.execute(True)
                            self.lfeatures.append(Edge)

                            # Extrusion
                            plane = model.addExtrusion(part, [Edge.result()], axis.result(), 10, 0)
                            plane.execute(True)
                            self.lfeatures.append(plane)
                            self.infoPoints[id_noeud]["plane"] = plane.result()
#==========================================================

# Execution of the Import

    def execute(self):
        """F.execute() -- execute the Feature"""
        # Retrieving the user input
        apath    = self.string(self.FILE_ID())

        filepath = apath.value()
        if filepath != "" :
            part = model.activeDocument()

            if self.lfeatures :
                for feature in self.lfeatures:
                    part.removeFeature(feature.feature())
                self.lfeatures = list()
                model.removeFolder(self.folder)

            self.infoPoints = dict()
            self.connectivities = dict()

            from os.path import basename
            filename = basename(filepath)
            if ( "." in filename ):
                laux= filename.split(".")
                nameRes = laux[0]
                for saux in laux[1:-1]:
                    nameRes+="."+saux
            else:
                nameRes = filename

            # Creating the construction points in the current document
            lFeatures = list()
            lVertices = list()
            lCodesPipes = list()
            lPipeSupports = dict()
            lPipes = list()

            # Décodage du fichier
            error = 0
            while not error:
                with open(filepath) as afile:
                    summary = 0
                    method = ""
                    for line in afile:
                        print (line[:-1])
                        if line == "\n":
                            print("========================= Saut de ligne =========================")
                            continue
                        if line[0] == "#" or line[:3] == "...":
                            continue
                        if summary == 0 and line[:-1] == "nodes section" :
                            print("========================= Lecture des noeuds =========================")
                            summary = 1
                            continue
                        if summary == 1 and line[:-1] == "connectivity section" :
                            print("========================= Lecture de la connectivite =========================")
                            summary = 2
                            continue
                        if summary == 2 and line[:6] == "method" :
                            print("===================== summary == 2 method =========================")
                            method = line[7:-1]
                            print(method)
                            if method != self.twopartwo and method != self.parligne:
                                raiseException("Problem with type of connectivity")
                            continue
                        if summary == 2 and line[:-1] == "fillets section" :
                            print("========================= Lecture des fillets =========================")
                            summary = 3
                            continue
                        if summary == 1:
                            print("===================== summary == 1 =========================")
                            diagno, texte = self.readNodeInfo(line[:-1])
                            if diagno:
                                raiseException("{}\nProblem with description of nodes.".format(texte))
                            continue
                        if summary == 2:
                            print("===================== summary == 2 =========================")
                            diagno = self.readConnectivity(line[:-1],method)
                            if diagno:
                                raiseException("Problem with description of connectivities")
                            continue
                        if summary == 3:
                            print("===================== summary == 3 =========================")
                            diagno = self.readFillet(line[:-1])
                            if diagno:
                                raiseException("Problem with description of fillets")
                            continue
                        print("===================== Rien =========================")
                        if diagno:
                            error = diagno
                            break

                if error:
                    break

                for key, value in self.connectivities.items():
                    self.infoPoints[value['chainage'][-1]]["isEnd"] = True

                if self._verbose:
                    print("infos points = " , self.infoPoints)
                    print("connectivities = " , self.connectivities)


                # Creation des points
                self.createPoints(part)

                # Creation des polylines
                self.createPolylines(part)

                # Creation des fillets
                self.createFillets(part)

                # Trouver les coudes droits
                self.searchRightConnections(part)


                # Création des paths pour le pipeNetwork
                print("========================= Création des paths =========================")
                for key, value in self.connectivities.items():
                    if self._verbose:
                        print("================================================================================= key = ", key, value['chainage'], value['fillet'])
                    # recherche des noeuds fillets
                    value["paths"] = list()
                    value["isPipe"] = list()
                    value["starts"] = list()
                    value["ends"] = list()
                    ind = 0
                    copy = value['fillet']
                    while ind < len(value['chainage'])-1:
                        value["starts"].append(self.connectivities[key]['chainage'][ind])
                        objectsForPath, ind, isPipe, end_noeud = self.retrieveSubshapesforWire(copy, key, ind)
                        if self._verbose:
                            print("************************* ind = ", ind)
                            print("************************* objectsForPath = ", objectsForPath)
                        path = model.addWire(part, objectsForPath, False)
                        path.execute(True)
                        self.lfeatures.append(path)
                        value["paths"].append(path)
                        value["isPipe"].append(isPipe)
                        value["ends"].append(end_noeud)
                        if ind < len(value['chainage'])-1:
                            copy = model.addCopy(part, [model.selection(copy.defaultResult())], 1)
                            copy.execute(True)
                            self.lfeatures.append(copy)


                # Création des sketchs pour le pipeNetwork
                print("========================= Création des sketchs =========================")
                for key, value in self.connectivities.items():
                    if self._verbose:
                        print("================================================================================= key = ", key)
                    # Creating sketch
                    edge = model.addEdge(part, self.infoPoints[value["chainage"][0]]["point"], self.infoPoints[value["chainage"][1]]["point"])
                    edge.execute(True)
                    self.lfeatures.append(edge)
                    plane = model.addPlane(part, edge.result(), self.infoPoints[value["chainage"][0]]["point"], True)
                    plane.execute(True)
                    self.lfeatures.append(plane)
                    sketch = model.addSketch(part, plane.result())
                    sketch.execute(True)
                    self.lfeatures.append(sketch)
                    SketchProjection = sketch.addProjection(self.infoPoints[value["chainage"][0]]["point"], False)
                    SketchProjection.execute(True)
                    SketchPoint = SketchProjection.createdFeature()
                    SketchPoint.execute(True)
                    SketchCircle = sketch.addCircle(0,0,self.radius)
                    SketchCircle.execute(True)
                    sketch.setCoincident(SketchPoint.result(), SketchCircle.center())
                    sketch.setRadius(SketchCircle.results()[1], self.radius)
                    sketch.execute(True)
                    model.do()
                    value["sketch"] = sketch.result()


                if self._verbose:
                    print("infos points = " , self.infoPoints)
                    print("********************************")
                    print("connectivities = " , self.connectivities)


                # Création des pipes
                print("========================= Création des pipes =========================")
                for key, value in self.connectivities.items():
                    if self._verbose:
                        print("================================================================================= key = ", key)
                    pipe = self.createPipe(part, value)
                    value["pipe"] = pipe.result()


                # Fusion des pipes
                print("========================= Fusion des pipes =========================")
                lPipes = list()
                for key, value in self.connectivities.items():
                    lPipes.append(value["pipe"])
                fuse = model.addFuse(part, lPipes, False)
                fuse.execute(True)
                self.lfeatures.append(fuse)
                fuse.result().setName(nameRes)
                self.folder = model.addFolder(part, self.lfeatures[0], self.lfeatures[-1])
                self.folder.setName(nameRes)

                break

            return

    def isMacro(self):
        """Override Feature.initAttributes().
        F.isMacro() -> True

        pipeNetwork feature is macro: removes itself on the creation transaction
        finish.
        """
        return False
