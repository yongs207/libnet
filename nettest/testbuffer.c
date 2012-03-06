#include "../net_buffer.h"

void buffer_test()
{
  net_buffer_t* buff_ptr = nbuf_create(8);
  char* str = "hello";
  char* str2 = "hello world @wind ";
  strcpy(nbuf_tailer(buff_ptr),str);
  nbuf_product(buff_ptr,strlen(str));
  nbuf_dump(buff_ptr);
  nbuf_consume(buff_ptr,2);
  if (nbuf_space_size(buff_ptr)>strlen(str))
    strcpy(nbuf_tailer(buff_ptr),str);
  
  nbuf_dump(buff_ptr);

  nbuf_reset(buff_ptr,32);
  nbuf_dump(buff_ptr);
  strcpy(nbuf_tailer(buff_ptr),str2);
  nbuf_product(buff_ptr,strlen(str2));
  nbuf_dump(buff_ptr);
  free(nbuf_data(buff_ptr));
  nbuf_clear(buff_ptr);

  strcpy(nbuf_tailer(buff_ptr),str);
  nbuf_product(buff_ptr,strlen(str));
  nbuf_dump(buff_ptr);
}



//int main()
//{
//  buffer_test();
//  system("pause");
//  return 0;
//}