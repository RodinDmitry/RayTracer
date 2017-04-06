#pragma once
#include"Canvas.h"
#include<vector>
#include"geometry3d.h"
#include"IPrimitive.h"
#include"C3DTree.h"
#include"thread_pool.h"
#include<algorithm>
#include<iostream>
#include<thread>
#include<queue>
#include<mutex>

const int RECURSIVE_DEPTH_MAX = 5;

class Renderer {
public:

	Renderer(int width, int height) :width(width),height(height)
		,canvas(width, height), camera(0,0,0) {
		canvas.buildCanvas();
	};

	void addPrimitive(IPrimitive* primitive) {
		primitives.push_back(primitive);
		if (primitive->isLightSource()) {
			lightSources.push_back(primitive);
		}
	}

	void setCamera(Point3d newCamera) {
		camera = newCamera;
	}

	Canvas renderScene() {

		tree.addPrimitives(primitives);
		tree.createTree();
		
		std::vector<std::thread> threads;

		for (int i = 0; i < threadsNumber; ++i) {
			threads.push_back(std::thread(&Renderer::buildPhantomSources, this, i));
		}
		for (int i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}

		threads.clear();


		for (int i = 0; i < threadsNumber; ++i) {
			threads.push_back(std::thread(&Renderer::renderCanvasPart,this,i));
		}
		for (int i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}
		return canvas;
	}
private:

	void renderCanvasPart(int id) {
		int beginI = (width * id) / threadsNumber;
		int endI = (width * (id+1)) / threadsNumber;
		int beginJ = 0;
		int endJ = height;
		
		for (int i = beginI; i < endI; ++i) {
			for (int j = beginJ; j < endJ; ++j) {
				canvas.setColour(i, j, calculateColour(i, j));
			}
		}
	}

	Colour calculateColour(int i, int j) {
		Line3d line{ camera,canvas.getPoint(i,j) };
		Point3d resultPoint;
		int result;
		Colour mainColour;
		if (getClosestPoint(line, result, resultPoint)) {
			mainColour = calculatePointColour(line, resultPoint, result, 0);
		}
		return mainColour;
	}


	Colour calculatePointColour(Line3d line, Point3d point, 
		int primitiveId, int recursiveDepth) {

		//std::unique_lock<std::mutex> lock(addMutex);

		if (recursiveDepth > RECURSIVE_DEPTH_MAX) {
			return Colour();
		}
		Colour mainColour = getMainColour(line,point, primitiveId);
		Colour reflectiveColour = getReflectiveColour(line, point, primitiveId, recursiveDepth);
		Colour refractiveColour = getRefractiveColour(line, point, primitiveId, recursiveDepth);
		float reflection = primitives[primitiveId]->getReflection(line);
		float transparency = primitives[primitiveId]->getTransparency(line);
		return mixColours(mainColour, reflectiveColour, refractiveColour,
			1 - reflection - transparency, reflection, transparency);
	}

	Colour mixColours(Colour colour1, Colour colour2, Colour colour3, 
		float fraction1, float fraction2, float fraction3) {
		colour1.fraction(fraction1);
		colour2.fraction(fraction2);
		colour3.fraction(fraction3);
		colour1.addColour(colour2);
		colour1.addColour(colour3);
		return colour1;
	}

	Colour getMainColour(Line3d line, Point3d startPoint, int primitiveId) {
		Colour mainColour = primitives[primitiveId]->getColour(line, startPoint);
		lightIntencity illumination = calculateIllumination(startPoint,primitiveId);
		mainColour.fraction(illumination.redIntencity,
			illumination.greenIntencity,
			illumination.blueIntencity);
		return mainColour;
	}

	Colour getReflectiveColour(Line3d line, Point3d point,int primitiveId,int recursiveDepth) {
		int result;
		Point3d resultPoint;
		if (reflectLine(line, point, primitiveId, result, resultPoint)) {
			return calculatePointColour(Line3d{ point,resultPoint }, resultPoint, result, recursiveDepth + 1);
		}
		return Colour();
	}

	Colour getRefractiveColour(Line3d line, Point3d point, int primitiveId, int recursiveDepth) {
		int result;
		Point3d resultPoint;
		float k = primitives[primitiveId]->getRefraction(line);
		if (refractLine(line, point, primitiveId, k, result, resultPoint)) {
			return calculatePointColour(Line3d{ point,resultPoint }, resultPoint, result, recursiveDepth + 1);
		}
		return Colour();
	}

	bool reflectLine(Line3d line, Point3d startPoint, int primitiveId,int& result, Point3d& resultPoint) {
		Point3d direction = line.getVector();
		Point3d normal = primitives[primitiveId]->getNormalVector(startPoint);
		if (sign(direction ^ normal) > 0) {
			normal = normal * -1;
		}
		Point3d newLine = reflectVector(normal, direction);
		return getClosestPoint(Line3d{startPoint,newLine}, result, resultPoint);

	}

	bool refractLine(Line3d line, Point3d startPoint, 
		int primitiveId, float k, 
		int& result, Point3d& resultPoint) {
		Point3d direction = line.getVector();
		Point3d normal = primitives[primitiveId]->getNormalVector(startPoint);
		if (sign(direction ^ normal) > 0) {
			normal = normal * -1;
		}
		Point3d newLine = refractVector(normal, direction, k);
		return getClosestPoint(Line3d{ startPoint,newLine }, result, resultPoint);
	}

