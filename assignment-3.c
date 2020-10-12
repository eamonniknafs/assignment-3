/**
 * Assignment #3: Strings, structs, pointers, command-line arguments.
 *  Let's use our knowledge to write a simple flight management system!
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// Limit constants
#define MAX_CITY_NAME_LEN 20
#define MAX_FLIGHTS_PER_CITY 5
#define MAX_DEFAULT_SCHEDULES 50

// Time definitions
#define TIME_MIN 0
#define TIME_MAX ((60 * 24)-1)   
#define TIME_NULL -1


/******************************************************************************
 * Structure and Type definitions                                             *
 ******************************************************************************/
typedef int time_t;                        // integers used for time values
typedef char city_t[MAX_CITY_NAME_LEN+1];; // null terminate fixed length city
 
// Structure to hold all the information for a single flight
//   A city's schedule has an array of these
struct flight {
  time_t time;       // departure time of the flight
  int available;  // number of seats currently available on the flight
  int capacity;   // maximum seat capacity of the flight
};

// Structure for an individual flight schedule
// The main data structure of the program is an Array of these structures
// Each structure will be placed on one of two linked lists:
//                free or active
// Initially the active list will be empty and all the schedules
// will be on the free list.  Adding a schedule is finding the first
// free schedule on the free list, removing it from the free list,
// setting its destination city and putting it on the active list
struct flight_schedule {
  city_t destination;                          // destination city name
  struct flight flights[MAX_FLIGHTS_PER_CITY]; // array of flights to the city
  struct flight_schedule *next;                // link list next pointer
  struct flight_schedule *prev;                // link list prev pointer
};

/******************************************************************************
 * Global / External variables                                                *
 ******************************************************************************/
// This program uses two global linked lists of Schedules.  See comments
// of struct flight_schedule above for details
struct flight_schedule *flight_schedules_free = NULL;
struct flight_schedule *flight_schedules_active = NULL;


/******************************************************************************
 * Function Prototypes                                                        *
 ******************************************************************************/
// Misc utility io functions
int city_read(city_t city);           
bool time_get(time_t *time_ptr);      
bool flight_capacity_get(int *capacity_ptr);
void print_command_help(void);

// Core functions of the program
void flight_schedule_initialize(struct flight_schedule array[], int n);
struct flight_schedule * flight_schedule_find(city_t city);
struct flight_schedule * flight_schedule_allocate(void);
void flight_schedule_free(struct flight_schedule *fs);
void flight_schedule_add(city_t city);
void flight_schedule_listAll(void);
void flight_schedule_list(city_t city);
void flight_schedule_add_flight(city_t city);
void flight_schedule_remove_flight(city_t city);
void flight_schedule_schedule_seat(city_t city);
void flight_schedule_unschedule_seat(city_t city);
void flight_schedule_remove(city_t city);

void flight_schedule_sort_flights_by_time(struct flight_schedule *fs);
int  flight_compare_time(const void *a, const void *b);


