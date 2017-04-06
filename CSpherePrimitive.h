#pragma once
#include"IPrimitive.h"

class CSpherePrimitive :public  IPrimitive {

public:
	CSpherePrimitive() {}
	CSpherePrimitive(Point3d center, long double radius, Attributes attr) 
		:center(center), radius(radius), attributes(attr) {};

	bool intersect( Line3d& ray, Point3d& intersectionPoint) {
		long double dist = linePointDistance(ray, center);
		long double length = sqrtl((center - ray.start).len2() - dist * dist);
		long double delta = sqrtl(radius * radius - dist * dist);
		length -= delta;

		if (sign(length) <= 0) {
			length += 2 * dist;
			intersectionPoint = ray.start - ray.getVector() * (length / ray.getVector().len());
			return true;
		}

		if (sign(dist - radius) > 0) {
			return false;
		}

		if (sign((center - ray.start) ^ (ray.end - ray.start)) > 0) {
			intersectionPoint = ray.start + ray.getVector() * (length / ray.getVector().len());
		}
		else {
			length += 2 * dist;
			intersectionPoint = ray.start - ray.getVector() * (length / ray.getVector().len());
		}
		return true;

	}

	Colour getColour(Line3d line, const Point3d intersectionPoint) {
		return attributes.colour;
	}

	Point3d getNormalVector(const Point3d& intersectionPoint) {
		return intersectionPoint - center;
	}

	Point3d getCenter() {
		return center;
	}

	long double getTransparency(Line3d line) {
		return attributes.transparency;
	}

	long double getTransparency(Line3d line,Point3d point) {
		return getTransparency(line);
	}

	long double getReflection(Line3d line) {
		return attributes.reflection;
	}

	long double getReflection(Line3d line,Point3d point) {
		return getReflection(line);
	}


	bool isLightSource() {
		return attributes.lightSource;
	}

	long double getIntencity(Line3d line) {
		return attributes.intensity;
	}

	CBoxPrimitive getBox() {
		return CBoxPrimitive{center + Point3d{ radius,radius,radius } ,center - Point3d{ radius,radius,radius } };
		
	}

	float getRefraction(Line3d line) {
		return attributes.refraction;
	}

	float getRefraction(Line3d line,Point3d point) {
		return getRefraction(line);
	}


	std::vector<Point3d> getPhantomReflectionSource(Point3d source) {
		Point3d shift = source - center;
		shift = shift * (1 / shift.len());
		shift = shift * (radius / 2);
		std::vector<Point3d> resultVector;
		resultVector.push_back(center + shift);
		return resultVector;
	}

	std::vector<Point3d> getPhantomRefractionSource(Point3d source) {
		if (!isNormalVector) {
			getNormalVector(center);
		}
		float k = getRefraction({ source,getCenter() });
		std::vector<Point3d> resultVector;
		Point3d resultPoint;
		int result;
		Line3d line{ source,(center + Point3d(radius / 2,radius / 2,radius / 2)) };
		intersect(line, resultPoint);
		Point3d refractionPoint = refractVector(resultPoint - source, getNormalVector(resultPoint), k);
		lineIntersection(Line3d{ refractionPoint,refractionPoint * 2 }, 
			Line3d{ source,center }, resultPoint);
		resultVector.push_back(resultPoint);
		return resultVector;
	}

	std::vector<Point3d> getAllIntersections(Line3d line) {
		
		std::vector<Point3d> resultVector;
		long double dist = linePointDistance(line, center);
		long double length = sqrtl((center - line.start).len2() - dist * dist);
		long double delta = sqrtl(radius * radius - dist * dist);
		length -= delta;
		if (sign(dist - radius) > 0) {
			return resultVector;
		}
		resultVector.push_back(line.start + line.getVector() * (length / line.getVector().len()));
		length += 2 * dist;
		resultVector.push_back(line.start - line.getVector() * (length / line.getVector().len()));
		return resultVector;
	}

	bool isInPrimitive(Point3d point) {
		return sign((center - point).len() - radius) <= 0;
	}

private:

	Point3d center;
	long double radius;
	Attributes attributes;


	Point3d normalVector;
	bool isNormalVector = false;
	bool isCentre = false;


};