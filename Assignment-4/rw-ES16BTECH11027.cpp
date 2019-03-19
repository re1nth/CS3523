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

int r_threads,w_threads;
int r_count,w_count;
double cri_myu,rem_myu;
int rcounter;

default_random_engine generator;

struct timeval *tp1;
struct timeval *tp2;
double start_time;

double *rreq_time;
double *rent_time;
double *wreq_time;
double *went_time;
double *r_wait;
double *w_wait;
double *r_worst;
double *w_worst;

FILE *fp1,*fp2;
sem_t *rw_mut,*mut;

static const char *semname1 = "Semaphore1";
static const char *semname2 = "Semaphore2";

void* reader(void* arg)
{
	exponential_distribution<double>expo1((double)1/cri_myu);
	exponential_distribution<double>expo2((double)1/rem_myu);

	int id=*(static_cast<int*>(arg));

	free(arg);

	for(int i=0;i<r_count;i++)
	{
		gettimeofday(&tp1[id],NULL);
		double reqTime=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6)-start_time;
		reqTime*=1000;
		fprintf(fp1, "%dth CS Request by Reader Thread %d at %lf\n",i+1,id+1,reqTime);
		rreq_time[id]=reqTime;

		sem_wait(mut);	
		rcounter++;
		if(rcounter==1) sem_wait(rw_mut);
		sem_post(mut);

		gettimeofday(&tp1[id],NULL);
		double EnterTime=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6)-start_time;
		EnterTime*=1000;
		rent_time[id]=EnterTime;
		fprintf(fp1, "%dth CS Entry by Reader Thread %d at %lf\n",i+1,id+1,EnterTime);

		r_wait[id]+=(rent_time[id]-rreq_time[id]);
		r_worst[id]=max(r_worst[id],rent_time[id]-rreq_time[id]);

		usleep(expo1(generator)*1e3);

		sem_wait(mut);
		rcounter--;
		if(rcounter==0) sem_post(rw_mut);
		sem_post(mut);

		gettimeofday(&tp1[id],NULL);
		double exitTime=(double)tp1[id].tv_sec+(double)tp1[id].tv_usec*(1e-6)-start_time;
		exitTime*=1000;
		fprintf(fp1, "%dth CS Exit by Reader Thread %d at %lf\n",i+1,id+1,exitTime);
	
		usleep(expo2(generator)*1e3);
	}

	r_wait[id]/=(r_count);

	return NULL;
}

void* writer(void* arg)
{
	int id=*(static_cast<int*>(arg));

	free(arg);

	exponential_distribution<double>expo1((double)1/cri_myu);
	exponential_distribution<double>expo2((double)1/rem_myu);

	for(int i=0;i<w_count;i++)
	{
		gettimeofday(&tp2[id],NULL);
		double reqTime=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6)-start_time;
		reqTime*=1000;
		fprintf(fp1, "%dth CS Request by Writer Thread %d at %lf\n",i+1,id+1,reqTime);
		wreq_time[id]=reqTime;

		sem_wait(rw_mut);

		gettimeofday(&tp2[id],NULL);
		double EnterTime=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6)-start_time;
		EnterTime*=1000;
		fprintf(fp1, "%dth CS Entry by Writer Thread %d at %lf\n",i+1,id+1,EnterTime);
		went_time[id]=EnterTime;

		w_wait[id]+=(went_time[id]-wreq_time[id]);
		w_worst[id]=max(w_worst[id],went_time[id]-wreq_time[id]);

		usleep(expo1(generator)*1e3);

		sem_post(rw_mut);

		gettimeofday(&tp2[id],NULL);
		double exitTime=(double)tp2[id].tv_sec+(double)tp2[id].tv_usec*(1e-6)-start_time;
		exitTime*=1000;
		fprintf(fp1, "%dth CS Exit by Writer Thread %d at %lf\n",i+1,id+1,exitTime);
		usleep(expo2(generator)*1e3);
	}

	w_wait[id]/=(w_count);

	return NULL;
}

