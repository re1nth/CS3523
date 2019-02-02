#include <bits/stdc++.h>

using namespace std;

int main()
{
	int num,cnt=0;

	FILE *fptr=NULL;

	fptr=fopen("inp-params.txt","r");

	fscanf(fptr,"%d",&num);

	int process_id,period,num_of_times;

	float process_time;

	while(cnt<num)
	{
		fscanf(fptr,"%d %f %d %d",&process_id,&process_time,&period,&num_of_times);

		cnt++;
	}
	
	fclose(fptr);

	return 0;
}