int main(int argc, char *argv[]) 
{
  long n = MAX_DEFAULT_SCHEDULES;
  char command;
  city_t city;

  if (argc > 1) {
    // If the program was passed an argument then try and convert the first
    // argument in the a number that will override the default max number
    // of schedule we will support
    char *end;
    n = strtol(argv[1], &end, 10); // CPAMA p 787
    if (n==0) {
      printf("ERROR: Bad number of default max scedules specified.\n");
      exit(EXIT_FAILURE);
    }
  }

  // C99 lets us allocate an array of a fixed length as a local 
  // variable.  Since we are doing this in main and a call to main will be 
  // active for the entire time of the program's execution this
  // array will be alive for the entire time -- its memory and values
  // will be stable for the entire program execution.
  struct flight_schedule flight_schedules[n];
 
  // Initialize our global lists of free and active schedules using
  // the elements of the flight_schedules array
  flight_schedule_initialize(flight_schedules, n);

  // DEFENSIVE PROGRAMMING:  Write code that avoids bad things from happening.
  //  When possible, if we know that some particular thing should have happened
  //  we think of that as an assertion and write code to test them.
  // Use the assert function (CPAMA p749) to be sure the initilization has set
  // the free list to a non-null value and the the active list is a null value.
  assert(flight_schedules_free != NULL && flight_schedules_active == NULL);

  // Print the instruction in the beginning
  print_command_help();

  // Command processing loop
  while (scanf(" %c", &command) == 1) {
    switch (command) {
    case 'A': 
      //  Add an active flight schedule for a new city eg "A Toronto\n"
      city_read(city);
      flight_schedule_add(city);

      break;
    case 'L':
      // List all active flight schedules eg. "L\n"
      flight_schedule_listAll();
      break;
    case 'l': 
      // List the flights for a particular city eg. "l\n"
      city_read(city);
      flight_schedule_list(city);
      break;
    case 'a':
      // Adds a flight for a particular city "a Toronto\n
      //                                      360 100\n"
      city_read(city);
      flight_schedule_add_flight(city);
      break;
    case 'r':
      // Remove a flight for a particular city "r Toronto\n
      //                                        360\n"
      city_read(city);
      flight_schedule_remove_flight(city);
	break;
    case 's':
      // schedule a seat on a flight for a particular city "s Toronto\n
      //                                                    300\n"
      city_read(city);
      flight_schedule_schedule_seat(city);
      break;
    case 'u':
      // unschedule a seat on a flight for a particular city "u Toronto\n
      //                                                      360\n"
        city_read(city);
        flight_schedule_unschedule_seat(city);
        break;
    case 'R':
      // remove the schedule for a particular city "R Toronto\n"
      city_read(city);
      flight_schedule_remove(city);  
      break;
    case 'h':
        print_command_help();
        break;
    case 'q':
      goto done;
    default:
      printf("Bad command. Use h to see help.\n");
    }
  }
 done:
  return EXIT_SUCCESS;
}

/**********************************************************************
 * city_read: Takes in and processes a given city following a command *
 *********************************************************************/
int city_read(city_t city) {
  int ch, i=0;

  // skip leading non letter characters
  while (true) {
    ch = getchar();
    if ((ch >= 'A' && ch <= 'Z') || (ch >='a' && ch <='z')) {
      city[i++] = ch;
      break;
    }
  }
  while ((ch = getchar()) != '\n') {
    if (i < MAX_CITY_NAME_LEN) {
      city[i++] = ch;
    }
  }
  city[i] = '\0';
  return i;
}


/****************************************************************
 * Message functions so that your messages match what we expect *
 ****************************************************************/
void msg_city_bad(char *city) {
  printf("No schedule for %s\n", city);
}

void msg_city_exists(char *city) {
  printf("There is a schedule of %s already.\n", city);
}

void msg_schedule_no_free(void) {
  printf("Sorry no more free schedules.\n");
}

void msg_city_flights(char *city) {
  printf("The flights for %s are:", city);
}

void msg_flight_info(int time, int avail, int capacity) {
  printf(" (%d, %d, %d)", time, avail, capacity);
}

void msg_city_max_flights_reached(char *city) {
  printf("Sorry we cannot add more flights on this city.\n");
}

void msg_flight_bad_time(void) {
  printf("Sorry there's no flight scheduled on this time.\n");
}

void msg_flight_no_seats(void) {
    printf("Sorry there's no more seats available!\n");
}

void msg_flight_all_seats_empty(void) {
  printf("All the seats on this flights are empty!\n");
}

void msg_time_bad() {
  printf("Invalid time value\n");
}

void msg_capacity_bad() {
  printf("Invalid capacity value\n");
}

