#pragma once
#include<math.h>
#include<iostream>


const long double eps = 1e-7;

struct Point3d {

	long double x = 0;
	long double y = 0;
	long double z = 0;

	Point3d() {};

	Point3d(long double x_, long double y_, long double z_) : x(x_), y(y_), z(z_) {};

	long double len2() const {
		return x*x + y*y + z*z;
	}

	long double len() const {
		return sqrtl(len2());
	}
};


Point3d operator*(Point3d a, Point3d b) {
	return{ a.y * b.z - a.z * b.y,
		-1 * (a.x * b.z - a.z * b.x) ,
		a.x * b.y - a.y * b.x };
}

Point3d operator*(long double a, Point3d b) {
	return{ a * b.x,a * b.y , a * b.z };
}

Point3d operator*(Point3d a, long double b) {
	return b * a;
}

long double operator^(Point3d a, Point3d b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point3d operator+(Point3d a, Point3d b) {
	return{ a.x + b.x, a.y + b.y, a.z + b.z};
}

Point3d operator-(Point3d a, Point3d b) {
	return{ a.x - b.x, a.y - b.y, a.z - b.z };
}

bool isZero(const long double z) {
	return fabsl(z) < eps;
}

long double sign(const long double z) {
	if (isZero(z))
		return 0;
	else
		return (z > 0) ? 1 : -1;
}

bool areEqual(long double a, long double b) {
	return sign(a - b) >= 0;
}

bool areCollinear(Point3d p1, Point3d p2) {
	return sign((p1 * p2).len2()) == 0;
}

struct Line3d {
	Point3d start;
	Point3d end;

	Point3d getVector() {
		return end - start;
	}
};

bool isOnLine(Line3d l, Point3d p1) {
	return areCollinear(l.start - p1, l.end - p1);
}


bool isOnLine(Point3d a, Point3d b, Point3d p) {
	return isOnLine(Line3d{a, b}, p);
}


bool isOnSegment(Line3d l, Point3d p) {
	return isOnLine(l, p) && sign((l.start - p) ^ (l.end - p)) <= 0;
}

bool isOnSegment(Point3d a, Point3d b, Point3d p) {
	return isOnSegment(Line3d{ a,b }, p);
}

bool lineIntersection(Line3d a, Line3d b,Point3d& result) {

	Point3d e = a.getVector();
	Point3d f = b.getVector();
	Point3d g = b.start - a.start;
	if (isZero((a.start - b.start).len2())) {
		result = a.start;
		return true;
	}
	if (isZero((a.end - b.end).len2())) {
		result = a.end;
		return true;
	}
	long double h = sqrtl((f * g).len2());
	long double k = sqrtl((f * e).len2());
	if (isZero(h) || isZero(k))
		return false;
	Point3d l = (h / k) * e;
	if (isOnLine(b, a.start + l)) {
		result = a.start + l;
	}
	else {
		result = a.start - l;
	}
	return true;
}

struct Plane {
	Point3d a;
	Point3d b;
	Point3d c;
	Point3d getNormalVector() {
		return (b - a) * (c - a);
	}
};

long double tripleProduct(Point3d a, Point3d b, Point3d c) {
	return a ^ (b * c);
}

bool pointOnPlane(Plane pln, Point3d a) {
	Point3d a1 = a - pln.a;
	Point3d a2 = pln.getNormalVector();
	long double result = a1 ^ a2;
	if (isZero(pln.getNormalVector() ^ (a - pln.a))) {
		return true;
	}
	return false;
}

bool linePlaneIntersection(Plane pln, Point3d p1, Point3d p2, Point3d& intersect)
{
	Point3d n = pln.getNormalVector();
	if (isZero(n ^ (p2 - p1))) {
		if (pointOnPlane(pln,p1)) {
			intersect = p1;
			return true;
		}
		return false;
	}
	long double u = (n ^ (pln.a - p1)) / (n ^ (p2 - p1));

	intersect = p1 + (p2 - p1) * u;
	if (pointOnPlane(pln, intersect)) {
		return true;
	}
	else {
		intersect = p1 - (p2 - p1) * u;
	}
	return true;
}

bool linePlaneIntersection(Plane pln, Line3d ln, Point3d& intersect)
{
	return linePlaneIntersection(pln, ln.start, ln.end, intersect);
}

bool lineOnPlane(Plane pln, Point3d a, Point3d b) {
	return pointOnPlane(pln, a) && pointOnPlane(pln, b);
}

bool lineOnPlane(Plane pln, Line3d ln) {
	return lineOnPlane(pln, ln.start, ln.end);
}


bool isBetween(long double val, long double l, long double r) {
	return (val >= l - eps) && (val <= r + eps);
}

bool pointInTriangle(Point3d a, Point3d b, Point3d c, Point3d p) {

	long double area = ((b - a) * (c - a)).len() / 2;

	long double area1 = ((p - b) * (p - c)).len() / 2;
	long double area2 = ((p - c) * (p - a)).len() / 2;
	long double area3 = ((p - b) * (p - a)).len() / 2;
	if (sign(area1 + area2 + area3 - area) == 0) {
		return true;
	}
	return false;
}




bool pointInRectangle(Point3d a, Point3d b, Point3d c, Point3d d, Point3d p) {
	
	long double area = ((b - a) * (d - a)).len();

	long double area1 = ((p - a) * (p - b)).len() / 2 ;
	long double area2 = ((p - b) * (p - c)).len() / 2;
	long double area3 = ((p - c) * (p - d)).len() / 2;
	long double area4 = ((p - d) * (p - a)).len() / 2;
	if (sign(area1 + area2 + area3 + area4 - area) == 0) {
		return true;
	}
	return false;
}


long double linePointDistance(Line3d line, Point3d point) {
	return (line.getVector() * (point - line.start)).len() / line.getVector().len();
}

bool pointInSphere(Point3d center, long double radius, Point3d point) {
	return isZero((point - center).len() - radius);
}


Point3d reflectVector(Point3d normal, Point3d line) {
	normal = normal * (1 / normal.len());
	return line - 2 * (line^normal) * normal;
}

Point3d refractVector(Point3d normal, Point3d line, float k) {
	Point3d shift = line + normal;
	shift = shift * k;
	Point3d result = -1 * normal + shift;
	return result;
}

Point3d pointOnPlaneProjection(Point3d point, Point3d planePoint, Point3d normal) {

	Point3d line = point - planePoint;

	if (sign(normal ^ line) < 0) {
		normal = -1 * normal;
	}

	normal = normal * (1 / normal.len());

	line = point - normal * (line^normal);

	return line;
}