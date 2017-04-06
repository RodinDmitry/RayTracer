#pragma once
#include<algorithm>

struct Colour {
public:


	Colour() {};

	Colour(int r, int g, int b) :R(r), G(g), B(b), A(255) {};

	void normalizeColour() {
		if (R > 255) {
			R = 255;
		}
		if (G > 255) {
			G = 255;
		}
		if (B > 255) {
			B = 255;
		}
		A = 255;
	}

	void addColour(float red, float green,float blue) {
		R += round(red);
		G += round(green);
		B += round(blue);
		normalizeColour();
	}

	void addColour(Colour colour) {
		R += colour.R;
		B += colour.B;
		G += colour.G;
		normalizeColour();
	}

	void fraction(float redAlpha, float greenAlpha, float blueAlpha) {
		R = round((float)R * redAlpha);
		G = round((float)G * greenAlpha);
		B = round((float)B * blueAlpha);
		normalizeColour();
	}

	void fraction(float alpha) {
		fraction(alpha, alpha, alpha);
	}

	int R = 0;
	int G = 0;
	int B = 0;
	int A = 255;

};