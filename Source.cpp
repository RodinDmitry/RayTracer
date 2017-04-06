#include<SFML\Graphics.hpp>

#include"Renderer.h"
#include"CTrianglePrimitive.h"
#include"CRectanglePrimitive.h"
#include"CPointPrimitive.h"
#include"CSpherePrimitive.h"
#include"CMultiplePrimitive.h"
#include"CImg.h"



using namespace cimg_library;

const int RESOLUTION_WIDTH = 1380;
const int RESOLUTION_HEIGHT = 788;

sf::Color getColor(Colour colour) {
	return sf::Color(colour.R, colour.G, colour.B, colour.A);
}

sf::VertexArray buildImage(Canvas& canvas) {
	sf::VertexArray image(sf::Points, canvas.getWidth() * canvas.getHeigth());
	for (int i = 0; i < canvas.getWidth(); ++i) {
		for (int j = 0; j < canvas.getHeigth(); ++j) {
			image[i*canvas.getHeigth() + j].position = sf::Vector2f(i, j);
			image[i*canvas.getHeigth() + j].color = getColor(canvas.getColour(i, j));
		}
	}
	return image;
}



void showImage(sf::VertexArray image) {
	sf::RenderWindow window(sf::VideoMode(RESOLUTION_WIDTH, RESOLUTION_HEIGHT), "Image");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(image);
		window.display();
	}
}


void print_num(int i, int j)
{
	std::cout << i << ' ' << j << '\n';
}


/*Attributes(Colour colour, bool isSource
, long double intensity, double transparency,
double reflection, double refraction) :colour(colour),
lightSource(isSource), intensity(intensity),
transparency(transparency), reflection(reflection), refraction(refraction) {};*/


Colour white{ 255,255,255 };
Colour green{ 0 ,255 ,0 };
Colour red{ 255,0,0 };
Colour blue{ 0,0,255 };

Attributes redA{ red,false,0,0,0,1 };
Attributes greenA{ green,false,0,0,0,1 };
Attributes blueA{ blue,false,0,0,0,1 };
Attributes whiteA{ white,false,0,0,0,1 };
Attributes greenM{ green,false,0,0,0.8,1 };
Attributes redM{ red,false,0,0,0.8,1 };

Attributes rainbow{ "C:\\Users\\rodin\\Documents\\Visual Studio 2015\\Projects\\RayTracing\\ConsoleApplication3\\cs3n2c16.bmp",false,0,0,0,1 };
Attributes evilSun{ "C:\\Users\\rodin\\Documents\\Visual Studio 2015\\Projects\\RayTracing\\ConsoleApplication3\\image.bmp",false,0,0,0,1 };

void createManyTriangles(Renderer& rend) {
	long double shift = 5;
	for (long double i = 0; i < 100; i = i + shift) {
		for (long double j = 0; j < 100; j = j + shift) {
			CTrianglePrimitive* triangle =
				new CTrianglePrimitive{ Point3d{ i,j,0 },Point3d{ i + shift,j,0 },Point3d{ i,j + shift,0 },whiteA,whiteA };
			rend.addPrimitive(triangle);
		}
	}
}

int main()
{


	Attributes source{ white,true,500,0,0,0 };
	Attributes sourceGreen{ green,true,500,0,0,0 };

	Point3d a{ 0,0,0 };
	Point3d b{ 50,50,0 };
	Point3d c{ 0,71,0 };
	Point3d d{ 100,100,-1 };
	Point3d s{ 50,50,-20 };


	CTrianglePrimitive* redT = new CTrianglePrimitive{ a,b,c,redA,greenA };
	CRectanglePrimitive* blueR = new CRectanglePrimitive{ { 0,0,5 },{ 100,0,5 },{ 100,100,5 },{ 0,100,5 },evilSun,evilSun };
	CRectanglePrimitive* greem1 = new CRectanglePrimitive{ { 25,0,-10 },{ 25,0,10 },{ 25,100,10 },{ 25,100,-10 },greenM,greenM };
	CRectanglePrimitive* greem2 = new CRectanglePrimitive{ { 75,0,-10 },{ 75,0,10 },{ 75,100,10 },{ 75,100,-10 },redM,redM };

	CSpherePrimitive*  sphere = new CSpherePrimitive{ { 40,40,-5 },5,greenM };

	CSpherePrimitive*  sphere1 = new CSpherePrimitive{ { 50,20,-5 },5,redA };
	CSpherePrimitive*  sphere2 = new CSpherePrimitive{ { 47,22,-5 },5,blueA };
	std::vector<bool> signum;
	signum.push_back(1);
	signum.push_back(1);
	CMultiplePrimitive* mult = new CMultiplePrimitive{ { sphere1,sphere2 },signum };


	CPointPrimitive* light = new CPointPrimitive{ s,source };
	CPointPrimitive* lightG = new CPointPrimitive{ { 0,0,-5 },sourceGreen };



	Renderer rend(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);

	//createManyTriangles(rend);

	//rend.addPrimitive(redT);
	rend.addPrimitive(light);
	//rend.addPrimitive(sphere);
	//rend.addPrimitive(mult);
	rend.addPrimitive(blueR);
	//rend.addPrimitive(lightG);
	//rend.addPrimitive(greem1);
	//rend.addPrimitive(greem2);

	rend.setCamera(Point3d{ 50,50,-1000 });

	Canvas canvas = rend.renderScene();

	sf::VertexArray dots = buildImage(canvas);
	showImage(dots);
	
	CImg<unsigned char> 
		img("C:\\Users\\rodin\\Documents\\Visual Studio 2015\\Projects\\RayTracing\\ConsoleApplication3\\cs3n2c16.bmp");
	
	return 0;
}