
#ifndef net_signaler_h
#define net_signaler_h


void* net_signal_create();
void net_signal_destory(void* signal);
void net_signal_send(void* signal);
void net_signal_recv(void* signal);
int net_signal_wait (void* signal,int timeout_);

#endif