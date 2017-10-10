/**
  Implementar:
  FCFS Simples [V] 0
  Round-Robin [V] 1
  FCFS com prioridade sem preempção [V] 2
  SJB [V] 3
  Round-Robin com prioridade e envelhecimento [V] 4
**/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"
#define T_MAX  50

enum state {NEW, READY, RUNNING, COMPLETED};

typedef struct process
{
   struct process *prev ;
   struct process *next ;
   int id ;
   int created_at;
   int completed_at;
   int duration;
   int dynamic_priority;
   int static_priority;
   int executed_time;
   int executed_quantum;
   enum state current_state;
} process ;

process *new_queue = NULL;
process *ready_queue = NULL;
process *running_current = NULL;
process *completed_queue = NULL;
int process_total = 0;
int current_time = 0;
int created_at = 0;
int duration = 0;
int static_priority = 0;
int context_swap = 0;

float get_live_time(process* queue){
  float n = 0;
  float lt = 0;
  process* first = queue;
  process* result = NULL;
  if(queue != NULL){
    do{
        lt = lt + ((float)(*queue).completed_at - (float)(*queue).created_at); 
        n = n + 1;
      queue = (*queue).next;
    }while(queue != first);
  }
  return lt/n;
}

float get_wait_time(process* queue){
  float n = 0;
  float wt = 0;
  process* first = queue;
  process* result = NULL;
  if(queue != NULL){
    do{
        wt = wt + (((float)(*queue).completed_at - (float)(*queue).created_at) - (float)(*queue).duration); 
        n = n + 1;
      queue = (*queue).next;
    }while(queue != first);
  }
  return wt/n;
}

struct process* get_process(process* queue, int id){
  process* first = queue;
  process* result = NULL;
  if(queue != NULL){
    do{
      if((*queue).id == id){
        result = queue;
        break;
      }
      else{
        queue = (*queue).next;
      }
    }while(queue != first);
  }
  return result;
}

 struct process* create_process(int id , int created_at, int duration,
  int static_priority)
 {
   struct process* new_process = (process*) malloc(sizeof(process));
   (*new_process).id = id;
   (*new_process).created_at = created_at;
   (*new_process).duration = duration;
   (*new_process).dynamic_priority = 0;
   (*new_process).static_priority = static_priority;
   (*new_process).executed_time = 0;
   (*new_process).executed_quantum = 0;
   (*new_process).current_state = NEW;
 }

int init(){
  printf("How many process at all?\n");
  scanf("%d", &process_total);
  for (int i = 0; i < process_total; i++) {
      printf("Write process info: created_at duration static_priority:\n");
      scanf("%d %d %d", &created_at, &duration, &static_priority);
      struct process* p = create_process(i, created_at, duration, static_priority);
      queue_append((queue_t**)&new_queue, (queue_t*)p);
      //printf("P%d. created_at %d duration %d executed_time %d\n",
      //(*p).id, (*p).created_at, (*p).duration, (*p).executed_time);
  }
  return process_total;
}

void print_header(int n){
  printf("tempo  ");
  for (int i = 0; i < n; i++) {
    printf("P%d ", i);
  }
  printf("\n");
}

void print_body(int t, int n){
  printf("%2d %2d ", t, t+1);
  for (int i = 0; i < n; i++) {
    process *current = NULL;

    current = get_process(new_queue, i);
    if(current != NULL) //processo na fila de novos
    {
      printf(" nn");
    }

    current = get_process(ready_queue, i);
    if(current != NULL) //processo na fila de prontos
    {
      printf(" rr");
    }

    if(running_current != NULL && (*running_current).id == i) //processo executando
    {
      printf(" ##");
    }

    current = get_process(completed_queue, i);
    if(current != NULL) //processo na fila de prontos
    {
      printf(" cc");
    }
  }
  printf("\n");
}

//politic = 0
struct process *scheduler_fcfs(process **queue){
  return (process *)queue_remove((queue_t**)queue, (queue_t*)*queue);
}

//politic = 1
struct process *scheduler_fcfs_priority(process **queue){
  process *q = *queue;
  process* first = *queue;
  process* result = *queue;
  if(q != NULL){
    do{
      if((*q).static_priority < (*result).static_priority){
        result = q;
        break;
      }
      else{
        q = (*q).next;
      }
    }while(q != first);
  }
  //return result;
  return (process *)queue_remove((queue_t**)queue, (queue_t*)result);
}

