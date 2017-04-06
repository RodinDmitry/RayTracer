#pragma once


struct lightIntencity {
	long double redIntencity = 0;
	long double greenIntencity = 0;
	long double blueIntencity = 0;

	lightIntencity() {};

	lightIntencity(long double red, long double green, long double blue) :
		redIntencity(red), greenIntencity(green), blueIntencity(blue) {};

};

lightIntencity operator+(lightIntencity left, lightIntencity right) {
	return lightIntencity(left.redIntencity + right.redIntencity,
		left.greenIntencity + right.greenIntencity,
		left.blueIntencity + right.blueIntencity);
}