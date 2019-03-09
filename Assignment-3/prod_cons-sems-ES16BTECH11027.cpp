#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <fstream>
#include <random>
#include <string.h>

using namespace std;

int capacity;
int p_threads,c_threads;
int p_count,c_count;
double p_myu,c_myu;

default_random_engine generator;

struct timeval *tp1;
struct timeval *tp2;
double start_time;

double *preq_time;
double *pcom_time;
double *creq_time;
double *ccom_time;

int in,out,next_produced,next_consumed;

FILE *fp;
sem_t *full, *empty, *foo;
int *buffer;

double producer_time;
double consumer_time;

static const char *semname1 = "Semaphore1";
static const char *semname2 = "Semaphore2";
static const char *semname3 = "Semaphore3";

void* producer(void* arg)
{
	exponential_distribution<double>expo1((double)1/p_myu);

	int id=*(static_cast<int*>(arg));

	free(arg);

	gettimeofday(&tp1[id],NULL);

	preq_time[id]=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6);

	for(int i=0;i<p_count;i++)
	{
		sem_wait(empty);
		sem_wait(foo);

		gettimeofday(&tp1[id],NULL);

		double EnterTime=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6)-start_time;

		//next_produced is evaluated and stored in the buffer by the producer

		++next_produced;

		buffer[in]=next_produced;

		fprintf(fp, "%dth item produced by thread %d at %lf into buffer location %d as %d\n",i+1,id+1,EnterTime,in,next_produced);

		in=(in+1)%capacity;

		sem_post(foo);
		sem_post(full);

		usleep(expo1(generator)*1e5);
	}

	gettimeofday(&tp1[id],NULL);

	pcom_time[id]=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6);

	producer_time+=(pcom_time[id]-preq_time[id]);

	return NULL;
}

void* consumer(void* arg)
{
	int id=*(static_cast<int*>(arg));

	free(arg);

	exponential_distribution<double>expo2((double)1/c_myu);

	gettimeofday(&tp2[id],NULL);

	creq_time[id]=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6);

	for(int i=0;i<c_count;i++)
	{
		sem_wait(full);
		sem_wait(foo);

		gettimeofday(&tp2[id],NULL);

		double EnterTime=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6)-start_time;

		//next_consumed is read from the buffer by the consumer

		next_consumed=buffer[out];

		fprintf(fp, "%dth item consumed by thread %d at %lf from buffer location %d as %d\n",i+1,id+1,EnterTime,out,next_consumed);

		out = (out + 1)%capacity;

		sem_post(foo);
		sem_post(empty);

		usleep(expo2(generator)*1e5);
	}

	gettimeofday(&tp2[id],NULL);

	ccom_time[id]=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6);

	consumer_time+=(ccom_time[id]-creq_time[id]);

	return NULL;
}

int main()
{
	ifstream input("inp-params.txt");
	input>>capacity>>p_threads>>c_threads>>p_count>>c_count>>p_myu>>c_myu;
	input.close();

	fp=fopen("sem-log.txt","w");

	foo=sem_open(semname1,O_CREAT|O_EXCL,0644,1);
	full=sem_open(semname2,O_CREAT|O_EXCL,0644,0);
	empty=sem_open(semname3,O_CREAT|O_EXCL,0644,capacity);

	pthread_t threadp[p_threads+1];
	pthread_t threadc[c_threads+1];

	buffer=(int*)malloc(capacity*sizeof(int));

	preq_time=(double*)malloc((p_threads+1)*sizeof(double));
	creq_time=(double*)malloc((c_threads+1)*sizeof(double));
	pcom_time=(double*)malloc((p_threads+1)*sizeof(double));
	ccom_time=(double*)malloc((c_threads+1)*sizeof(double));

	tp1=(struct timeval*)malloc((p_threads+1)*sizeof(struct timeval));
	tp2=(struct timeval*)malloc((c_threads+1)*sizeof(struct timeval));

	struct timeval bar;
	gettimeofday(&bar,NULL);
	start_time=(double)bar.tv_sec+(double)bar.tv_usec*(1e-6);

	for(int i=0;i<p_threads;i++)
	{
        int *x=(int*)malloc(sizeof(int));
        *x=i;

		pthread_create(&threadp[i],NULL,producer, (void*)x);
	}

	for(int i=0;i<c_threads;i++)
	{
        int *y=(int*)malloc(sizeof(int));
		*y=i;

		pthread_create(&threadc[i],NULL,consumer, (void*)y);
	}

	for(int i=0;i<p_threads;i++)
	{
		pthread_join(threadp[i],NULL);
	}

	for(int i=0;i<c_threads;i++)
	{
		pthread_join(threadc[i],NULL);
	}

	producer_time=producer_time/(double)p_count;
	consumer_time=consumer_time/(double)c_count;

	cout<<"Average producer waiting time: "<<producer_time<<"\n";
	cout<<"Average consumer waiting time: "<<consumer_time<<"\n";

	free(preq_time);
	free(creq_time);
	free(pcom_time);
	free(ccom_time);
    free(tp1);
	free(tp2);
	free(buffer);
	fclose(fp);

	sem_unlink(semname1);
	sem_unlink(semname2);
	sem_unlink(semname3);

	return 0;
}