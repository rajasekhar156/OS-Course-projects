#include <iostream> 
#include <fstream> 
#include <pthread.h> 
#include <limits.h>
#include <random> 
#include <algorithm> 
#include <vector>
#include <semaphore.h>
#include <sys/time.h>
#include <cstdlib> 
#include <chrono>
#include <unistd.h> 
using namespace std;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

int nw,nr,kw,kr,count_r=0;
float MUIcs,MUIrem;
FILE *frw_log = fopen("RW-log.txt","w");
FILE *avg_log = fopen("Average_RW_time.txt","w+");
sem_t mut,rwmut;
float w_avgtime = 0, r_avgtime = 0, w_maxtime = 0, r_maxtime = 0;

vector<int> func_Time(time_t param)
{
    vector<int> times;
    tm *TIME;
    TIME = localtime(&param);
    times.push_back(TIME->tm_hour);
    times.push_back(TIME->tm_min);
    times.push_back(TIME->tm_sec);
    return times;
}  

default_random_engine creator1,creator2;
exponential_distribution<double> Dcs;
exponential_distribution<double> Drem;

void *writerfunc(void *input)
{
    int threadid = *(int*)input;
    int i;
    vector<int> times;
    for(i=0;i<kw;i++)
    {
        time_t reqenter_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(reqenter_time/1000000);
        fprintf(frw_log,"%d CS Request by Writer Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        sem_wait(&rwmut);
        // entry_section

        
        time_t start_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(start_time/1000000);
        fprintf(frw_log,"%d CS Entry by Writer Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        w_avgtime = w_avgtime + (start_time - reqenter_time);
        float temp = (start_time - reqenter_time);
        w_maxtime = max(w_maxtime,temp);
        creator1.seed(threadid*nw + i);
        usleep(Dcs(creator1)*1000000);
        sem_post(&rwmut);
        // critical_section

        time_t exit_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(exit_time/1000000);
        fprintf(frw_log,"%d CS Exit by Writer Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        creator2.seed(threadid*nw + i);
        usleep(Drem(creator2)*1000000);
        // remainder_section
    }
    return 0;
}

void *readerfunc(void *input)
{
    int threadid = *(int*)input;
    int i;
    vector<int> times;
    for(i=0;i<kr;i++)
    {
        time_t reqenter_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(reqenter_time/1000000);
        fprintf(frw_log,"%d CS Request by Reader Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        sem_wait(&mut);
        count_r++;
        if(count_r == 1)
        {
            sem_wait(&rwmut);
        }
        sem_post(&mut);
        // entry_section

        
        time_t start_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(start_time/1000000);
        fprintf(frw_log,"%d CS Entry by Reader Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        r_avgtime = r_avgtime + (start_time - reqenter_time);
        float temp = (start_time - reqenter_time); 
        r_maxtime = max(r_maxtime,temp); 
        creator1.seed(threadid*nr + i); 
        usleep(Dcs(creator1)*1000000);
        sem_wait(&mut);
        count_r--;
        if(count_r == 0)
        {
            sem_post(&rwmut);
        }
        sem_post(&mut);
        // critical_section

        time_t exit_time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        times = func_Time(exit_time/1000000);
        fprintf(frw_log,"%d CS Exit by Reader Thread %d at %d:%d:%d \n",i+1,threadid,times[0],times[1],times[2]);
        creator2.seed(threadid*nr + i);
        usleep(Drem(creator2)*1000000);
        // remainder_section
    }
    return 0;
}

int main()
{
    int i;
    ifstream fin;
    fin.open("inp-params.txt");
    fin >> nw >> nr >> kw >> kr >> MUIcs >> MUIrem ;
    //fstream fout;
    //fout.open("Average_time.txt");
    Dcs = exponential_distribution<double>(1000.0/MUIcs);
    Drem = exponential_distribution<double>(1000.0/MUIrem);
    sem_init(&mut,0,1);
    sem_init(&rwmut,0,1);
    pthread_t writer_threadid[nw];
    pthread_t reader_threadid[nr];
    pthread_attr_t writer_attr[nw];
    pthread_attr_t reader_attr[nr];
    int writerid[nw];
    int readerid[nr];

    for(i=0;i<nw;i++)
    {
        writerid[i] = i+1;
        pthread_attr_init(&writer_attr[i]);
        pthread_create(&writer_threadid[i],&writer_attr[i],writerfunc,&writerid[i]);
    }
    for(i=0;i<nr;i++)
    {
        readerid[i] = i+1;
        pthread_attr_init(&reader_attr[i]);
        pthread_create(&reader_threadid[i],&reader_attr[i],readerfunc,&readerid[i]);
    }
    for(i=0;i<nw;i++)
    {
       pthread_join(writer_threadid[i],NULL);
    }
    for(i=0;i<nr;i++)
    {
       pthread_join(reader_threadid[i],NULL);
    }
    
    fprintf(avg_log, "Average Time Taken for thread to gain entry in RW is %f" , (w_avgtime + r_avgtime)/(nw*kw*1000 + nr*kr*1000));
	cout << "Average Time Taken for Reader Threads" << r_avgtime/(nr*kr)/1000.0 << "ms" <<endl;
	cout << "Average Time Taken for Writer Threads" << w_avgtime/(nw*kw)/1000.0 << "ms" <<endl;
	cout << "Worst   Time Taken for Reader Threads" << r_maxtime/1000.0 <<"ms"<<endl;
	cout << "Worst   Time Taken for Writer Threads" << w_maxtime/1000.0 <<"ms"<<endl;
    return 0;
}

