#include "AVControllerEaseThread.h"

using namespace netflix::device::esplayer;

DEFINE_THREAD(DPI_AVCONTROLLER_EASE, 70, 0);

AVControllerEaseThread::AVControllerEaseThread(AVController& avController, Task task,
												ThreadConfig *config)
	: Thread(config),
	  avController_(avController),
	  task_(task)
{
	Start();
	WaitRunning();
}

AVControllerEaseThread::~AVControllerEaseThread()
{
	Wait();	// don't let the thread outlive the object
}

void AVControllerEaseThread::Run()
{
	(avController_.*task_)();
}
