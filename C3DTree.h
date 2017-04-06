#pragma once
#include"IPrimitive.h"
#include"CBoxPrimitive.h"
#include<vector>
#include<algorithm>
#include"thread_pool.h"


static const int MAX_PRIMITIVES_IN_BOX = 25;

class C3DTree {
public:
	
	C3DTree() {};

	C3DTree(std::vector<IPrimitive*>&& primitives,std::vector<int>&& ids)
		:primitives(std::move(primitives)),id(std::move(ids)) {};

	void addPrimitives(std::vector<IPrimitive*> primitives) {
		this->primitives = primitives;
		id.clear();
		for (int i = 0; i < primitives.size(); ++i) {
			id.push_back(i);
		}
	}

	~C3DTree() {
		for (int i = 0; i < primitives.size(); ++i) {
			delete primitives[i];
		}
		delete left;
		delete right;
	}
	
	void createBoxes() {
		if (isCreatedBoxes) {
			return;
		}
		for (int i = 0; i < primitives.size(); ++i) {
			boxes.push_back(std::make_pair(i, primitives[i]->getBox()));
			
		}
		box = boxes[0].second;
		for (int i = 1; i < boxes.size(); ++i) {
			box.enlagreBox(boxes[i].second);
		}
		isCreatedBoxes = true;
	}

	void createTree() {
		thread_pool<void>* pool = new thread_pool<void>(7);
		createBoxes();
		nodesToSplit.store(1);
		pool->submit(std::bind(&C3DTree::splitBox, this, this, pool, &nodesToSplit));
		while (nodesToSplit.load() > 0) {
			std::this_thread::yield();
		}
		pool->shutdown();
		delete pool;
	}

	bool traceRay(Line3d line,int& result, Point3d& intersection) {
		if (!box.lineBoxInterseption(line)) {
			return false;
		}
		if (primitives.size() > 0) {
			Point3d closestPoint;
			int resultId;
			long double min = 10e10;
			for (int i = 0; i < primitives.size(); ++i) {
				if (primitives[i]->intersect(line, intersection)) {
					if (sign((intersection - line.start) ^ (line.getVector())) > 0) {
						if ((intersection - line.start).len() < min) {
							min = (intersection - line.start).len();
							closestPoint = intersection;
							resultId = id[i];
						}
					}
				}
			}
			if (min >= 10e8) {
				return false;
			}
			intersection = closestPoint;
			result = resultId;
			return true;
		}
		Point3d min;
		int minResult;
		if (left->traceRay(line, result, intersection)) {
			if (right->traceRay(line, minResult, min)) {
				if (sign((intersection - line.start).len() - (min - line.start).len()) > 0) {
					intersection = min;
					result = minResult;
				}
			}
			return true;
		}
		if (right->traceRay(line, result, intersection)) {
			return true;
		}
		return false;
	}

private:

	struct BoxLessMax
	{
		int axis;
		BoxLessMax(int axis) :axis(axis) {};
		bool operator()(std::pair<int, CBoxPrimitive> p1, std::pair<int, CBoxPrimitive> p2) const {
			return (p1.second.getMaxPoint(axis) < p2.second.getMaxPoint(axis));
		}
	};


	void splitBox(C3DTree* node,thread_pool<void>* pool, std::atomic<long long>* counter) {

		int splitAxis = -1;
		int sizeLeft = -1;
		if (node->boxes.size() < MAX_PRIMITIVES_IN_BOX) {
			counter->fetch_sub(1);
			return;
		}

		long double min = node->box.getMaxPoint(0) - node->box.getMinPoint(0);
		splitAxis = 0;
		sizeLeft = node->boxes.size() / 2;
		for (int axis = 0; axis < 3; ++axis) {
			if (min > node->box.getMaxPoint(axis) - node->box.getMinPoint(axis)) {
				min = node->box.getMaxPoint(axis) - node->box.getMinPoint(axis);
				splitAxis = axis;
			}
		}

		std::sort(node->boxes.begin(), node->boxes.end(), BoxLessMax(splitAxis));

		long double base = node->boxes[sizeLeft - 1].second.getMaxPoint(splitAxis);

		

		if (splitAxis != -1) {
			std::vector<IPrimitive*> leftPrimitives;
			std::vector<IPrimitive*> rightPrimitives;
			std::vector<int> leftId;
			std::vector<int> rightId;
			for (int i = 0; i < sizeLeft; ++i) {
				leftPrimitives.push_back(node->primitives[node->boxes[i].first]);
				leftId.push_back(node->id[node->boxes[i].first]);
			}
			for (int i = sizeLeft; i < node->boxes.size(); ++i) {
				rightPrimitives.push_back(node->primitives[node->boxes[i].first]);
				rightId.push_back(node->id[node->boxes[i].first]);
			}
			node->primitives.clear();
			node->boxes.clear();
			node->id.clear();
			node->left = new C3DTree(std::move(leftPrimitives),std::move(leftId));
			node->right = new C3DTree(std::move(rightPrimitives), std::move(rightId));
			node->left->createBoxes();
			node->right->createBoxes();
			pool->submit(std::bind(&C3DTree::splitBox, this, node->left, pool, counter));
			pool->submit(std::bind(&C3DTree::splitBox, this, node->right, pool, counter));
			counter->fetch_add(2);
		}
		counter->fetch_sub(1);

	}

	CBoxPrimitive box;
	C3DTree* left = nullptr;
	C3DTree* right = nullptr;
	bool isCreatedBoxes = false;

	std::vector<IPrimitive*> primitives;
	std::vector<int> id;
	std::vector<std::pair<int,CBoxPrimitive>> boxes;
	std::atomic<long long> nodesToSplit;

};


