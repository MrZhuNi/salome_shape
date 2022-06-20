from GeomAlgoAPI import GeomAlgoAPI_CanonicalRecognition

class CanonicalRecognition:
    "The class provides recognition of canonical shapes"

    def isPlane(self, shape, tolerance, normal = [], origin = []):
        "Check if shape is planar"
        return GeomAlgoAPI_CanonicalRecognition.isPlane(shape, tolerance, normal, origin)

    def isSphere(self, shape, tolerance, origin = [], radius = 0.0):
        "Check if shape is spherical"
        return GeomAlgoAPI_CanonicalRecognition.isSphere(shape, tolerance, origin, radius)

    def isCone(self, shape, tolerance, axis = [], apex = [], halfAngle = 0.0):
        "Check if shape is conical"
        return GeomAlgoAPI_CanonicalRecognition.isCone(shape, tolerance, axis, apex, halfAngle)

    def isCylinder(self, shape, tolerance, axis = [], origin = [], radius = 0.0):
        "Check if shape is cylinder"
        return GeomAlgoAPI_CanonicalRecognition.isCylinder(shape, tolerance, axis, origin, radius)

    def isLine(self, edge, tolerance, direction = [], origin = []):
        "Check if edge/wire is line"
        return GeomAlgoAPI_CanonicalRecognition.isLine(edge, tolerance, direction, origin)

    def isCircle(self, edge, tolerance, normal = [], origin = [], radius = 0.0):
        "Check if edge/wire is circle"
        return GeomAlgoAPI_CanonicalRecognition.isCircle(edge, tolerance, normal, origin, radius)

    def isEllipse(self, edge, tolerance, normal = [], dirX = [], origin = [], majorRadius = 0.0, minorRadius = 0.0):
        "Check if edge/wire is ellipse"
        return GeomAlgoAPI_CanonicalRecognition.isEllipse(edge, tolerance, normal, dirX, origin, majorRadius, minorRadius)
