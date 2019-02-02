#include <bits/stdc++.h>

using namespace std;

struct Event
{
	int process_id;
	int interrupt;
	int process_time;
	int action;
	int period;
};

struct Process
{
	int process_id;
	int process_time;
	int period;
};

struct Fuple
{
	int process_id;
	int process_time;
	int period;
	int num_of_times;
};

vector<Event> Event_list;

queue<Process> Process_queue;

vector<Fuple>Store;

Process running;

bool compare(Event e1, Event e2)
{
	return (e1.interrupt<e2.interrupt);
}

void next_process()
{
	int cnt=0,sz=Process_queue.size();

	int period_temp=INT_MAX;

	while(cnt<sz)
	{
		Process ref=Process_queue.front();

		if(period_temp==INT_MAX || (ref.period<period_temp))
		{
			running=ref;

			period_temp=ref.period;
		}	

		Process_queue.pop();

		Process_queue.push(ref);

		cnt++;
	}

	if(period_temp==INT_MAX)
	{
		running.process_id=-1;

		running.process_time=INT_MAX;

		running.period=INT_MAX;
	}
}

void update(int process_id,int time_executes)
{	
	int cnt=0,sz=Process_queue.size();

	while(cnt<sz)
	{
		Process ref=Process_queue.front();	

		Process_queue.pop();

		if(ref.process_id==process_id)
		{
			if(time_executes!=-1)
			{
				if(ref.process_time-time_executes>0)
				{
					ref.process_time=(ref.process_time-time_executes);

					Process_queue.push(ref);	
				}
			}
		}	

		else Process_queue.push(ref);

		cnt++;
	}
}

int main()
{
	int num_of_process,cnt=0; 

	FILE *fptr1=NULL;

	FILE *fptr2=NULL;

	fptr1=fopen("inp-params.txt","r");

	fptr2=fopen("RM-Log.txt","w");

	fscanf(fptr1,"%d",&num_of_process);

	int process_id,period,num_of_times;

	int process_time;

	int prev_process=-1;

	int prev_left=0;

	int tim[num_of_process+1];

	while(cnt<num_of_process)
	{
		fscanf(fptr1,"%d %d %d %d",&process_id,&process_time,&period,&num_of_times);

		fprintf(fptr2,"Process P%d: processing time=%d; deadline:%d, period=%d joined the system at time 0\n",process_id,process_time,period,period);

		Fuple temp;

		temp={process_id,process_time,period,num_of_times};

		tim[process_id]=process_time;

		Store.push_back(temp);

		cnt++;
	}
	
	Event event_temp;

	Process process_temp;

	cnt=0;

	for(auto it: Store)
	{
		process_id=it.process_id;

		process_time=it.process_time;

		period=it.period;

		num_of_times=it.num_of_times;

		for(int i=0;i<num_of_times;i++)
		{
			event_temp.process_id=process_id;

			event_temp.interrupt=(i*period);

			event_temp.process_time=(process_time);

			event_temp.action=(1);

			event_temp.period=period;

			Event_list.push_back(event_temp);
		
			event_temp.process_id=process_id;

			event_temp.interrupt=((i+1)*period);

			event_temp.process_time=process_time;

			event_temp.period=period;

			event_temp.action=(-1);

			Event_list.push_back(event_temp);
		}

		cnt++;
	}

	sort(Event_list.begin(),Event_list.end(),compare);

	for(auto it=Event_list.begin();it!=Event_list.end();)
	{
		int interrupt=it->interrupt;

		if(!Process_queue.empty())next_process();

		queue<Process>Temp_queue;

		while(it->interrupt==interrupt && it!=Event_list.end())
		{	
			if(it->action==1)
			{
				process_temp.process_id=it->process_id;

				process_temp.process_time=it->process_time;

				process_temp.period=it->period;

				Temp_queue.push(process_temp);
			}

			else
			{
				update(it->process_id,-1);
			}

			it++;
		}	

		while(!Temp_queue.empty())
		{
			Process_queue.push(Temp_queue.front());

			Temp_queue.pop();
		}

		if(it==Event_list.end()) break;

		int curr=interrupt;

		int deadline=it->interrupt;

		cnt=0; 

		int sz=Process_queue.size();

		if(!Process_queue.empty()) next_process();

		if(running.process_id==-1) break;

		int execution_time=running.process_time;

		do		
		{
			if(prev_process!=-1)
			{
				if(prev_process!=running.process_id)
				{
					fprintf(fptr2,"Process P%d is preempted by P%d at %d. Remaining process time:%d \n",prev_process,running.process_id,(int)curr,(int)prev_left);
				}
			}

			if(running.process_time==tim[running.process_id])
			{
				fprintf(fptr2,"Process P%d starts execution at %d \n",running.process_id,(int)curr);	
			}

			else
			{
				if(prev_process!=running.process_id)
				{
					fprintf(fptr2,"Process P%d resumes execution at %d \n",running.process_id,(int)curr);
				}
			}			
	
			if(curr+execution_time<=deadline)
			{
				update(running.process_id,-1);			

				fprintf(fptr2,"Process P%d finishes execution at %d \n",running.process_id,(int)(curr+execution_time));
			
				prev_process=-1;
			}

			else 
			{
				prev_process=running.process_id;

				prev_left=running.process_time-(deadline-curr);

				update(running.process_id,deadline-curr);
			}

			next_process();

			curr=curr+execution_time;

			if(running.process_id!=-1) execution_time=running.process_time;
		
			else break;

		} while(curr<deadline);	
	}

	fclose(fptr1);

	fclose(fptr2);

	return 0;
}