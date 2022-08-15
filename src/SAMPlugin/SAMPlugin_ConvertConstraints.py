from sam.catalog_constraint import *

import math

from SketchAPI import *
from shapervssam.utils.logger import logger


def get_params_value(entity): 
    try :
        value_attr = entity.feature().real("ConstraintValue")
        if value_attr is not None:
            return value_attr.value()
    except Exception as e :
        logger.info(f'Problem with constraint parameters: {e}')
    return None 

class ShapertoSAMConstraints:

    def convert(entity, refs):

        if entity.getKind() == 'SketchConstraintAngle':
            return Angle(references = refs, angle = get_params_value(entity)) 

        if entity.getKind() == 'SketchConstraintCoincidence':
            return Coincident(references = refs)

        if entity.getKind() == 'SketchConstraintDistance':
            return Distance(references = refs, distance_min = get_params_value(entity)) 

        if entity.getKind() == 'SketchConstraintDistanceHorizontal':
            return HorizontalDistance(references = refs, distance_min = get_params_value(entity)) 

        if entity.getKind() == 'SketchConstraintDistanceVertical':
            return VerticalDistance(references = refs, distance_min = get_params_value(entity)) 

        if entity.getKind() == 'SketchConstraintEqual':
            return Equal(references = refs)
        
        if entity.getKind() == 'SketchConstraintHorizontal':
            return Horizontal(references = refs)
    
        if entity.getKind() == 'SketchConstraintMiddle' :
            return Midpoint(references = refs)

        if entity.getKind() == 'SketchConstraintParallel' :
            return Parallel(references = refs)

        if entity.getKind() == 'SketchConstraintPerpendicular' :
            return Perpendicular(references = refs)

        if entity.getKind() == 'SketchConstraintTangent' :
            return Tangent(references = refs)

        if entity.getKind() == 'SketchConstraintVertical':
            return Vertical(references = refs)


        if entity.getKind() == 'SketchConstraintRadius':

            return Radius(references = refs, radius = get_params_value(entity)) 
        

        if entity.getKind() == 'SketchConstraintLength':
            return Length(references = refs, length = get_params_value(entity)) 


        return None



        