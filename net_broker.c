#include "net_broker.h"
#include "net_listener.h"
#include "net_connection.h"

struct net_broker_
{
	net_connection_t* connections;
	int connection_count;
	int thread_count;
	//callback
	//app_id
};

