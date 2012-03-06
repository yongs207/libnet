#include "../net_connection.h"

void connection_test()
{
  int i=0;
  net_connection_t* alloc_conn3[3],*alloc_conn4[4];
  net_conn_manager_t* conn_manager = nconnection_create(16,nlog_create());
  nconnections_dump(conn_manager);
  printf("申请3个conn\n");
  for(i=0;i<3;i++)//申请3个conn
    alloc_conn3[i]=nconnection_get(conn_manager);
  nconnections_dump(conn_manager);
  printf("释放2个\n");
  //释放2个
  for(i=0;i<2;i++)
    nconnection_free(conn_manager,alloc_conn3[i]);
  nconnections_dump(conn_manager);
  //再申请4
  printf("申请4个conn\n");
  for(i=0;i<4;i++)//申请3个conn
    alloc_conn4[i]=nconnection_get(conn_manager);
  nconnections_dump(conn_manager);
  //释放5个
  printf("释放5个\n");
  nconnection_free(conn_manager,alloc_conn3[2]);
  for(i=0;i<4;i++)
    nconnection_free(conn_manager,alloc_conn4[i]);
  nconnections_dump(conn_manager);
}

//int main()
//{
//  connection_test();
//  system("pause");
//  return 0;
//}