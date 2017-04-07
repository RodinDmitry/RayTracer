#pragma once
#include"IPrimitive.h"

class CPointPrimitive :public  IPrimitive {

public:
	CPointPrimitive() {}
	CPointPrimitive(Point3d a,Attributes* attr) :A(a), attributes(attr) {};

	bool intersect( Line3d& ray, Point3d& intersectionPoint) {
		intersectionPoint = A;
		if (isOnLine(ray, A)) {
			return true;
		}
		return false;
	}

	Colour getColour(Line3d line, const Point3d intersectionPoint) {
		return attributes->colour;
	}

	Point3d getNormalVector(const Point3d& intersectionPoint) {
		return Point3d{ 0,0,0 };
	}

	Point3d getCenter() {
		return A;
	}

	long double getTransparency(Line3d line) {
		return attributes->transparency;
	}

	long double getReflection(Line3d line) {
		return attributes->reflection;
	}

	long double getTransparency(Line3d line, Point3d point) {
		return attributes->transparency;
	}

	long double getReflection(Line3d line, Point3d point) {
		return attributes->reflection;
	}

	bool isLightSource() {
		return attributes->lightSource;
	}

	long double getIntencity(Line3d line) {
		return attributes->intensity;
	}
	
	CBoxPrimitive getBox() {
		return CBoxPrimitive{ A,A };
	}

	float getRefraction(Line3d line) {
		return attributes->refraction;
	}

	float getRefraction(Line3d line,Point3d point) {
		return attributes->refraction;
	}

	std::vector<Point3d> getPhantomReflectionSource(Point3d source) {
		return { A };
	}

	std::vector<Point3d> getPhantomRefractionSource(Point3d source) {
		return { A };
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
		if (sign((A - point).len2() == 0)) {
			return true;
		}
		return false;
	}

private:

	Point3d A;

	Attributes* attributes;


};