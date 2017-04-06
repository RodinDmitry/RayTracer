#pragma once
#include"geometry3d.h"

class CBoxPrimitive {
public:

	CBoxPrimitive() {}

	CBoxPrimitive(Point3d pointMax, Point3d pointMin) 
		: maxPoint(pointMax), minPoint(pointMin) {}

	void getBox(Point3d& minPoint, Point3d& maxPoint) {
		minPoint = this->minPoint;
		maxPoint = this->maxPoint;
	}

	bool lineBoxInterseption(Line3d line) {
		Plane front{ minPoint,Point3d{minPoint.x,minPoint.y,maxPoint.z},Point3d{minPoint.x,maxPoint.y,minPoint.z} };
		Plane left{ minPoint,Point3d{ minPoint.x,minPoint.y,maxPoint.z },Point3d{ maxPoint.x,minPoint.y,minPoint.z } };
		Plane down{ minPoint,Point3d{ minPoint.x,maxPoint.y,minPoint.z },Point3d{ maxPoint.x,minPoint.y,minPoint.z } };
		Plane back{ maxPoint,Point3d{ maxPoint.x,minPoint.y,maxPoint.z },Point3d{ maxPoint.x,maxPoint.y,minPoint.z } };
		Plane right{ maxPoint,Point3d{ minPoint.x,maxPoint.y,maxPoint.z },Point3d{ maxPoint.x,maxPoint.y,minPoint.z } };
		Plane up{ maxPoint,Point3d{ minPoint.x,maxPoint.y,maxPoint.z },Point3d{ maxPoint.x,minPoint.y,maxPoint.z } };
		Point3d intersection;
		if (linePlaneIntersection(front, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		if (linePlaneIntersection(left, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		if (linePlaneIntersection(down, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		if (linePlaneIntersection(back, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		if (linePlaneIntersection(right, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		if (linePlaneIntersection(up, line, intersection)) {
			if (pointInBox(intersection)) {
				return true;
			}
		}
		return false;
	}

	long double getVolume() {
		return (maxPoint.x - minPoint.x) * (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z);
	}


	long double getArea() {
		return  (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (maxPoint.z - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (maxPoint.y - minPoint.y);
	}
	
	long double getAreaMaxLimit(int axis, long double limit) {
		switch (axis)
		{
		case 0: return (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z) 
		+ (limit - minPoint.x) * (maxPoint.z - minPoint.z) 
		+ (limit - minPoint.x) * (maxPoint.y - minPoint.y);
		case 1: return (limit - minPoint.y) * (maxPoint.z - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (maxPoint.z - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (limit - minPoint.y);
		case 2: (maxPoint.y - minPoint.y) * (limit - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (limit - minPoint.z) 
		+ (maxPoint.x - minPoint.x) * (maxPoint.y - minPoint.y);
		}
	}

	long double getVolumeMaxLimit(int axis,long double maxlimit) {
		switch (axis)
		{
		case 0: return (maxlimit - minPoint.x) * (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z);
		case 1: return (maxPoint.x - minPoint.x) * (maxlimit - minPoint.y) * (maxPoint.z - minPoint.z);
		case 2: return (maxPoint.x - minPoint.x) * (maxPoint.y - minPoint.y) * (maxlimit - minPoint.z);
		}
	}

	void setMaxPoint(Point3d point) {
		maxPoint = point;
	}

	void setMinPoint(Point3d point) {
		maxPoint = point;
	}

	long double getMaxPoint(int axis) {
		switch (axis)
		{
			case 0: return maxPoint.x;
			case 1: return maxPoint.y;
			case 2: return maxPoint.z;
		}
	}

	long double getMinPoint(int axis) {
		switch (axis)
		{
		case 0: return minPoint.x;
		case 1: return minPoint.y;
		case 2: return minPoint.z;
		}
	}

	void enlagreBox(CBoxPrimitive box) {
		maxPoint = Point3d{ std::max(maxPoint.x,box.getMaxPoint(0))
			,std::max(maxPoint.y,box.getMaxPoint(1))
			, std::max(maxPoint.z,box.getMaxPoint(2)) };
		minPoint = Point3d{ std::min(minPoint.x,box.getMinPoint(0))
			,std::min(minPoint.y,box.getMinPoint(1))
			, std::min(minPoint.z,box.getMinPoint(2)) };
	}

	bool pointInBox(Point3d point) {
		if (sign(maxPoint.x - point.x) >= 0 && sign(point.x - minPoint.x) >= 0
			&& sign(maxPoint.y - point.y) >= 0 && sign(point.y - minPoint.y) >= 0
			&& sign(maxPoint.z - point.z) >= 0 && sign(point.z - minPoint.z) >= 0) {
			return true;
		}
		return false;
	}
private:
	Point3d maxPoint;
	Point3d minPoint;
};