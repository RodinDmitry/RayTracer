#pragma once
#include"IPrimitive.h"
#include"geometry3d.h"


class CTrianglePrimitive :public  IPrimitive {

public:

	CTrianglePrimitive() {}
	CTrianglePrimitive(Point3d a, Point3d b, Point3d c, Attributes attr, Attributes anti) :
		A(a), B(b), C(c), attributesNormal(attr), attributesAntiNormal(anti) {};

	bool intersect(Line3d& ray, Point3d& intersectionPoint) {
		if (linePlaneIntersection(Plane{ A,B,C }, ray, intersectionPoint)) {
			return pointInTriangle(A, B, C, intersectionPoint);
		}
		return false;
	}

	Colour getColour(Line3d line, const Point3d intersectionPoint) {
		if (!isNormalVector) {
			getNormalVector(intersectionPoint);
		}
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.colour;
		}
		else {
			return attributesAntiNormal.colour;
		}
	}

	Point3d getNormalVector(const Point3d& intersectionPoint) {
		if (!isNormalVector) {
			normalVector = (B - A) * (C - A);
		}
		return normalVector;
	}

	Point3d getCenter() {
		if (!isCenter) {
			center = (A + B + C) * (1 / 3);
		}
		return center;
	}

	long double getTransparency(Line3d line) {
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.transparency;
		}
		else {
			return attributesAntiNormal.transparency;
		}
	}

	long double getReflection(Line3d line) {
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.reflection;
		}
		else {
			return attributesAntiNormal.reflection;
		}
	}

	bool isLightSource() {
		return attributesAntiNormal.lightSource || attributesNormal.lightSource;
	}

	long double getIntencity(Line3d line) {
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.intensity;
		}
		else {
			return attributesAntiNormal.intensity;
		}
	}

	float getRefraction(Line3d line) {
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.refraction;
		}
		else {
			return attributesAntiNormal.refraction;
		}
	}

	CBoxPrimitive getBox() {
		return CBoxPrimitive{
		Point3d{std::max(std::max(A.x,B.x),C.x),
		std::max(std::max(A.y,B.y),C.y),
		std::max(std::max(A.z, B.z), C.z)},
		Point3d{std::min(std::min(A.x,B.x),C.x),
		std::min(std::min(A.y,B.y),C.y),
		std::min(std::min(A.z, B.z), C.z) }};
	}


	Point3d getPhantomReflectionSource(Point3d source) {
		if (!isNormalVector) {
			getNormalVector(A);
		}
		Point3d resultPoint = pointOnPlaneProjection(source, A, normalVector);
		return source + 2 * (resultPoint - source);
	}

	Point3d getPhantomRefractionSource(Point3d source, float k) {
		if (!isNormalVector) {
			getNormalVector(A);
		}
		Point3d projectionPoint = pointOnPlaneProjection(source, A, normalVector);
		Point3d refractionPoint = refractVector(A - source, normalVector, k);
		Point3d result;
		lineIntersection(Line3d{ refractionPoint,refractionPoint * 2 }, Line3d{ source,projectionPoint }, result);
		return result;
	}

	std::vector<Point3d> getAllIntersections(Line3d line) {
		Point3d resultPoint;
		std::vector<Point3d> resultVector;
		if (intersect(line, resultPoint)) {
			resultVector.push_back(resultPoint);
		}
		return resultVector;
	}

	bool isInPrimitive(Point3d point) {
		return pointInTriangle(A, B, C, point);
	}

private:

	Point3d A;
	Point3d B;
	Point3d C;
	Attributes attributesNormal;
	Attributes attributesAntiNormal;


	Point3d normalVector;
	bool isNormalVector = false;
	Point3d center;
	bool isCenter = false;


};