int main()
{
	ifstream input("inp-params.txt");
	input>>r_threads>>w_threads>>r_count>>w_count>>cri_myu>>rem_myu;
	input.close();

	fp1=fopen("RW-log.txt","w");
	fp2=fopen("RW-Average_time.txt","w");

	rw_mut=sem_open(semname1,O_CREAT|O_EXCL,0644,1);
	mut=sem_open(semname2,O_CREAT|O_EXCL,0644,1);

	pthread_t threadr[r_threads+1];
	pthread_t threadw[w_threads+1];

	rreq_time=(double*)malloc((r_threads+1)*sizeof(double));
	wreq_time=(double*)malloc((w_threads+1)*sizeof(double));
	rent_time=(double*)malloc((r_threads+1)*sizeof(double));
	went_time=(double*)malloc((w_threads+1)*sizeof(double));
	r_wait=(double*)malloc((r_threads+1)*sizeof(double));
	w_wait=(double*)malloc((w_threads+1)*sizeof(double));
	r_worst=(double*)malloc((r_threads+1)*sizeof(double));
	w_worst=(double*)malloc((w_threads+1)*sizeof(double));

	tp1=(struct timeval*)malloc((r_threads+1)*sizeof(struct timeval));
	tp2=(struct timeval*)malloc((w_threads+1)*sizeof(struct timeval));

	struct timeval bar;
	gettimeofday(&bar,NULL);
	start_time=(double)bar.tv_sec+(double)bar.tv_usec*(1e-6);

	for(int i=0;i<r_threads;i++)
	{
        int *x=(int*)malloc(sizeof(int));
        *x=i;
        r_wait[i]=0;
        r_worst[i]=0;
		pthread_create(&threadr[i],NULL,reader, (void*)x);
	}

	for(int i=0;i<w_threads;i++)
	{
        int *y=(int*)malloc(sizeof(int));
		*y=i;

		w_wait[i]=0;
		w_worst[i]=0;

		pthread_create(&threadw[i],NULL,writer, (void*)y);
	}

	for(int i=0;i<r_threads;i++)
	{
		pthread_join(threadr[i],NULL);
	}

	for(int i=0;i<w_threads;i++)
	{
		pthread_join(threadw[i],NULL);
	}

	double avg_read=0,avg_write=0;
	double wor_read=0,wor_write=0;

	for(int i=0;i<r_threads;i++)
	{
		avg_read+=(r_wait[i]);
		fprintf(fp2,"Average waiting time for reader thread %d is %lf\n",i+1,r_wait[i]);
	}

	fprintf(fp2,"--------------------------------------\n");

	for(int i=0;i<w_threads;i++)
	{
		avg_write+=(w_wait[i]);
		fprintf(fp2,"Average waiting time for writer thread %d is %lf\n",i+1,w_wait[i]);
	}

	fprintf(fp2,"--------------------------------------\n");

	for(int i=0;i<r_threads;i++)
	{
		wor_read+=(r_worst[i]);
		fprintf(fp2,"Worst case time for reader thread %d is %lf\n",i+1,r_worst[i]);
	}

	fprintf(fp2,"--------------------------------------\n");

	for(int i=0;i<w_threads;i++)
	{
		wor_write+=(w_worst[i]);
		fprintf(fp2,"Worst case time for writer thread %d is %lf\n",i+1,w_worst[i]);
	}	

	fprintf(fp2,"--------------------------------------\n");

	avg_read/=(r_threads);
	avg_write/=(w_threads);
	wor_read/=(r_threads);
	wor_write/=(w_threads);

	fprintf(fp2,"Average waiting time for reader thread is %lf\n",avg_read);
	fprintf(fp2,"Average waiting time for writer thread is %lf\n",avg_write);
	fprintf(fp2,"Average worst time for reader thread is %lf\n",wor_read);
	fprintf(fp2,"Average worst time for writer thread is %lf\n",wor_write);	

	free(rreq_time);
	free(wreq_time);
	free(rent_time);
	free(went_time);
	free(r_wait);
	free(w_wait);
	free(r_worst);
	free(w_worst);
    free(tp1);
	free(tp2);
	fclose(fp1);
	fclose(fp2);

	sem_unlink(semname1);
	sem_unlink(semname2);

	return 0;
}