	bool getClosestPoint(Line3d line,int& result, Point3d& resultPoint) {
		return tree.traceRay(line, result, resultPoint);
	}

	lightIntencity calculateIllumination(Point3d start, int primitiveId) {
		return calculateNormalIllumination(start, primitiveId) + calculatePhantomIllumination(start, primitiveId);
	}

	lightIntencity calculateNormalIllumination(Point3d start, int primitiveId) {
		lightIntencity totalIllumination;
		for (int i = 0; i < lightSources.size(); ++i) {
			Point3d resultPoint;
			int result;
			Line3d line{ start, lightSources[i]->getCenter() };
			Point3d normal = primitives[primitiveId]->getNormalVector(start);
			long double cos = fabsl(normal ^ line.getVector());
			cos /= normal.len();
			cos /= line.getVector().len();
			if (getClosestPoint(line, result, resultPoint)) {
				if (sign((resultPoint - start).len() - line.getVector().len()) >= 0) {
					Colour sourceColour = lightSources[i]->getColour(Line3d{ start,resultPoint },resultPoint);
					long double intencity = lightSources[i]->getIntencity(Line3d{start,resultPoint});
					totalIllumination.redIntencity += (long double)sourceColour.R 
						* intencity * cos / ((long double)255 * line.getVector().len2());
					totalIllumination.greenIntencity += (long double)sourceColour.G 
						* intencity * cos / ((long double)255 * line.getVector().len2());
					totalIllumination.blueIntencity += (long double)sourceColour.B 
						* intencity * cos / ((long double)255 * line.getVector().len2());
				}
			}
		}
		return totalIllumination;
	}

	lightIntencity calculatePhantomIllumination(Point3d start, int primitiveId) {
		lightIntencity totalIllumination;
		for (int i = 0; i < phantomSources.size(); ++i) {
			Point3d resultPoint;
			int result;
			Line3d line{ start, phantomSources[i].point };
			Point3d normal = primitives[primitiveId]->getNormalVector(start);
			long double cos = fabsl(normal ^ line.getVector());
			cos /= normal.len();
			cos /= line.getVector().len();
			if (getClosestPoint(line, result, resultPoint)) {
				if (result == phantomSources[i].primitiveId 
					&& ((primitives[phantomSources[i].primitiveId]->getReflection(line) > 0))
					|| (primitives[phantomSources[i].primitiveId]->getRefraction(line))) {
					Line3d newLine{ resultPoint,lightSources[phantomSources[i].sourceId]->getCenter() };
					start = resultPoint;
					if (getClosestPoint(newLine, result, resultPoint)) {
						if (sign((resultPoint - start).len() - newLine.getVector().len()) >= 0) {
							Colour sourceColour = 
								lightSources[phantomSources[i].sourceId]->getColour(newLine,resultPoint);
							Colour objColour = 
								primitives[phantomSources[i].primitiveId]->getColour(newLine,resultPoint);
							long double intencity = 
								lightSources[phantomSources[i].sourceId]->getIntencity(newLine);
							totalIllumination.redIntencity += (long double)std::min(sourceColour.R,objColour.R)
								* intencity * cos / ((long double)255 * line.getVector().len2());
							totalIllumination.greenIntencity += (long double)std::min(sourceColour.G, objColour.G)
								* intencity * cos / ((long double)255 * line.getVector().len2());
							totalIllumination.blueIntencity += (long double)std::min(sourceColour.B, objColour.B)
								* intencity * cos / ((long double)255 * line.getVector().len2());
						}
					}
				}
			}
		}
		return totalIllumination;
	}

	void buildPhantomSources(int id) {
		int from = primitives.size() * id / threadsNumber;
		int to = primitives.size() * (id + 1) / threadsNumber;
		for (int i = from; i < to; ++i) {
			for (int j = 0; j < lightSources.size(); ++j) {
				Line3d line{ lightSources[j]->getCenter(),primitives[i]->getCenter() };
				if (primitives[i]->getReflection(line) > 0) {
					addPhantomSource(i, j,
						primitives[i]->getPhantomReflectionSource(lightSources[j]->getCenter()));
				}
			}
			for (int j = 0; j < lightSources.size(); ++j) {
				Line3d line{ lightSources[j]->getCenter(),primitives[i]->getCenter() };
				if (primitives[i]->getTransparency(line) > 0) {
					addPhantomSource(i, j,
						primitives[i]->getPhantomRefractionSource(lightSources[j]->getCenter()));
				}
			}
		}
	}

	void addPhantomSource(int primitiveId,int sourceId,std::vector<Point3d> points) {
		std::unique_lock<std::mutex> lock(addMutex);
		for (auto point : points) {
			phantomSources.push_back(phantomSource{ primitiveId,sourceId,point });
		}
	}

	int threadsNumber = 5;
	C3DTree tree;
	int width;
	int height;
	Canvas canvas;
	std::vector<IPrimitive*> primitives;
	std::vector<IPrimitive*> lightSources;
	std::mutex addMutex;
	
	struct phantomSource
	{
		int primitiveId;
		int sourceId;
		Point3d point;
	};

	std::vector<phantomSource> phantomSources;

	Point3d camera;

};