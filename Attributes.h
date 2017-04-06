#pragma once
#include"Colour.h"

struct Attributes {

	Attributes() {};

	Attributes(Colour colour, bool isSource
		, long double intensity, double transparency,
		double reflection,double refraction) :colour(colour),
		lightSource(isSource), intensity(intensity), 
		transparency(transparency),reflection(reflection),refraction(refraction) {};

	Colour colour;
	bool lightSource = false;
	long double intensity = 0;
	double transparency = 0;
	double reflection = 0;
	double refraction = 1;
};