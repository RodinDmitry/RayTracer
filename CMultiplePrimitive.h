#pragma once
#include"IPrimitive.h"
#include<vector>


class CMultiplePrimitive :public  IPrimitive {

public:
	CMultiplePrimitive() {}

	bool intersect(Line3d& ray, Point3d& intersectionPoint) {
		Point3d closestPoint;
		long double min = 10e8;
		bool intersectionFlag = false;
		for (int i = 0; i < primitives.size(); ++i) {
			std::vector<Point3d> points = primitives[i]->getAllIntersections(ray);
			for (int k = 0; k < points.size(); ++k) {
				bool flag = true;
				for (int j = 0; j < primitives.size(); ++j) {

					if ((signum[j] && !primitives[i]->isInPrimitive(points[k]))
						|| (!signum[j] && primitives[i]->isInPrimitive(points[k]))) {
						flag = false;
					}
				}
				if (flag && ((points[k] - ray.start).len() < min)) {
					min = (points[k] - ray.start).len();
					closestPoint = points[k];
					intersectionFlag = true;
				}
			}
		}
		intersectionPoint = closestPoint;
		return intersectionFlag;
	}
	
	Colour getColour(Line3d line, const Point3d intersectionPoint) {
		
	}

	Point3d getNormalVector(const Point3d& intersectionPoint) {
		
	}

	Point3d getCenter() {
		
	}

	long double getTransparency(Line3d line) {
		
	}

	long double getReflection(Line3d line) {
		
	}

	bool isLightSource() {
		
	}

	long double getIntencity(Line3d line) {
		
	}

	float getRefraction(Line3d line) {
		
	}

	CBoxPrimitive getBox() {
		
	}


	Point3d getPhantomReflectionSource(Point3d source) {
		
	}

	Point3d getPhantomRefractionSource(Point3d source, float k) {
		
	}

	std::vector<Point3d> getAllIntersections(Line3d line) {
		
	}

	bool isInPrimitive(Point3d point) {
		
	}

private:

	
	std::vector<IPrimitive*> primitives;
	std::vector<bool> signum;

};