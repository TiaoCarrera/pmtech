#ifndef _threads_h
#define _threads_h

#include "pen.h"

namespace pen
{
	struct thread;
	struct mutex;
	struct semaphore;
    
    enum thread_start_flags : u32
    {
        THREAD_START_DETACHED = 1,
        THREAD_START_JOINABLE = 2
    };

	//threads
	pen::thread*	threads_create( PEN_THREAD_ROUTINE( thread_func ), u32 stack_size, void* thread_params, thread_start_flags flags );
	void			threads_destroy( pen::thread* p_thread );

	//mutex
	pen::mutex*		threads_mutex_create( );
	void			threads_mutex_destroy( pen::mutex* p_mutex );

	void			threads_mutex_lock( pen::mutex* p_mutex );
	u32				threads_mutex_try_lock( pen::mutex* p_mutex );
	void			threads_mutex_unlock( pen::mutex* p_mutex );

	//semaphore
	pen::semaphore* threads_semaphore_create( u32 initial_count, u32 max_count );
	void			threads_semaphore_destroy( pen::semaphore* p_semaphore );

	bool			threads_semaphore_wait( pen::semaphore* p_semaphore );
	void			threads_semaphore_signal( pen::semaphore* p_semaphore, u32 count );

	//actions
	void			threads_sleep_ms( u32 milliseconds );
	void			threads_sleep_us( u32 microseconds );
}


#endif
