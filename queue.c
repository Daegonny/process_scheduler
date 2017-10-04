#include <stdio.h>
#include "queue.h"

int queue_size (queue_t *queue)
{
  if(queue == NULL)
  {
    return 0;
  }
  else
  {
    int counter = 1;
    queue_t *last = NULL;
    last = (*queue).prev;
    while(queue != last)
    {
      queue = (*queue).next;
      counter++;
    }
    return counter;
  }
}

void queue_append(queue_t **queue, queue_t *elem)
{
  if(elem == NULL)
  {
    printf("NULL element can not be insert in queue!\n");
    return;
  }
  if((*elem).next != NULL || (*elem).prev != NULL)
  {
    printf("Element is already queued!\n");
    return;
  }

  if(queue_size(*queue) == 0)
  {
    *queue = elem;
    (**queue).prev = elem;
    (**queue).next = elem;
  }
  else
  {
    queue_t *last = NULL;
    last = (**queue).prev;
    (*last).next = elem;
    (*elem).next = *queue;
    (*elem).prev = last;
    (**queue).prev = elem;
  }
}

queue_t *queue_remove(queue_t **queue, queue_t *elem)
{
  int size = queue_size(*queue);
  queue_t *aux = NULL;
  aux = *queue;

  if(size == 0)
  {
    printf("No such element to remove from an empty queue!\n");
    return NULL;
  }
  else if(size == 1 && aux == elem )
  {
    *queue = NULL;
    (*aux).next = NULL;
    (*aux).prev = NULL;
    return aux;
  }
  else
  {
      queue_t *last = NULL;
      last = (*aux).prev;
      do {
        if(aux == elem)
        {
          queue_t *after, *before = NULL;
          after = (*aux).next;
          before = (*aux).prev;
          (*after).prev = before;
          (*before).next = after;

          if(elem == *queue)
          {
            *queue = (*aux).next;
          }

          (*aux).next = NULL;
          (*aux).prev = NULL;
          return aux;
        }
        else
        {
            aux = (*aux).next;
        }
      } while(aux != *queue);
      printf("The element is not queued!\n");
      return NULL;
  }
}

void queue_print (char *name, queue_t *queue, void print_elem (void*))
{
  printf("%s: ", name);
  if(queue_size(queue) == 0)
  {
    printf("[]\n");
    return;
  }
  queue_t *first = NULL;
  first = queue;
  printf("[");
  do {
    print_elem(queue);
    if((*queue).next != first)
      printf(" ");
    queue = (*queue).next;
  } while(queue != first);
  printf("]\n");
}
