#include <gtest/gtest.h>
#include "utils/memory.h"
#include "utils/struct_of_arrays.h"
#include "utils/lockfree_queue.h"
#include <string>

struct Point {
	Point(float _x, float _y)
		:x(_x), y(_y)
	{}
	float x;
	float y;
};

template <class T>
void Dump(T &p) {
	std::cout << p <<std::endl;
}

template<>
void Dump(struct Point& p)
{
	std::cout<<"{" << p.x << ", " << p.y <<"} " << std::endl;
}

TEST(SOA_TEST, for_each)
{
	using TestSOA = redtea::common::StructureOfArrays<std::string, Point>;

	TestSOA soa;
	// push test
	soa.push_back("point1", { 0, 0 });
	soa.push_back("point2", { 0.5, 0.5 });
	soa.push_back("point3", { 1, 1 });

	enum {
		NAME,
		POSITION
	};
	int i = 0;
	int size = soa.size();
	
	// foreach test
	std::cout << "foreach test:" << std::endl;
	soa.forEach([&i, size](auto p) {
		using T = typename std::decay<decltype(*p)>::type;
		for (int j = 0; j < size; j++)
		{
			Dump(p[j]);
		}
		i++;
	});

	//iterator test 
	std::cout << "iterator test:" << std::endl;
	for (TestSOA::iterator it = soa.begin(); it != soa.end(); it++)
	{
		auto p = *it;
		Dump(p.get<NAME>());
		Dump(p.get<POSITION>());
	}
}

TEST(LOCKFREEQUEUE_TEST, for_each)
{
	using namespace redtea::common;
	LockFreeQueue<char*> queue(64);

	char* msg = "Hello World;";
	queue.push(msg);
	char* result;
	queue.pop(result);
	std::cout << result << std::endl;

	char* msg1 = "first message;";
	queue.push(msg1);
	char* msg2 = "second message;";
	queue.push(msg2);
	EXPECT_EQ(queue.size(), 2);
	EXPECT_EQ(queue.capacity(), 64);

	//FIFO Test
	queue.pop(result);
	std::cout << result << std::endl;
	queue.pop(result);
	std::cout << result << std::endl; 
}