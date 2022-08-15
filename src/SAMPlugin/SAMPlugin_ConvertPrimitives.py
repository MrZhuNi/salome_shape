from sam.catalog_primitive import Arc, Line, Circle, Point

import math

from SketchAPI import *
from GeomAPI import *
from shapervssam.utils.logger import logger

def convert_angle(angle):
    a =  int(angle / 360)
    if math.fabs(angle)  >= 360. :
        return angle - a * 360
    else :
        return angle 

class ShapertoSAMPrimitive:

    def convert(entity):

        if entity.getKind() == 'SketchPoint':
            return ShapertoSAMPrimitive.convert_point(entity)
        if entity.getKind() == 'SketchArc':
            return ShapertoSAMPrimitive.convert_arc(entity)
        if entity.getKind() == 'SketchCircle':
            return ShapertoSAMPrimitive.convert_circle(entity)
        if entity.getKind() == 'SketchLine':
            return ShapertoSAMPrimitive.convert_line(entity)
        return None, {}
    
    def check_construction(entity):
        auxAttr = entity.auxiliary()
        return auxAttr is not None and auxAttr.value()

    def convert_point(entity):
        feat = SketchAPI_Point(entity)   
        p1 = feat.coordinates()
        return Point(status_construction= ShapertoSAMPrimitive.check_construction(entity), point = [p1.x(), p1.y()]), {}

    def convert_line(entity):
        feat = SketchAPI_Line(entity)

        p1 = feat.startPoint()
        p2 = feat.endPoint()
        line = Line(status_construction=ShapertoSAMPrimitive.check_construction(entity), pnt1= [p1.x(), p1.y()], pnt2= [p2.x(), p2.y()])
        return line, {(p1.x(), p1.y(), feat.name()) : line.pnt1, (p2.x(), p2.y(), feat.name()) : line.pnt2}

    def convert_circle(entity):
        feat = SketchAPI_Circle(entity) 
        c = feat.center()  
        radius = feat.radius()

        circle =  Circle(status_construction=ShapertoSAMPrimitive.check_construction(entity), 
                        center = [c.x(), c.y()],
                        radius= radius.value())#feat.radius().value())
        return circle, {(c.x(), c.y(), feat.name()) : circle.center}

    def convert_arc(entity):
        status = ShapertoSAMPrimitive.check_construction(entity)
        feat = SketchAPI_Arc(entity)
        c = feat.center()
        s = entity.defaultResult()
        edge = GeomAPI_Edge(s.shape())
        a0,a1 = edge.getRange()
        start_angle = convert_angle(math.degrees(a0))
        end_angle = convert_angle(math.degrees(a1))

        arc = Arc(status_construction=status, 
                        center = [c.x(), c.y()],
                        radius= feat.radius().value(),
                        angles= [start_angle, end_angle])
        arc.add_points_startend()
        return arc, {(c.x(), c.y(), feat.name()) : arc.center}
