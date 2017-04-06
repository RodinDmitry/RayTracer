#pragma once
#include"Colour.h"
#include"geometry3d.h"
#include"Light.h"
#include"Attributes.h"
#include"CBoxPrimitive.h"
#include<vector>





class IPrimitive {
public:

	virtual bool intersect(Line3d& ray, Point3d& intersectionPoint) = 0;

	virtual Colour getColour(Line3d line, const Point3d intersectionPoint) = 0;

	virtual Point3d getNormalVector(const Point3d& intersection_point) = 0;

	virtual Point3d getCenter() = 0;

	virtual long double getTransparency(Line3d line) = 0;

	virtual long double getTransparency(Line3d line,Point3d point) = 0;

	virtual long double getReflection(Line3d line) = 0;

	virtual long double getReflection(Line3d line,Point3d point) = 0;

	virtual float getRefraction(Line3d line) = 0;

	virtual float getRefraction(Line3d line,Point3d point) = 0;

	virtual bool isLightSource() = 0;

	virtual long double getIntencity(Line3d line) = 0;

	virtual CBoxPrimitive getBox() = 0;

	virtual std::vector<Point3d> getPhantomReflectionSource(Point3d source) = 0;

	virtual std::vector<Point3d> getPhantomRefractionSource(Point3d source) = 0;

	virtual std::vector<Point3d> getAllIntersections(Line3d line) = 0;

	virtual bool isInPrimitive(Point3d point) = 0;

};