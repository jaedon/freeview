#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

void * create_queue(unsigned int count, size_t element_size);
void delete_queue(void * q);

void * get_message(void * q, unsigned int t);
void send_message(void * q, void * m);
void * receive_message(void * q, unsigned int t);
void release_message(void * q, void * m);

#endif /* MESSAGE_QUEUE_H_ */
