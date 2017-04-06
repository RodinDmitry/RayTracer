#pragma once
#include"geometry3d.h"
#include"IPrimitive.h"
#include<vector>


class Canvas {
public:
	Canvas(int w, int h) :width(w),heigth(h) {}

	void buildCanvas() {
		leftUp = { 0,100,0 };
		leftDown = { 0,0,0 };
		rightDown = { 100,0,0 };
		
		canvas.resize(width);
		colours.resize(width);
		for (int i = 0; i < width; ++i) {
			canvas[i].resize(heigth);
			colours[i].resize(heigth);
		}
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < heigth; ++j) {
				canvas[i][j] = ((long double)heigth - (long double)j) / (long double)heigth * (leftUp - leftDown)
					+ ((long double)i) / (long double)width * (rightDown - leftDown);
			}
		}
	}

	Point3d& getPoint(int i, int j) {
		return canvas[i][j];
	}

	Colour& getColour(int i, int j) {
		return colours[i][j];
	}

	void setColour(int i, int j, Colour colour) {
		colours[i][j] = colour;
	}

	int getWidth() {
		return width;
	}

	int getHeigth() {
		return heigth;
	}

	Canvas SSAA4(Canvas& unsmoothed) {
		Canvas result(width / 2, heigth / 2);
		for (int i = 0; i < width; i += 2) {
			for (int j = 0; j < heigth; j += 2) {
				Colour resultColour;
				unsmoothed.getColour(i, j).fraction(0.25);
				resultColour.addColour(unsmoothed.getColour(i, j));
				unsmoothed.getColour(i+1, j).fraction(0.25);
				resultColour.addColour(unsmoothed.getColour(i+1, j));
				unsmoothed.getColour(i, j+1).fraction(0.25);
				resultColour.addColour(unsmoothed.getColour(i, j+1));
				unsmoothed.getColour(i+1, j+1).fraction(0.25);
				resultColour.addColour(unsmoothed.getColour(i+1, j+1));
			}
		}
		return result;
	}

private:
	int width;
	int heigth;
	std::vector<std::vector<Point3d>> canvas;
	std::vector<std::vector<Colour>> colours;
	Point3d leftUp;
	Point3d leftDown;
	Point3d rightDown;

};