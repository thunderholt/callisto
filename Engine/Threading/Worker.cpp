#include "Threading/Worker.h"

void WorkerEntryPoint(void* arg)
{
	Worker* worker = (Worker*)arg;
	worker->Run();
}

Worker::Worker(IRunnable* runnable)
{
	IFactory* factory = GetFactory();

	this->runnable = runnable;
	this->isStarted = false;
	this->shouldTerminate = false;
	this->hasTerminated = false;
	this->workIsComplete = false;
	this->semaphore = factory->MakeSemaphore();
}

Worker::~Worker()
{
	SafeDeleteAndNull(this->semaphore);
}

void Worker::Start()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	if (threadManager->GetThreadingIsSupported())
	{
		threadManager->StartThread(WorkerEntryPoint, this);
		this->isStarted = true;
	}
}

void Worker::Stop()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();
	ILogger* logger = engine->GetLogger();

	if (threadManager->GetThreadingIsSupported() && this->isStarted)
	{
		logger->Write("Stopping worker...");
		this->shouldTerminate = true;
		this->semaphore->Signal();
		while (!this->hasTerminated);
	}
}

void Worker::DoWork()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->workIsComplete = false;

	if (threadManager->GetThreadingIsSupported())
	{
		this->semaphore->Signal();
	}
	else
	{
		this->runnable->Run();
	}
}

void Worker::WaitForWorkToComplete()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	if (threadManager->GetThreadingIsSupported())
	{
		while (!this->workIsComplete)
		{
			threadManager->Sleep(1);
		}
	}
}

void Worker::Run()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	while (true)
	{
		this->semaphore->Wait();

		if (this->shouldTerminate)
		{
			break;
		}
		else
		{
			this->runnable->Run();
			this->workIsComplete = true;
		}
	}

	this->hasTerminated = true;
	logger->Write("... worker stopped.");
}