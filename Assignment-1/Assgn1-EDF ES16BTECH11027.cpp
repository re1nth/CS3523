#include <bits/stdc++.h>

using namespace std;

struct Event
{
	int process_id;
	float interrupt;
	float process_time;
	int action;
	float period;
};

struct Process
{
	int process_id;
	float process_time;
	float deadline;
	float period;
};

struct Fuple
{
	int process_id;
	float process_time;
	float period;
	int num_of_times;
};

vector<Event> Event_list;

queue<Process> Process_queue;

vector<Fuple>Store;

Process running;

int total_processes,completed_processes,missed_processes;

float start_time[30];

float waiting_time[30];

int prev_process;

FILE *fptr1,*fptr2,*fptr3;

bool compare(Event e1, Event e2)
{
	return (e1.interrupt<e2.interrupt);
}

void next_process()
{
	int cnt=0,sz=Process_queue.size();

	float deadline_temp=1e9,exec_temp=1e9;

	while(cnt<sz)
	{
		Process ref=Process_queue.front();	

		if(deadline_temp==1e9||(ref.deadline<deadline_temp))
		{
			running=ref;

			deadline_temp=ref.deadline;

			exec_temp=ref.process_time;
		}

		else if(ref.deadline==deadline_temp)
		{
			if(ref.process_time<exec_temp)
			{
				running=ref;

				exec_temp=ref.process_time;
			}
		}

		Process_queue.pop();

		Process_queue.push(ref);

		cnt++;
	}

	if(deadline_temp==1e9)
	{
		running.process_id=-1;

		running.process_time=1e9;

		running.deadline=-1;

		running.period=1e9;
	}
}

void update(int process_id,float time_executes)
{	
	int cnt=0,sz=Process_queue.size();

	while(cnt<sz)
	{
		Process ref=Process_queue.front();	

		Process_queue.pop();

		if(ref.process_id==process_id)
		{
			if(time_executes==-1)
			{
				if(ref.process_time>0) 
				{
					waiting_time[ref.process_id]+=(ref.deadline-start_time[ref.process_id]);

					fprintf(fptr2,"The process %d is killed at deadline %f\n",ref.process_id,ref.deadline);

					missed_processes++;
				}

				if(prev_process==ref.process_id) prev_process=-1;
			}

			else if(time_executes==-2)
			{
				completed_processes++;
			}

			else
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

	fptr1=fopen("inp-params.txt","r");

	fptr2=fopen("ED-Log.txt","w");

	fptr3=fopen("ED-Stats.txt","w");

	fscanf(fptr1,"%d",&num_of_process);

	int process_id,num_of_times;

	float period,process_time;

	float prev_left=0;

	float tim[num_of_process+1];

	int repeat[num_of_process+1];

	prev_process=-1;

	while(cnt<num_of_process)
	{
		fscanf(fptr1,"%d %f %f %d",&process_id,&process_time,&period,&num_of_times);

		fprintf(fptr2,"Process P%d: processing time=%f; deadline:%f, period=%f joined the system at time 0\n",process_id,process_time,period,period);

		repeat[cnt+1]=num_of_times;

		total_processes+=(num_of_times);

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
		float interrupt=it->interrupt;

		if(!Process_queue.empty()) next_process();

		queue<Process>Temp_queue;

		while(it->interrupt==interrupt && it!=Event_list.end())
		{	
			if(it->action==1)
			{
				process_temp.process_id=it->process_id;

				process_temp.process_time=it->process_time;

				process_temp.period=it->period;

				process_temp.deadline=it->interrupt+it->period;

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

			start_time[Temp_queue.front().process_id]=interrupt;

			Temp_queue.pop();
		}

		if(it==Event_list.end()) break;

		float curr=interrupt;

		float deadline=it->interrupt;

		cnt=0; 

		int sz=Process_queue.size();

		if(!Process_queue.empty()) next_process();

		if(running.process_id==-1) break;

		float execution_time=running.process_time;

		do		
		{
			waiting_time[running.process_id]+=(curr-start_time[running.process_id]);

			if(prev_process!=-1)
			{
				if(prev_process!=running.process_id)
				{
					fprintf(fptr2,"Process P%d is preempted by P%d at %f. Remaining process time:%f \n",prev_process,running.process_id,curr,prev_left);
				}
			}

			if(running.process_time==tim[running.process_id])
			{
				fprintf(fptr2,"Process P%d starts execution at %f \n",running.process_id,curr);	
			}

			else
			{
				if(prev_process!=running.process_id)
				{
					fprintf(fptr2,"Process P%d resumes execution at %f \n",running.process_id,curr);
				}
			}		
	
			if(curr+execution_time<=deadline)
			{
				start_time[running.process_id]=curr+execution_time;

				update(running.process_id,-2);			

				fprintf(fptr2,"Process P%d finishes execution at %f \n",running.process_id,(curr+execution_time));

				prev_process=-1;
			}

			else 
			{
				start_time[running.process_id]=deadline;

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

	fprintf(fptr3,"Time displayed in microseconds\n");

	fprintf(fptr3,"Number of processes that came into the system: %d\n",total_processes);

	fprintf(fptr3,"Number of processes that successfully completed:%d\n",completed_processes);

	fprintf(fptr3,"Number of processes that missed their deadlines: %d\n",missed_processes);

	float sum1=0,sum2=0;

	for(int i=1;i<=num_of_process;i++)
	{
		fprintf(fptr3,"Average Waiting time for process %d is %f \n",i,(waiting_time[i]/repeat[i]));
	
		sum1+=(waiting_time[i]);

		sum2+=(repeat[i]);
	}

	fprintf(fptr3,"Average waiting time is %f\n",sum1/sum2);

	fclose(fptr1);

	fclose(fptr2);

	fclose(fptr3);

	return 0;
}