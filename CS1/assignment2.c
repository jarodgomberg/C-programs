/* COP 3502C Assignment 2
This program is written by: Jarod Gomberg */

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define NUMLINES 12 //for checkout lines
int current_time; //for current time

//customer info struct
typedef char cname[9]; //1-9 uppercase letters
typedef struct customer{
  cname name;
  int num_items;
  int num_line;
  int time;
} customer;

//node struct with pointer to data from customer
typedef customer *shopper;
typedef struct node{
  shopper data;
  struct node *next;
  struct node *prev;
} node;

//queue struct pointer to rear and front
typedef struct Queue{
  node *rear;
  node *front;
  int size;
} Queue;
Queue list[NUMLINES]; //for list of queues

void init(Queue *q); //initialize's queue by setting front and rear to null as well as size to 0
void enqueue(Queue *q, shopper data); //add element to rear of queue
shopper dequeue(Queue *q); //takes and returns element from front of queue
shopper Peek(Queue *q); //returns element at front of queue without dequeue
void checkout(customer *shopper); //gets customer checkout time and prints checkout info
void display_customer(FILE *fp);     //read in file and get all customer information. Process the information using
                                   //getCustomer() and checkout() functions, then display that information

int main()
{
  FILE *fp = fopen("assignment2input.txt", "r"); //open and read file
  int num_cases;

  fscanf(fp, "%d", &num_cases); //scan number of cases iterate through
  for(int i = 0; i < num_cases; ++i)
  {
    display_customer(fp); //process and display all customer and checkout information
  }
  fclose(fp); //close file
  return 0;
}



void init(Queue *q)
{
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
}

void enqueue(Queue *q, shopper data)
{
  node *new; //new variable for added element
  node *prev;

  new = malloc(sizeof(node)); //allocate memory for new element
  new->next = NULL;
  new->data = data;

  prev = q->rear;
  prev = NULL;
  for(node *curnode = q->front; curnode != NULL; curnode = curnode->next) //get rear of queue using pointer to front
  {
    prev = curnode;
  }
  if(prev != NULL)
  {
    prev->next = new;
  }
  else
  {
    q->front = new;
  }
  new->prev = prev;
  q->rear = new;
  q->size += 1; //increase size of queue
}

shopper dequeue(Queue *q)
{
  //create pointers for current and next node
  node *current;
  node *nextnode;
  shopper data; //data pointer for customer/shopper
  current = q->front; //get node in front
  if(current == NULL) //if that node is null, data is null
  {
    data = NULL;
  }
  data = current->data; //get current data
  nextnode = current->next; //continue pointing to next node

  node *rear = q->rear;
  current->prev = rear;

  if(current == rear) //if current node is the rear
  {
    q->rear = nextnode;
  }
  q->front = nextnode;
  q->size -= 1; //decrease size of queue from taken element
  free(current); //free current node
  return data;
}

shopper Peek(Queue *q) //returns element at front of queue without dequeue
{
  node *curnode;
  shopper data;
  curnode = q->front;
  if(curnode != NULL) //current node is not null
  {
    data = curnode->data; //get current node data
  }
  else
  {
    data = NULL;
  }
  return data; //return data from current node
}

int empty(Queue *q) //determine if queue is empty
{
  if(q->front == q->rear)
  {
    return 1; //returns 1 if empty
  }
  else
  {
    return 0; //0 if not empty
  }
}

customer *getCustomer()
{
  int n; //to see all lines
  int qidx;
  Queue *q;
  int min; //minimun time
  int remaining; //customers left to checkout
  customer *shopper;
  customer *first; //for customer with fastest checkout time

  for(n = 1; n <= 2; ++n) //for passing through lines
  {
    first = NULL;
    min = 100000;
    remaining = 0;

    for(qidx = 0; qidx < NUMLINES; ++qidx) //go through queues and get first customer from each line
    {
      q = &list[qidx];

      shopper = Peek(q); //on current line for front customer
      if(shopper == NULL)
      {
        continue;
      }
      remaining = 1; //remaining customer is 1 or more

      if(shopper->time < min) //keep track of min time
      {
        min = shopper->time;
      }

      if(shopper->time > current_time) //customer at checkout or not
      {
        continue;
      }

      if(first == NULL) //keep track of best time shopper
      {
        first = shopper;
        continue;
      }

      if(shopper->num_items < first->num_items) //get new customer with better time
      {
        first = shopper;
        continue;
      }
    }

    if(!remaining) //if all customers are done
    {
      break;
    }

    if(first != NULL) //found customer in first
    {
      q = &list[first->num_line - 1];
      dequeue(q); //dequeue from line
      break;
    }
    current_time = min; //set time to minimum of customers left
  }
  return first; //return best customer
}

void checkout(customer *shopper)
{
  int passed; //time passed
  if(current_time < shopper->time) //current_time is time they enter line
  {
    current_time = shopper->time;
  }

  passed = 0; //add up time it takes to help current customer
  passed += 30;
  passed += shopper->num_items * 5;

  current_time += passed; //set time to the time it took to checkout

  printf("%s from line %d checks out at time %d.\n", shopper->name, shopper->num_line, current_time);
  free(shopper); //free the shopper data once checked out
}

void display_customer(FILE *fp)
{
  Queue *qptr;
  customer *shopper;
  int i;
  int num_customers;
  fscanf(fp, "%d", &num_customers); //get number of customers

  for(i = 0; i < num_customers; ++i) //get all customer data
  {
    shopper = malloc(sizeof(*shopper));
    fscanf(fp," %d", &shopper->time);
    fscanf(fp," %d", &shopper->num_line);
    fscanf(fp," %s", shopper->name);
    fscanf(fp," %d", &shopper->num_items);

    qptr = &list[shopper->num_line - 1];
    enqueue(qptr, shopper); //place customer in correct spot in queue
  }

  current_time = -1; //set time to past time

  while(1) //loop until no more customers
  {
    shopper = getCustomer(); //get customer info

    if(shopper == NULL)
    {
      break; //break if all customers helped
    }
    checkout(shopper); //checkout customer
  }
}
