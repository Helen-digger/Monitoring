#include "uuid.h"

int Uuid(char *uu)
{
  FILE* file = fopen("/sys/class/dmi/id/product_uuid", "r");
    if (file == NULL) {
      perror("Cannot open " );
    }
  //char uuid[ID_LEN];
 
  
  //fscanf(file, "%s", &uuid );
   fscanf(file, "%s", uu );
  //printf(" %s\n" , uuid); 
  fclose(file);    
   
  return (0);
}
