#include <bits/stdc++.h>

using namespace std;

struct Event
{
	float time;
	int process_id;
	int action;
};

struct Process
{
	int process_id;
	float process_time;
	int period;
};

queue<Event> Event_queue;
queue<Process> Process_queue;

int main()
{
	
	
	Event *x=new Event;

	x->time = 0.34;
	x->process_id = 1;
	x->action = -1;

	Event_queue.push(*x);

	while(!Event_queue.empty())
	{
		Event *ref=&(Event_queue.front());
		cout<<ref->time<<" "<<ref->process_id<<" "<<ref->action<<"\n";
		Event_queue.pop();
	}

	Process *y=new Process;

	y->process_id=1;
	y->process_time=2.34;
	y->period=3;

	Process_queue.push(*y);

	while(!Process_queue.empty())
	{
		Process *ref=&(Process_queue.front());
		cout<<ref->process_id<<" "<<ref->process_time<<" "<<y->period<<"\n";
		Process_queue.pop();
	}

	delete x;

	return 0;
}