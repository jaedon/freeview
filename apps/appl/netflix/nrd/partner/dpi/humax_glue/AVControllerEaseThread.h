#ifndef AVCONTROLLER_EASETHREAD_H
#define AVCONTROLLER_EASETHREAD_H

/** @file AVControllerEaseThread.h - Defines a thread object used by the reference
  * implementations of the Audio Ease function.
  */

#include <nrdbase/Thread.h>

DECLARE_THREAD(DPI_AVCONTROLLER_EASE);

namespace netflix {
namespace device {
namespace esplayer {
	
class AVController;

/**
  * @class AVControllerEaseThread AVControllerEaseThread.h
  * @brief Wrapper of any thread that is running on the AVController
  */
class AVControllerEaseThread : private Thread
{
public:
	typedef void (AVController::*Task)();

	/**
	 * Constructer
	 *
	 * @param[in] avController the AVController instance
   	 * @param[in] task the task (function) to be executed
	 * @param[in] pConfig the ThreadConfig instance 	 
	 */
	AVControllerEaseThread(AVController& avController, Task task, ThreadConfig *pConfig);

	/**
	 * Destructor
	 */
	virtual ~AVControllerEaseThread();

private:
	virtual void Run();
	AVController& avController_;
	Task task_;
};

}	// namespace esplayer
}	// namespace device
}	// namespace netflix

#endif // AVCONTROLLER_EASETHREAD_H 
