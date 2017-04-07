#pragma once
#include"Colour.h"
#include<string>
#include"CImg.h"

using namespace cimg_library;


struct Attributes {

	Attributes() {};

	~Attributes() {
		if (initTexture) {
			delete image;
		}
	}

	Attributes(Colour colour, bool isSource
		, long double intensity, double transparency,
		double reflection,double refraction) :colour(colour),
		lightSource(isSource), intensity(intensity), 
		transparency(transparency),reflection(reflection),refraction(refraction) {};


	Attributes(std::string texName, bool isSource
		, long double intensity, double transparency,
		double reflection, double refraction) :textureName(texName),hasTexture(true),
		lightSource(isSource), intensity(intensity),
		transparency(transparency), reflection(reflection), refraction(refraction) {};

	void loadTexture() {
		if (!initTexture && hasTexture) {
			image = new CImg<unsigned char>(textureName.c_str());
			initTexture = true;
		}
	}

	Colour getPoint(long double x,long double y) {
		loadTexture();
		CImg<unsigned char>& im = *image;
		int width = image->width();
		int height = image->height();
		long double xMN = 0.2;
		long double yMN = 0.2;
		int red = (int)im((int)round(x * xMN) % width, (int)round(y * yMN) % height, 0, 0);
		int green = (int)im((int)round(x * xMN) % width, (int)round(y * yMN) % height, 0, 1);
		int blue = (int)im((int)round(x * xMN) % width, (int)round(y * yMN) % height, 0, 2);
		Colour result{ red,green,blue };
		result.normalizeColour();
		return result;
	}

	Colour colour;
	bool lightSource = false;
	long double intensity = 0;
	double transparency = 0;
	double reflection = 0;
	double refraction = 1;
	bool hasTexture = false;
	bool initTexture = false;
	CImg<unsigned char>* image;
	std::string textureName;
};