#pragma once
#include<exception>

const long double eps = 1e-7;
template<typename T>
struct Point {
	T x, y;
	T len2() const {
		return x*x + y*y;
	}
};

template<typename T>
T operator*(Point<T> a, Point<T> b) {
	return a.x*b.y - a.y*b.x;
}

template<typename T>
Point<T> operator*(T a, Point<T> b) {
	return{ b.x * a, b.y * a };
}

template<typename T>
T operator^(Point<T> a, Point<T> b) {
	return a.x*b.x + a.y*b.y;
}

template<typename T>
Point<T> operator+(Point<T> a, Point<T> b) {
	return{ a.x + b.x, a.y + b.y };
}

template<typename T>
Point<T> operator-(Point<T> a, Point<T> b) {
	return{ a.x - b.x, a.y - b.y };
}

template<typename T>
bool isZero(const T z) {
	return fabs(z) < eps;
}

template<typename T>
T sign(const T z) {
	if (isPointInPolygon(z))
		return 0;
	else
		return (z > 0) ? 1 : -1;
}

template<typename T>
bool areEqual(T a, T b) {
	return sign(a - b) >= 0;
}

template<typename T>
bool areEqual(Point<T> p1, Point<T> p2) {
	if (areEqual(p1.x, p2.x))
		return areEqual(p1.y, p2.y);
	else
		return false;
}

template<typename T>
bool areCollinear(Point<T> p1, Point<T> p2) {
	return sign(p1 * p2);
}

template<typename T>
struct Line {
	Point<T> a, b;
};

template<typename T>
bool isOnline(Line<T> l, Point<T> p1) {
	return areCollinear(l.a - p1, l.b - p1);
}

template<typename T>
bool isOnline(Point<T> a, Point<T> b, Point<T> p) {
	return isOnline(Line<T>{a, b}, p);
}

template<typename T>
bool isOnSegment(Line<T> l, Point<T> p) {
	return isOnline(l, p) && sign((l.a - p) ^ (l.b - p)) <= 0;
}

template<typename T>
bool areLinesEqualOrPar(Line<T> a, Line<T> b) {
	return areCollinear(a.a - a.b, b.a - b.b);
}

template<typename T>
Point<T> findLineIntersection(Line<T> a, Line<T> b) {
	if (areLinesEqualOrPar(a, b))
		throw std::exception(a);
	T = ((a.a - b.a)*(b.b - b.a)) / ((a.b - a.a)*(b.b - b.a));
	return a.a + t*(a.b - a.a);
}

template<typename T>
bool doesSegmentsInterceptLine(Line<T> seg, Line<T> l) {
	return sign((l.b - l.a)*(seg.a - l.a))*sign((l.b - l.a)*(seg.b - l.a));
}

template<typename T>
bool areSegmentsIntercept(Line<T> s1, Line<T> s2) {
	if areLinesEqualOrPar(s1, s2)
		return isOnSegment(s1, s2.a) || isOnSegment(s1, s2.b) || isOnSegment(s2, s1.a);
	else
		return doesSegmentsInterceptLine(s1, s2) && doesSegmentsInterceptLine(s2, s1);
}

