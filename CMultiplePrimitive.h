#pragma once
#include"IPrimitive.h"
#include<vector>


class CMultiplePrimitive :public  IPrimitive {

public:
	CMultiplePrimitive(std::vector<IPrimitive*> primitives,std::vector<bool> signum):
	primitives(primitives),signum(signum){}

	~CMultiplePrimitive() {
		for (int i = 0; i < primitives.size(); ++i) {
			delete primitives[i];
		}
	}

	bool intersect(Line3d& ray, Point3d& intersectionPoint) {
		Point3d closestPoint;
		long double min = 10e8;
		bool intersectionFlag = false;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(ray);
				for (int k = 0; k < points.size(); ++k) {
					bool flag = true;
					for (int j = 0; j < primitives.size(); ++j) {

						if ((signum[j] && !primitives[j]->isInPrimitive(points[k]))
							|| (!signum[j] && primitives[j]->isInPrimitive(points[k]))) {
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
		}
		intersectionPoint = closestPoint;
		return intersectionFlag;
	}
	
	Colour getColour(Line3d line, const Point3d intersectionPoint) {
		int primitiveId = -1;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(line);
				for (int k = 0; k < points.size(); ++k) {
					if (sign((points[k] - intersectionPoint).len()) == 0) {
						primitiveId = i;
					}
				}
			}
		}
		if (primitiveId == -1) {
			return Colour();
		}
		return primitives[primitiveId]->getColour(line, intersectionPoint);
	}

	Point3d getNormalVector(const Point3d& intersectionPoint) {
		int primitiveId = -1;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				if (primitives[i]->isInPrimitive(intersectionPoint)) {
					primitiveId = i;
				}
			}
		}
		if (primitiveId == -1) {
			return{ 0,0,0 };
		}
		return primitives[primitiveId]->getNormalVector(intersectionPoint);
	}

	Point3d getCenter() {
		if (!isCenter) {
			calculateCenter();
		}
		return center;
	}

	long double getTransparency(Line3d line) {
		for (int i = 0; i < primitives.size(); ++i) {
			if (primitives[i]->getTransparency(line) > 0)
				return primitives[i]->getTransparency(line);
		}	
	}

	long double getTransparency(Line3d line,Point3d point) {
		int primitiveId = -1;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(line);
				for (int k = 0; k < points.size(); ++k) {
					if (sign((points[k] - point).len()) == 0) {
						primitiveId = i;
					}
				}
			}
		}
		if (primitiveId == -1) {
			return 0;
		}
		return primitives[primitiveId]->getTransparency(line, point);
	}

	long double getReflection(Line3d line) {
		for (int i = 0; i < primitives.size(); ++i) {
			if (primitives[i]->getTransparency(line) > 0)
				return primitives[i]->getReflection(line);
		}
	}

	long double getReflection(Line3d line,Point3d point) {
		int primitiveId = -1;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(line);
				for (int k = 0; k < points.size(); ++k) {
					if (sign((points[k] - point).len()) == 0) {
						primitiveId = i;
					}
				}
			}
		}
		if (primitiveId == -1) {
			return 0;
		}
		return primitives[primitiveId]->getReflection(line, point);
	}

	bool isLightSource() {
		return false;
	}

	long double getIntencity(Line3d line) {
		return 0;
	}

	float getRefraction(Line3d line) {
		for (int i = 0; i < primitives.size(); ++i) {
			if (primitives[i]->getTransparency(line) > 0)
				return primitives[i]->getRefraction(line);
		}
	}

	float getRefraction(Line3d line,Point3d point) {
		int primitiveId = -1;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(line);
				for (int k = 0; k < points.size(); ++k) {
					if (sign((points[k] - point).len()) == 0) {
						primitiveId = i;
					}
				}
			}
		}
		if (primitiveId == -1) {
			return 0;
		}
		return primitives[primitiveId]->getRefraction(line, point);
	}

	CBoxPrimitive getBox() {
		if (!isBoxSet) {
			calculateBox();
		}
		return box;
	}


	std::vector<Point3d> getPhantomReflectionSource(Point3d source) {
		std::vector<Point3d> result;
		for (int i = 0; i < primitives.size(); ++i) {
			Line3d line{ source,primitives[i]->getCenter() };
			if (primitives[i]->getRefraction(line) > 0) {
				std::vector<Point3d> temp = primitives[i]->getPhantomReflectionSource(source);
				result.insert(result.end(), temp.begin(), temp.end());
			}
		}
		return result;
	}

	std::vector<Point3d> getPhantomRefractionSource(Point3d source) {
		std::vector<Point3d> result;
		for (int i = 0; i < primitives.size(); ++i) {
			Line3d line{ source,primitives[i]->getCenter() };
			if (primitives[i]->getRefraction(line) > 0) {
				std::vector<Point3d> temp = primitives[i]->getPhantomRefractionSource(source);
				result.insert(result.end(), temp.begin(), temp.end());
			}
		}
		return result;
	}

	std::vector<Point3d> getAllIntersections(Line3d line) {
		std::vector<Point3d> result;
		for (int i = 0; i < primitives.size(); ++i) {
			if (signum[i]) {
				std::vector<Point3d> points = primitives[i]->getAllIntersections(line);
				for (int k = 0; k < points.size(); ++k) {
					bool flag = true;
					for (int j = 0; j < primitives.size(); ++j) {
						if ((signum[j] && !primitives[j]->isInPrimitive(points[k]))
							|| (!signum[j] && primitives[j]->isInPrimitive(points[k]))) {
							flag = false;
						}
					}
					if (flag) {
						result.push_back(points[k]);
					}
				}
			}
		}
		return result;
	}

	bool isInPrimitive(Point3d point) {
		bool flag = true;
		for (int j = 0; j < primitives.size(); ++j) {
			if ((signum[j] && !primitives[j]->isInPrimitive(point))
				|| (!signum[j] && primitives[j]->isInPrimitive(point))) {
				flag = false;
			}
		}
		return flag;
	}

private:

	void calculateCenter() {
		center = { 0,0,0 };
		for (int i = 0; i < primitives.size(); ++i) {
			center = center + primitives[i]->getCenter() * (1.0 /  primitives.size());
		}
	}

	void calculateBox() {
		if (primitives.size() > 0) {
			box = primitives[0]->getBox();
			for (int i = 1; i < primitives.size(); ++i) {
				box.enlagreBox(primitives[i]->getBox());
			}
		}
	}

	std::vector<IPrimitive*> primitives;
	std::vector<bool> signum;
	bool isCenter = false;
	Point3d center;
	CBoxPrimitive box;
	bool isBoxSet = false;
};