//politic = 2
struct process *scheduler_sjf(process **queue){
  process *q = *queue;
  process* first = *queue;
  process* result = *queue;
  if(q != NULL){
    do{
      if((*q).duration < (*result).duration){
        result = q;
        break;
      }
      else{
        q = (*q).next;
      }
    }while(q != first);
  }
  //return result;
  return (process *)queue_remove((queue_t**)queue, (queue_t*)result);
}

//politc = 3
struct process *scheduler_rr(process **queue){
  process *q = *queue;
  process* first = *queue;
  process* result = *queue;
  if(q != NULL){
    do{
      if(((*q).static_priority - (*q).dynamic_priority) < ((*result).static_priority - (*result).dynamic_priority)){
        result = q;
        break;
      }
      else{
        q = (*q).next;
      }
    }while(q != first);
  }
  //return result;
  return (process *)queue_remove((queue_t**)queue, (queue_t*)result);
}

void dispatcher(int has_quantum, int quantum, int has_aging, int n, int politic){
  while(current_time < T_MAX)
  {
    if(running_current != NULL){
      if((*running_current).duration == (*running_current).executed_time){
        (*running_current).current_state = COMPLETED;
        (*running_current).completed_at = current_time;
        queue_append((queue_t**)&completed_queue, (queue_t*)running_current);
        running_current = NULL;
      }
      else if(has_quantum && (*running_current).executed_quantum == quantum){
         (*running_current).executed_quantum = 0;
          queue_append((queue_t**)&ready_queue, (queue_t*)running_current);
          running_current = NULL;
       }
    }

    if(queue_size((queue_t*)completed_queue) < n)
    {
      process *aux = new_queue;
      int count = queue_size((queue_t*)new_queue);
      for (int i = 0; i < count; i++) {
        if((*aux).created_at == current_time){
          queue_remove((queue_t**)&new_queue, (queue_t*)aux);
          queue_append((queue_t**)&ready_queue, (queue_t*)aux);
          (*aux).current_state = READY;
          aux = new_queue;
        }
        else{
          aux = (*aux).next;
        }
      }

      if(running_current == NULL)//processador ocioso
      {
        //pegar processo pronto se houver algum
        if(queue_size((queue_t*)ready_queue) > 0){
          
          process *p = NULL;
          
          if(politic == 0)
          {
            p = scheduler_fcfs(&ready_queue);  
          }
          else if(politic == 1)
          {
             p = scheduler_fcfs(&ready_queue);  
          }
          else if(politic == 2)
          {
             p = scheduler_fcfs_priority(&ready_queue);  
          }
          else if(politic == 3)
          {
             p = scheduler_sjf(&ready_queue);  
          }
          else if(politic == 4)
          {
             p = scheduler_rr(&ready_queue);  
          }
          
          running_current = p;
          (*running_current).current_state = RUNNING;
          context_swap++;
          (*running_current).executed_time = (*running_current).executed_time + 1;
          if(has_quantum) (*running_current).executed_quantum = (*running_current).executed_quantum + 1;
        }
      }
      else
      {
        (*running_current).executed_time = (*running_current).executed_time + 1;
        if(has_quantum) (*running_current).executed_quantum = (*running_current).executed_quantum + 1;
      }
      print_body(current_time, n);
    }

    if(has_aging && queue_size((queue_t*)ready_queue) > 0)
    {
      process *aux = ready_queue;
      int count = queue_size((queue_t*)ready_queue);
      for (int i = 0; i < count; i++) {
        (*aux).dynamic_priority = (*aux).dynamic_priority + 1;
        aux = (*aux).next;
      }
    }

    current_time++;
  }
}

int main(int argc, char const *argv[]) {
  int n, politc;
  n = init();
  printf("\n");
  print_header(n);
  
  dispatcher(0, 0, 0, n, 0); //FCFS Simples
  //dispatcher(1, 2, 0, n, 1); //Round-Robin
  //dispatcher(0, 0, 0, n, 2); //FCFS com prioridade sem preempção
  //dispatcher(0, 0, 0, n, 3); //SJB
  //dispatcher(1, 3, 1, n, 4); //Round-Robin com prioridade e envelhecimento
  
  printf("\nContext swap: %d", context_swap);
  printf("\nAvg lt: %2f", get_live_time((queue_t*)completed_queue));
  printf("\nAvg wt: %2f", get_wait_time((queue_t*)completed_queue));
  return 0;
}
