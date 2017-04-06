#pragma once


#include<future>
#include<mutex>
#include<thread>
#include<vector>
#include<exception>
#include<deque>


template <class Value, class Container = std::deque<Value>>
class thread_safe_queue {

public:

	explicit thread_safe_queue(size_t _capacity) {
		capacity = _capacity;
		finalFlag = false;
	}

	void enqueue(Value v) {

		std::unique_lock<std::mutex> lock(acsessMutex);
		if (finalFlag) {
			throw std::exception();
		}
		/*while (container.size() == capacity) {
			reader.wait(lock);
		}*/

		container.push_back(std::move(v));
		writer.notify_one();

	}

	void pop(Value& v) {

		std::unique_lock<std::mutex> lock(acsessMutex);
		while (container.empty()) {
			if (finalFlag) {
				throw std::exception();
			}
			writer.wait(lock);
		}

		v = std::move(container.front());
		container.pop_front();
		reader.notify_one();

	}

	void shutdown() {
		finalFlag = true;
		reader.notify_all();
		writer.notify_all();
	}

private:

	Container container;
	size_t capacity;
	bool finalFlag;
	std::mutex acsessMutex;
	std::condition_variable reader;
	std::condition_variable writer;

};




template <class Value>
class thread_pool {
public:

	explicit thread_pool(size_t num_workers) : tasks(num_workers) {
		
		for (size_t i = 0; i < num_workers; ++i) {
			workers.push_back(std::thread(&thread_pool::loop, this));
		}
	}

	std::future<Value> submit(std::function<Value()> function) {
		std::packaged_task<Value()> task(function);
		std::future<Value> result(task.get_future());
		tasks.enqueue(std::move(task));
		return result;
	}

	void shutdown() {
		tasks.shutdown();
		for (size_t i = 0; i < workers.size(); ++i) {
			workers[i].join();
		}
	}

private:

	thread_safe_queue<std::packaged_task<Value()> > tasks;
	std::vector<std::thread > workers;

	void loop() {
		while (true) {
			std::packaged_task<Value()> task;
			try {
				tasks.pop(task);
				task();
			}
			catch (const std::exception& e)
			{
				return;
			}
		}
	}
};

