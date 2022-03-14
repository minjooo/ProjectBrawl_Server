

#pragma once
#include <thread>
#include "TypeDef.h"

class MyThread
{
public:
	virtual ~MyThread() {};

public:
	virtual UxVoid InitThread() = 0;
	virtual UxVoid ProcThread() = 0;
	virtual UxVoid JoinThread() = 0;

protected:
	std::thread mythread;
};

