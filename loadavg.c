
#include "loadavg.h"

int Load(AVGList * avginfo)
{
	printf("%s\n", __func__);
	
	FILE * file = fopen(PROCLOADFILE, "r");
	if (file == NULL) 
	{
		perror("Cannot open " PROCLOADFILE);
		return -1;
	}

	fscanf(file,
	       "%lf %f %f ",
	       &avginfo->avg_now,
	       &avginfo->avg_five,
	       &avginfo->avg_fifteen
	       //&avginfo->proccess
	       );

	printf("cpu %lf %f %f ",
	       avginfo->avg_now,
	       avginfo->avg_five,
	       avginfo->avg_fifteen
	       //avginfo->proccess
	       );

	fclose(file); 
	return (0);
}
