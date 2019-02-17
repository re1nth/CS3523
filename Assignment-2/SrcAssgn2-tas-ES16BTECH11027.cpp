#include <iostream>
#include <atomic>
#include <thread>
#include <utility>
#include <unistd.h>
#include <sys/time.h>
#include <vector>

using namespace std;

int NUM_THREADS,NUM_TIMES;

double t1,t2;

atomic<bool> foo(false);

struct timeval tp[100];

double waiting_time[100];

double reqEnterTime[100][100];

double actEnterTime[100][100];

double exitTime[100][100];

double worst_case;

double start_time;

struct capsule
{
	double xtime;
	int thread_id;
	int num;
	int type;
};

vector<capsule>vec;

bool mycomp(capsule s1,capsule s2)
{
	return(s1.xtime<s2.xtime);
}

void testCS(int id)
{
	for(int i=0;i<10;i++)
	{
		gettimeofday(&tp[id],NULL);

		reqEnterTime[id][i]=(double)tp[id].tv_sec+(double)tp[id].tv_usec*(1e-6)-start_time;

		// entry section

		while(atomic_exchange_explicit(&foo,true,memory_order_acquire)){}
		
		gettimeofday(&tp[id],NULL);

		actEnterTime[id][i]=(double)tp[id].tv_sec+(double)tp[id].tv_usec*(1e-6)-start_time;

		waiting_time[id]+=(actEnterTime[id][i]-reqEnterTime[id][i]); 

		worst_case=max(worst_case,waiting_time[id]);

		// simulation of critical section

		usleep(t1);

		// exitsection

		atomic_exchange_explicit(&foo,false,memory_order_release);
	
		gettimeofday(&tp[id],NULL);

		exitTime[id][i]=(double)tp[id].tv_sec+(double)tp[id].tv_usec*(1e-6)-start_time;

		// simulation of reminder section

		usleep(t2);		
	}
}

int main()
{	
	FILE *fptr1=fopen("inp-params.txt","r");

	FILE *fptr2=fopen("tas.txt","w");

	fscanf(fptr1,"%d %d %d %d",&NUM_THREADS,&NUM_TIMES,&t1,&t2);

	thread myThreads[NUM_THREADS];

	struct timeval bar;

	gettimeofday(&bar,NULL);

	start_time=(double)bar.tv_sec+(double)bar.tv_usec*(1e-6);	

	for(int i=0;i<NUM_THREADS;i++)
	{
		myThreads[i]=thread(testCS,i);
	}

	for(int i=0;i<NUM_THREADS;i++)
	{
		myThreads[i].join();
	}

	for(int i=0;i<NUM_THREADS;i++)
	{
		for(int j=0;j<NUM_TIMES;j++)
		{
			vec.push_back({reqEnterTime[i][j],i+1,j+1,1});

			vec.push_back({actEnterTime[i][j],i+1,j+1,2});

			vec.push_back({exitTime[i][j],i+1,j+1,3});
		}
	}

	sort(vec.begin(),vec.end(),mycomp);

	fprintf(fptr2,"Number of threads : %d\n",NUM_THREADS);

	fprintf(fptr2,"Number of times each thread is executed : %d\n",NUM_TIMES);

	fprintf(fptr2,"Worst case waiting time (in micro seconds) : %.9lf\n",worst_case*1e6);

	for(int i=0;i<NUM_THREADS;i++)
	{
		fprintf(fptr2,"Thread %d : Average waiting time(micro seconds) : %.9lf \n",i+1,waiting_time[i]/(NUM_THREADS)*1e6);
	}

	fprintf(fptr2,"Assuming application starts at 0.0000000 microseconds\n");

	for(int i=0;i<vec.size();i++)
	{
		capsule temp=vec[i];

		if(temp.type==1)
		{
			fprintf(fptr2,"%d th CS Requested at %.9lf by thread %d\n",temp.num,temp.xtime*1e6,temp.thread_id);				
		}

		else if(temp.type==2)
		{
			fprintf(fptr2,"%d th CS Entered at %.9lf by thread %d\n",temp.num,temp.xtime*1e6,temp.thread_id);
		}

		else
		{
			fprintf(fptr2,"%d th CS Exited at %.9lf by thread %d\n",temp.num,temp.xtime*1e6,temp.thread_id);
		}
	}

	return 0;
}