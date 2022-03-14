

#pragma once
#include "TypeDef.h"


class MyThread;
class ThreadHandler
{
public:
	ThreadHandler();
	~ThreadHandler();

public:
	UxVoid CreateThreads();
	UxVoid JoinThreads();

private:
	UxVoid AddThread( MyThread* mythread );

private:
	std::vector<MyThread*> threads;
};

