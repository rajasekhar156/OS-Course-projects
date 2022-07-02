#include <bits/stdc++.h>
#include <fstream>
#include <limits.h>
using namespace std;
int curr_time = 0;

struct Process
{
  int procid;
  int proctime;
  int period;
  int repeatnum;
  int arrivaltime;
  int completiontime;
  int deadline;
  int remainningtime;
  int waitingtime;
  bool preemption;
  bool termination;
  int unfinishedprocess;
};

class compare
{
  public:
    bool operator() ( Process *node1,  Process *node2)
    {
      return (node1->deadline-curr_time) > (node2->deadline-curr_time);
    }
};

bool Totalprocessisnotover (struct Process *temp, int n)
{
    for(int i=0;i<n;i++)
    {
        if((temp+i)->repeatnum>0)
        {
            return true;
        }
    }
    return false;
}


int main()
{
  ifstream fin;
  int n,i;


  fin.open("inp-params.txt");
  fin >> n;
  int *temprepeats=(int*)malloc(n*sizeof(int));

  struct Process *process = (struct Process*)malloc(n*sizeof(struct Process));
  priority_queue< Process* , vector<Process*> , compare> pqueue;

  for(i=0;i<n;i++)
  {
    fin >> process[i].procid >> process[i].proctime >> process[i].period >> process[i].repeatnum;
    *(temprepeats+i) = process[i].repeatnum;
  }
  
  ofstream fout;
  fout.open("EDF-Log.txt");

  struct Process *tempprocess1;
  struct Process *tempprocess2 = tempprocess1;
  bool cpuisidleornot;
  bool tempvar;
  int numofprocentering=0;
  int numofproccompleted=0;
  int numofprocdeadlinemissed=0;
  for(i=0;i<n;i++)
  {
    numofprocentering = numofprocentering + (process+i)->repeatnum;
  }
  while(Totalprocessisnotover(process,n))
  {
    for(i=0;i<n;i++)
    {
      if((process + i)->repeatnum > 0)
      {
        if(curr_time % process[i].period == 0)
        {
          if((process + i)->remainningtime > 0 && (process + i)->deadline < curr_time +1)
          {
            fout << "Process" <<(process+i)->procid << " missed deadline at time " << curr_time << "terminated" << endl;
            (process + i)->unfinishedprocess = (process + i)->unfinishedprocess + 1;
            (process + i)->waitingtime = (process+i)->waitingtime + curr_time - (process+i)->arrivaltime - (process+i)->proctime + (process+i)->remainningtime;
            (process + i)->repeatnum = (process + i)->repeatnum - 1;
            (process + i)->remainningtime = (process + i)->proctime;
            (process + i)->preemption = false;
            if((process+i)->repeatnum==0)
            {
                (process+i)->termination=true;
            }
            else
            {
                (process+i)->deadline=curr_time + (process+i)->period;
                numofprocdeadlinemissed = numofprocdeadlinemissed + 1;
                (process+i)->arrivaltime = curr_time;
            }
            if((pqueue.top())->procid == (process+i)->procid)
            {
                pqueue.pop();
                pqueue.push(process+i);
                tempprocess2 = pqueue.top();
            }

            (process + i)->deadline = curr_time + (process + i)->period;
            (process + i)->arrivaltime = curr_time;
          }
          else
          {
            process[i].arrivaltime = curr_time ;
            process[i].deadline = curr_time + process[i].period;
            process[i].remainningtime = process[i].proctime;
            pqueue.push(&process[i]);
            fout << "Process" << process[i].procid << ":processing time =" << process[i].proctime << "; deadline:" << process[i].deadline << "; period:" << process[i].period << "joined the system at time " << curr_time << endl;
          }
        }
      }
    }
    tempprocess1 = pqueue.top();
    if(pqueue.empty() == true )
    {
      cpuisidleornot = true;
    }
    else
    {
      if(cpuisidleornot)
      {
        fout << "CPU is idle till time " << curr_time -1 << endl;
        cpuisidleornot = false;
        tempvar = true;
      }
      if(tempprocess2!=tempprocess1 && tempprocess2!=NULL)
      {
       if(tempprocess2->remainningtime>0)
        {
          tempprocess2->preemption = true;
          fout << "Process" << tempprocess2->procid << " is preempted by " << "Process" << tempprocess1->procid << " at time " << curr_time << " remainning processing time " << tempprocess2->remainningtime << endl;
        }
      }
      if(tempprocess1->remainningtime==1)
      {
        fout << "Process" << tempprocess1->procid << " finishes execution at time " << curr_time + 1  << endl;
      }
      if(tempprocess1->remainningtime==tempprocess1->proctime)
      {
        fout << "Process" << tempprocess1->procid << " started execution at time " ;
        if(tempvar)
        {
          fout << curr_time << endl;
          tempvar = false ;
        }
        else
        {
          fout << curr_time + 1 << endl;
        }
      }
      if(tempprocess1->preemption==true)
      {
        fout << "Process " << tempprocess1->procid << " resumes its execution at time " << curr_time + 1 << endl;
        tempprocess1->preemption = false;
      }
      if(tempprocess1->remainningtime == 1)
      {
        tempprocess1->repeatnum = tempprocess1->repeatnum - 1;
        tempprocess1->remainningtime = tempprocess1->remainningtime - 1;
        tempprocess1->waitingtime = tempprocess1->waitingtime + curr_time - tempprocess1->arrivaltime - tempprocess1->proctime + 1;
        numofproccompleted = numofproccompleted + 1;
        pqueue.pop();
      }
      else
      {
        tempprocess1->remainningtime = tempprocess1->remainningtime - 1;
      }
    }

    if(!pqueue.empty())
    {
      tempprocess2 = tempprocess1;
    }
    else
    {
      tempprocess2 = NULL;
    }
    curr_time = curr_time +1 ;
  }
  
  ofstream foutput;
  foutput.open("EDF-Stats.txt");
  
  foutput<< "The number of process that came into the system " << numofprocentering << endl;
  foutput<< "The number of process that succesfully completed "<< numofproccompleted << endl;
  foutput<< "The number of process that missed deadlines are "<< numofprocdeadlinemissed << endl;
  float temp;
  int temp2=0;
  float temp3;
  for(i=0;i<n;i++)
  {
    temp = *(temprepeats+i);
    temp = (process+i)->waitingtime/temp;
    temp2 = temp2 + (process+i)->waitingtime;
    foutput<< "Average waiting time for process "<<(process+i)->procid << "is: " << temp << endl;
  }
  temp3 = temp2/numofprocentering;
  foutput<< "Average waiting time for complete process is: " << temp3 << endl;
  fin.close();
  return 0;
}