void print_command_help()
{
  printf("Here are the possible commands:\n"
	 "A <city name>     - Add an active empty flight schedule for\n"
	 "                    <city name>\n"
	 "L                 - List cities which have an active schedule\n"
	 "l <city name>     - List the flights for <city name>\n"
	 "a <city name>\n"
         "<time> <capacity> - Add a flight for <city name> @ <time> time\n"
	 "                    with <capacity> seats\n"  
	 "r <city name>\n"
         "<time>            - Remove a flight form <city name> whose time is\n"
	 "                    <time>\n"
	 "s <city name>\n"
	 "<time>            - Attempt to schedule seat on flight to \n"
	 "                    <city name> at <time> or next closest time on\n"
	 "                    which their is an available seat\n"
	 "u <city name>\n"
	 "<time>            - unschedule a seat from flight to <city name>\n"
	 "                    at <time>\n"
	 "R <city name>     - Remove schedule for <city name>\n"
	 "h                 - print this help message\n"
	 "q                 - quit\n"
);
}


/****************************************************************
 * Resets a flight schedule                                     *
 ****************************************************************/
void flight_schedule_reset(struct flight_schedule *fs) {
    fs->destination[0] = 0;
    for (int i=0; i<MAX_FLIGHTS_PER_CITY; i++) {
      fs->flights[i].time = TIME_NULL;
      fs->flights[i].available = 0;
      fs->flights[i].capacity = 0;
    }
    fs->next = NULL;
    fs->prev = NULL;
}

/******************************************************************
* Initializes the flight_schedule array that will hold any flight *
* schedules created by the user. This is called in main for you.  *
 *****************************************************************/

void flight_schedule_initialize(struct flight_schedule array[], int n)
{
  flight_schedules_active = NULL;
  flight_schedules_free = NULL;

  // takes care of empty array case
  if (n==0) return;

  // Loop through the Array connecting them
  // as a linear doubly linked list
  array[0].prev = NULL;
  for (int i=0; i<n-1; i++) {
    flight_schedule_reset(&array[i]);
    array[i].next = &array[i+1];
    array[i+1].prev = &array[i];
  }
  // Takes care of last node.  
  flight_schedule_reset(&array[n-1]); // reset clears all fields
  array[n-1].next = NULL;
  array[n-1].prev = &array[n-2];
  flight_schedules_free = &array[0];
}

/***********************************************************
 * time_get: read a time from the user
   Time in this program is a minute number 0-((24*60)-1)=1439
   -1 is used to indicate the NULL empty time 
   This function should read in a time value and check its 
   validity.  If it is not valid eg. not -1 or not 0-1439
   It should print "Invalid Time" and return false.
   othewise it should return the value in the integer pointed
   to by time_ptr.
 ***********************************************************/
bool time_get(int *time_ptr) {
  if (scanf("%d", time_ptr)==1) {
    return (TIME_NULL == *time_ptr || 
	    (*time_ptr >= TIME_MIN && *time_ptr <= TIME_MAX));
  } 
  msg_time_bad();
  return false;
}

/***********************************************************
 * flight_capacity_get: read the capacity of a flight from the user
   This function should read in a capacity value and check its 
   validity.  If it is not greater than 0, it should print 
   "Invalid capacity value" and return false. Othewise it should 
   return the value in the integer pointed to by cap_ptr.
 ***********************************************************/
bool flight_capacity_get(int *cap_ptr) {
  if (scanf("%d", cap_ptr)==1) {
    return *cap_ptr > 0;
  }
  msg_capacity_bad();
  return false;
}

void flight_schedule_sort_flights_by_time(struct flight_schedule *fs) 
{
  qsort(fs->flights, MAX_FLIGHTS_PER_CITY, sizeof(struct flight),
	flight_compare_time);
}

int flight_compare_time(const void *a, const void *b) 
{
  const struct flight *af = a;
  const struct flight *bf = b;
  
  return (af->time - bf->time);
}

/***********************************************************
 * flight_schedule_allocate:
   – Takes a schedule off the free list and places it on the active list
   – Returns the updated flight_schedule.
   – This should be used as a helper function for flight_schedule_add
 ***********************************************************/
