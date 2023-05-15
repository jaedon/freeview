/*****************************************************************************
 * Message Queue implementation
 *****************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct list_s
{
   struct list_s * nextp;
   unsigned char data[0];
} list_t;

typedef struct mq_s
{
   /* semaphores to control signalling */
   sem_t rec_sem;
   sem_t send_sem;

   /* critical section to control access to the lists, just share, although you could use two */
   pthread_mutex_t cs;

   /* the two lists */
   list_t * rec_q;
   list_t * send_q;
} mq_t;


void * create_queue(unsigned int count, size_t element_size)
{
   /* allocate enough contiguous space for everything */
   mq_t * q = (mq_t *)malloc(sizeof(mq_t) + (count * (sizeof(list_t) + element_size)));

   if (q)
   {
      unsigned int i;
      list_t * p = (list_t *)((uintptr_t)q + sizeof(mq_t));
      /* create the semaphores */
      sem_init(&q->send_sem, 0, count);
      sem_init(&q->rec_sem, 0, 0);

      /* initialize the critical section */
      pthread_mutex_init(&(q->cs), NULL);

      /* one NULL queue and one with all the mesages in */
      q->send_q = p;
      q->rec_q = NULL;

      /* populate the queue, last one is NULL terminated */
      for (i = 0; i < (count - 1); i++, p = p->nextp)
         p->nextp = (list_t *)((uintptr_t)p + sizeof(list_t) + element_size);
      /* terminate the list */
      p->nextp = NULL;
   }

   /* will be NULL if failure */
   return (void *)q;
}

void delete_queue(void * q)
{
   mq_t * mq = (mq_t *)q;

   if (mq)
   {
      sem_destroy(&(mq->rec_sem));
      sem_destroy(&(mq->send_sem));

      pthread_mutex_destroy(&(mq->cs));

      free(mq);
   }
}

static bool sem_timedwait_reltimeout(sem_t * sem, unsigned int t)
{
   bool done = false;
   bool timed_out = false;

   if (t != 0xFFFFFFFF)
   {
      while (!done)
      {
         struct timespec ts;
         int s;
         long long ns;
         clock_gettime(CLOCK_REALTIME, &ts);
         ns = (long long)ts.tv_nsec;
         ns += (long long)t * 1000000;
         ts.tv_sec += (time_t)(ns / 1000000000);
         ts.tv_nsec = (long)(ns % 1000000000);
         while ((s = sem_timedwait(sem, &ts)) == -1 && errno == EINTR)
            continue;       /* Restart if interrupted by handler */
         if (s == -1)
         {
            if (errno == ETIMEDOUT)
               timed_out = true;
         }
         done = true;
      }
   }
   else
      sem_wait(sem);

   return !timed_out;
}



void * get_message(void * q, unsigned int t)
{
   mq_t * mq = (mq_t *)q;
   void * m = NULL;

   if (mq)
   {
      /* this will pull a semaphore from the list, if non available then it will block */
      if (sem_timedwait_reltimeout(&(mq->send_sem), t))
      {
         pthread_mutex_lock(&(mq->cs));

         /* we always guarentee a valid pointer back from here */
         m = mq->send_q->data;
         /* advance to next free item */
         mq->send_q = mq->send_q->nextp;

         pthread_mutex_unlock(&(mq->cs));
      }
   }

   return m;
}

void send_message(void * q, void * m)
{
   mq_t * mq = (mq_t *)q;

   if (mq)
   {
      pthread_mutex_lock(&(mq->cs));

      list_t * l = mq->rec_q;
      list_t * p = (list_t *)((uintptr_t)m - sizeof(list_t));

      /* configure the new block to point to the current head */
      p->nextp = NULL;
      /* place the block on the back of the receive queue */
      if (l)
      {
         while(l->nextp != NULL)
            l = l->nextp;
         l->nextp = p;
      }
      else
         mq->rec_q = p;

      pthread_mutex_unlock(&(mq->cs));

      sem_post(&(mq->rec_sem));
   }
}

void * receive_message(void * q, unsigned int t)
{
   mq_t * mq = (mq_t *)q;
   void * m = NULL;

   if (q)
   {
      /* this will pull a semaphore from the list, if non available then it will block */
      if (sem_timedwait_reltimeout(&(mq->rec_sem), t))
      {
         pthread_mutex_lock(&(mq->cs));

         /* we always guarentee a valid pointer back from here */
         m = mq->rec_q->data;
         /* move on to the next free block */
         mq->rec_q = mq->rec_q->nextp;

         pthread_mutex_unlock(&(mq->cs));
      }
   }

   return m;
}

void release_message(void * q, void * m)
{
   mq_t * mq = (mq_t *)q;

   if (mq)
   {
      pthread_mutex_lock(&(mq->cs));

      list_t * l = mq->send_q;
      list_t * p = (list_t *)((uintptr_t)m - sizeof(list_t));

      p->nextp = NULL;
      if (l)
      {
         while(l->nextp != NULL)
            l = l->nextp;
         l->nextp = p;
      }
      else
         mq->send_q = p;

      pthread_mutex_unlock(&(mq->cs));

      sem_post(&(mq->send_sem));
   }
}
