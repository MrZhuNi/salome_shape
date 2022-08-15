from shapervssam.utils.logger import logger

from shapervssam.shaper_to_sam.convert_primitive import ShapertoSAMPrimitive
from shapervssam.shaper_to_sam.convert_constraints import ShapertoSAMConstraints
from sam.sketch import Sketch

from ModelAPI import *
from SketchAPI import *
from GeomDataAPI import *


def convert_sketch(sketch: object): # a shaper sketch

	exchange_sketch = Sketch()

	mapping = {}
	# Add the primitives
	for sub in sketch.features().list():
		feat =  ModelAPI.objectToFeature(sub)

		if feat is not None :
			entity = SketchAPI_SketchEntity(feat)
			entity_type = entity.getKind()

			convert, update_mapping = ShapertoSAMPrimitive.convert(entity)
			if convert is not None:
				mapping[entity.name()] = convert

			mapping.update(update_mapping)
			
			if convert is not None:
				exchange_sketch.add(convert)

	logger.debug(f'Mapping; {mapping}')

	# Add the constraints
	sketchFeature = featureToCompositeFeature(sketch.feature())
	n = sketchFeature.numberOfSubs()
	for i in range(n):
		entity = sketchFeature.subFeature(i)
		entity_type = entity.getKind()

		if 'Constraint' in entity_type :
			refs = []
			l_attributs = [entity.refattr("ConstraintEntityA"), entity.refattr("ConstraintEntityB"), 
						entity.refattr("ConstraintEntityC"), entity.refattr("ConstraintEntityD")]
			for ref in l_attributs:
				if ref is None: continue
				if ref.isObject():
					attr = ModelAPI_Feature.feature(ref.object())
					refs.append(mapping[attr.name()])

				else :
					attr = ref.attr()
					owner = objectToFeature(attr.owner())
					elt = geomDataAPI_Point2D(attr).pnt()
					refs.append(mapping.get((elt.x(), elt.y(), owner.name()), owner.name()))

			convert = ShapertoSAMConstraints.convert(entity, refs)
			exchange_sketch.add(convert)
	
	return exchange_sketch
			