struct flight_schedule * flight_schedule_allocate(){
  struct flight_schedule *fsa_next = flight_schedules_active; //Keeps pointer for next node (in active list)
  flight_schedules_active = flight_schedules_free; 
  flight_schedules_free = flight_schedules_free->next;
  flight_schedules_active->next = fsa_next;
  flight_schedules_active->prev = NULL;
  if (fsa_next != 0) fsa_next->prev = flight_schedules_active;
  flight_schedules_free->prev = NULL;
  return flight_schedules_active;
}

/***********************************************************
 * flight_schedule_free():
   – Takes as input a flight_schedule struct.
   – Takes a schedule off the active list, resets it, and places the node back on the free list.
   – Does not return anything.
   – This should be used as a helper function for flight_schedule_remove
 ***********************************************************/
void flight_schedule_free(struct flight_schedule *fs){
  flight_schedule_reset(fs);
  flight_schedules_free->prev = fs;
  fs->next = flight_schedules_free;
  flight_schedules_free = fs;
}

/***********************************************************
 * flight_schedule_add():
   – Takes as input a city and adds a flight schedule for this given city.
   – Does not return anything.
   – Command line syntax: ”A Toronto\n”
 ***********************************************************/
void flight_schedule_add(city_t city){
  struct flight_schedule *fs = flight_schedule_allocate();
  strcpy(fs->destination, city);
}

/***********************************************************
 * flight_schedule_remove():
   – Takes as input a city and removes the flight schedule for this city, if it exists.
   – Does not return anything.
   – Command line syntax: ”R Toronto\n”
 ***********************************************************/
void flight_schedule_remove(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  while (trav != NULL){
    if (*trav->destination == *city) { //find the right node, them removes references
      if (trav->next != NULL && trav->prev != NULL){ //accounts for next & prev pointers on node being removed
        trav->prev->next = trav->next;
        trav->next->prev = trav->prev;
      }
      else if (trav->prev != NULL) trav->prev->next = NULL;
      else if (trav->next != NULL) flight_schedules_free = trav->next;
      else flight_schedules_free = NULL;
      flight_schedule_reset(trav); //Resets node being removed
      trav->next = flight_schedules_free; //Places node back on free list
      flight_schedules_free = trav;
    }
    trav = trav->next;
  }
}

/***********************************************************
 * flight_schedule_listAll():
   – Lists all of the existing flight schedules.
   – Command line syntax: ”L\n”
 ***********************************************************/
void flight_schedule_listAll(){
  struct flight_schedule *trav = flight_schedules_active;
  while (trav != NULL){ //finds all non-NULL nodes
    printf("%s\n", trav->destination);
    trav = trav->next;
  }
}
 
/***********************************************************
 * flight_schedule_list():
   – Lists all of the flights of a given city
   – Command line syntax: ”l Toronto\n”
 ***********************************************************/
void flight_schedule_list(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  while (trav != NULL){
    if (*trav->destination == *city){ //find the right node (destination city)
      printf("%s", "The flights for ");
      printf("%s", trav->destination);
      printf("%s", " are:");
      for (int i = 0; trav->flights[i].time != TIME_NULL; i++){ //find the flight(s)
        printf("%s", " (");
        printf("%i", trav->flights[i].time);
        printf("%s", ", ");
        printf("%i", trav->flights[i].available);
        printf("%s", ", ");
        printf("%i", trav->flights[i].capacity);
        printf("%s", ")");
      }
    }
    trav = trav->next;
  }
  printf("\n");
}

/***********************************************************
 * flight_schedule_add_flight():
   – Takes as input a city and adds a given flight for this city.
   – This function should call time_get and flight_capacity_get to take in the time and capacity for the newly added flight.
   – Does not return anything.
   – Command line syntax: ”a Toronto\n 360 100\n”
 ***********************************************************/
