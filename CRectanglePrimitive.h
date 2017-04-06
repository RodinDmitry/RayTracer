#pragma once
#include"IPrimitive.h"
#include"geometry3d.h"

class CRectanglePrimitive :public  IPrimitive {

public:
	CRectanglePrimitive() {}
	CRectanglePrimitive(Point3d a, Point3d b,
		Point3d c,Point3d d, Attributes attr,Attributes anti): A(a), B(b), 
		C(c), D(d), attributesNormal(attr),attributesAntiNormal(anti) {};

	bool intersect( Line3d& ray, Point3d& intersectionPoint) {
		if (linePlaneIntersection(Plane{ A,B,C }, ray, intersectionPoint)) {
			return pointInTriangle(A, B, C, intersectionPoint) || pointInTriangle(A, D, C, intersectionPoint);
		}
		return false;
	}

	Colour getColour(Line3d line,const Point3d intersectionPoint) {
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
			center = (A + B + C + D) * (1 / 4);
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

	long double getTransparency(Line3d line, Point3d point) {
		return getTransparency(line);
	}

	long double getReflection(Line3d line) {
		if (sign(line.getVector() ^ normalVector) < 0) {
			return attributesNormal.reflection;
		}
		else {
			return attributesAntiNormal.reflection;
		}
	}

	long double getReflection(Line3d line, Point3d point) {
		return getReflection(line);
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

	float getRefraction(Line3d line, Point3d point) {
		return getRefraction(line);
	}


	CBoxPrimitive getBox() {
		return CBoxPrimitive{
			Point3d{ std::max(std::max(A.x,B.x),std::max(C.x,D.x)),
			std::max(std::max(A.y,B.y),std::max(C.y,D.y)),
			std::max(std::max(A.z, B.z), std::max(C.z,D.z)) },
			Point3d{ std::min(std::min(A.x,B.x),std::min(C.x,D.x)),
			std::min(std::min(A.y,B.y),std::min(C.y,D.y)),
			std::min(std::min(A.z, B.z), std::min(C.z,D.z)) } };
	}

	std::vector<Point3d> getPhantomReflectionSource(Point3d source) {
		if (!isNormalVector) {
			getNormalVector(A);
		}
		Point3d resultPoint = pointOnPlaneProjection(source, A, normalVector);
		std::vector<Point3d> result;
		result.push_back(source + 2 * (resultPoint - source));
		return result;
	}

	std::vector<Point3d> getPhantomRefractionSource(Point3d source) {
		if (!isNormalVector) {
			getNormalVector(A);
		}
		float k = getRefraction({ source,getCenter() });
		Point3d projectionPoint = pointOnPlaneProjection(source, A, normalVector);
		Point3d refractionPoint = refractVector(A - source, normalVector, k);
		Point3d result;
		lineIntersection(Line3d{ refractionPoint,refractionPoint * 2 }, Line3d{ source,projectionPoint }, result);
		std::vector<Point3d> resultVector;
		resultVector.push_back(result);
		return resultVector;

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
		return pointInTriangle(A, B, C, point) || pointInTriangle(A, D, C, point);
	}

private:

	Point3d A;
	Point3d B;
	Point3d C;
	Point3d D;

	Attributes attributesNormal;
	Attributes attributesAntiNormal;

	Point3d normalVector;
	bool isNormalVector = false;
	Point3d center;
	bool isCenter = false;


};