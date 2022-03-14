

#include "Header.h"
#include "Global.h"
#include "MyThread.h"
#include "WorkerThread.h"
#include "ThreadHandler.h"


ThreadHandler::ThreadHandler()
{
}

ThreadHandler::~ThreadHandler()
{
	JoinThreads();
}

void ThreadHandler::AddThread( MyThread* mythread )
{
	mythread->InitThread();
	threads.emplace_back( mythread );
}

void ThreadHandler::CreateThreads()
{
	// WorkerThread 持失
	std::cout << "Creating " << maxWorkerThread << " WorkerThreads.." << std::endl;
	for ( int i = 0; i < maxWorkerThread; ++i )
		AddThread( new WorkerThread );
	std::cout << "WorkerThreads Created." << std::endl;

	// TimerThread 持失
	std::cout << "Creating TimerThread.." << std::endl;
	//AddThread( new TimerThread );
	std::cout << "TimerThread Created." << std::endl;
}

void ThreadHandler::JoinThreads()
{
	for ( auto& thread : threads )
		thread->JoinThread();
}