void flight_schedule_add_flight(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  while (trav != NULL){
    if (*trav->destination == *city) { //find the right node (destination city)
      int idx = 0;
      while (trav->flights[idx].time != TIME_NULL){ //find an empty slot in that node's flights[] array
        idx++;
        if (idx >= MAX_FLIGHTS_PER_CITY) break;
      }
      time_get(&trav->flights[idx].time); //assign the values
      flight_capacity_get(&trav->flights[idx].capacity);
      trav->flights[idx].available = trav->flights[idx].capacity;
    }
    trav = trav->next;
  }
}

/***********************************************************
 * flight_schedule_remove_flight():
   – Takes as input a city and removes the given flight for this city.
   – This function should call time get to determine which flight should be removed.
   – Does not return anything.
   – Command line syntax: ”r Toronto\n 360 '\n'”
 ***********************************************************/
void flight_schedule_remove_flight(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  time_t in;
  while (trav != NULL){
    if (*trav->destination == *city){ //find the right node (destination city)
      time_get(&in);
      int idx = 0;
      while (trav->flights[idx].time != in){ //find the correct flight in that node's flights[] array
        idx++;
        if (idx >= MAX_FLIGHTS_PER_CITY) break;
      }
      if (trav->flights[idx].time == in) {
        trav->flights[idx].available = 0;
        trav->flights[idx].capacity = 0;
        trav->flights[idx].time = TIME_NULL;
      }
    }
    trav = trav->next;
  }
}

/***********************************************************
 * flight_schedule_schedule_seat():
   – Takes as input a city and schedules a seat on a flight for this city.
   – The user can specify any time and your program should schedule the next available flight from the given time.
   – Be sure to adjust the available seats accordingly.
   – Does not return anything.
   – Command line syntax: ”s Toronto\n 340 '\n'”
 ***********************************************************/
void flight_schedule_schedule_seat(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  time_t in;
  while (trav != NULL){
    if (*trav->destination == *city){ //find the right node (destination city)
      time_get(&in);
      int idx = -1;
      for (int temp = 0; temp < MAX_FLIGHTS_PER_CITY; temp++){ //find the correct flight in that node's flights[] array
        if (idx == -1){
          if (trav->flights[temp].time >= in) idx = temp;
        }
        else if (trav->flights[temp].time >= in && trav->flights[temp].time < trav->flights[idx].time) idx = temp;
      }
      if (idx != -1 && trav->flights[idx].available>0) trav->flights[idx].available--; //subtracts from number of available seats
    }
    trav = trav->next;
  }
}

/***********************************************************
 * flight_schedule_unschedule_seat():
   – Takes as input a city and unschedules a seat on a given flight for this city.
   – The user must specify the exact time for the flight that they are unscheduling. 
   – Be sure to adjust the available seats accordingly.
   – Does not return anything.
   – Command line syntax: ”u Toronto\n 360 '\n'”
 ***********************************************************/
void flight_schedule_unschedule_seat(city_t city){
  struct flight_schedule *trav = flight_schedules_active;
  time_t in;
  while (trav != NULL){
    if (strcmp(trav->destination,city) == 0){ //find the right node (destination city)
      time_get(&in);
      int idx = 0;
      while (trav->flights[idx].time != in){ //find the correct flight in that node's flights[] array
        idx++;
        if (idx >= MAX_FLIGHTS_PER_CITY) break;
      }
      if (trav->flights[idx].available<trav->flights[idx].capacity) trav->flights[idx].available++; //adds to the number of available seats
    }
    trav = trav->next;
  }
}

//TODO: flight_schedule_find
/***********************************************************
 * flight_schedule_find():
   – Takes as input a city and traverses the active flight list until it finds the flight schedule for this city, if it exists
   – Returns the flight schedule of said city.
   – While not necessary, this function will prove very useful in implementing the other functions.
 ***********************************************************/
 struct flight_schedule * flight_schedule_find(city_t city){
   return 0;
 }