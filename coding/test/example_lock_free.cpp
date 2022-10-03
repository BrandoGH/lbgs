#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>

namespace
{

// a simple global linked list:
struct Node { int value; Node* next; };
std::atomic<Node*> list_head(nullptr);

void append(int val)
{     // append an element to the list
	Node* oldHead = list_head;
	Node* newNode = new Node{ val,oldHead };

	// what follows is equivalent to: list_head = newNode, but in a thread-safe way:
	while (!list_head.compare_exchange_weak(oldHead, newNode))
		newNode->next = oldHead;
}

TEST(Lock_Free_Stack, test)
{
	//std::vector<std::thread> threads;
	//for (int i = 0; i < 50; ++i) threads.push_back(std::thread(append, i));
	//for (auto& th : threads) th.join();

	//// print contents:
	//for (Node* it = list_head; it != nullptr; it = it->next)
	//	printf_color(PRINTF_YELLOW, " %d", it->value);
	//std::cout << '\n';

	//// cleanup:
	//Node* it; while (it = list_head) { list_head = it->next; delete it; }
}
}