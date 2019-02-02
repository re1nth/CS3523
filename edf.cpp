#include <bits/stdc++.h>

using namespace std;

struct Event
{
	int process_id;
	float interrupt;
	int action;
};

struct Process
{
	int process_id;
	float process_time;
	int period;
};

struct Fuple
{
	int process_id;
	float process_time;
	int period;
	int num_of_times;
};

vector<Event> Event_queue;

queue<Process> Process_queue;

vector<Fuple>Store;

bool compare(Event e1, Event e2)
{
	return(e1.interrupt<e2.interrupt);
}

int main()
{
	int num_of_process,cnt=0; 

	FILE *fptr=NULL;

	fptr=fopen("inp-params.txt","r");

	fscanf(fptr,"%d",&num_of_process);

	int process_id,period,num_of_times;

	float process_time;

	while(cnt<num_of_process)
	{
		fscanf(fptr,"%d %f %d %d",&process_id,&process_time,&period,&num_of_times);

		Fuple *temp=new Fuple;

		*temp={process_id,process_time,period,num_of_times};

		Store.push_back(*temp);

		cnt++;
	}
	
	fclose(fptr);	

	Event *event_temp=new Event;

	cnt=0;

	for(auto it: Store)
	{
		process_id=it.process_id;

		process_time=it.process_time;

		period=it.period;

		num_of_times=it.num_of_times;

		for(int i=0;i<num_of_times;i++)
		{
			event_temp->process_id=process_id;

			event_temp->interrupt=(i*period);

			event_temp->action=(1);

			Event_queue.push_back(*event_temp);
		
			event_temp->process_id=process_id;

			event_temp->interrupt=((i+1)*period);

			event_temp->action=(-1);

			Event_queue.push_back(*event_temp);
		}

		cnt++;
	}

	sort(Event_queue.begin(),Event_queue.end(),compare);

	delete event_temp;

	return 0;
}