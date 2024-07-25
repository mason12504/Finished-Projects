// Project 3 Implimentation
// Mason Andersen
// CS 2060
// 4-25-2024
// 
// ** WRITTEN USING WINDOWS 10 OPERATING SYSTEM **
//
// This program allows an admin to create services for multiple rideshare companies, and take rides from users
// An administrator enters cost parameters, and can see a daily summary of the rides and ratings
// Riders are able to select companies, view immediate ride details, and give ratings of the ride experience
// Each rideshare has its own parameters and surveys, and a unique summary printed to an output file

#include <stdio.h>   // input output
#include <stdlib.h>  // for rand
#include <time.h>    // for srand seed
#include <stdbool.h> // for boolean variable in input validation
#include <string.h>  // strcmp etc
#include <math.h>	// used in getAlphabeticalRank

// change to whatever it is on your pc, I included last backslashes here in the #define
#define FILE_PATH "D:\\CS2060 github repository\\CS2060ClassCode\\CS2060 Projects\\"

// define constants
#define MIN_RAND_MINUTES_FACTOR 1.2		// min bounds for random generator
#define MAX_RAND_MINUTES_FACTOR 1.5		// max bounds for random generator
#define STRING_LENGTH 80				// max string length

// survey rows/columns for 2d array
#define SURVEY_RIDER_ROWS 5
#define SURVEY_CATEGORIES 3

//login info
#define CORRECT_ID "1"
#define CORRECT_PASSCODE "1"
#define LOGIN_MAX_ATTEMPTS 3			
#define SENTINEL_VALUE -1				// value that ends the program

// admin info validation min/max, shared for all the admin info numbers
#define MIN_SET_UP 0.1
#define MAX_SET_UP 50

// define structure for storing rideshare data
// above function prototypes since they are using it as a type in args
typedef struct rideShareOrg {

	// name of the company
	char orgName[STRING_LENGTH];

	// fare calculation parameters
	double baseFare;
	double costPerMinute;
	double costPerMile;
	double minFlatRate;

	// rating arrays
	double ratingAverages[SURVEY_CATEGORIES];
	int ratingsArr[SURVEY_RIDER_ROWS][SURVEY_CATEGORIES];

	// to track how many surveys in array
	int surveyCount;

	// declare running total variables, for summmary later
	double totalMiles;
	int totalMinutes;
	double totalFares;
	int riderCount;

	struct rideShareOrg* nextRideSharePtr;


} RideShareOrg;
// end rideshare struct

// basic input functions without min/max range validation
bool getInt(int* result);
void stringNoNewLine(char* stringReturn, int length);
bool getDouble(double* result);

// valid input functions
double getValidDouble(double min, double max, double sentinel);
double getValidDoublePrompt(double min, double max, char* prompt);
int getRatingPrompt(int min, int max, char* prompt);
bool getYesNo(void);
void getRatings(RideShareOrg* rideshare);

// print/display functions
void printFare(int count, double miles, int minutes, double fare);
void printTotals(RideShareOrg* rideshare);
void printCategories(const char* categories[]);
void displayAverages(const double avgArr[]);
void displayRatings(RideShareOrg * rideshare);

// file printing functions
void writeFileCategories(const char* surveyCategories[], FILE* filePtr);
void writeFileAverages(const double avgArr[], FILE* filePtr);
void writeInfoFile(RideShareOrg* headPtr);

// linked list related
void displayAllRatings(RideShareOrg* headPtr);
RideShareOrg* selectRideShare(RideShareOrg* headPtr);
void deAllocateList(RideShareOrg* headPtr);

// array for the survey categories 
const char* surveyCategories[SURVEY_CATEGORIES] = { "Safety", "Cleanliness", "Comfort"};

// setup functions
void adminSetup(RideShareOrg* rideshare);
bool adminLogin(char pass[STRING_LENGTH], char id[STRING_LENGTH]);
void createRideShareList(RideShareOrg** headptr);
void addRideShareList(RideShareOrg** headPtr);

// calculation functions
int getRandTime(double miles);
double calculateFare(const RideShareOrg* rideshare, double miles, int minutes);
void calcAverages(RideShareOrg* rideshare);
int runRiderMode(RideShareOrg* rideshare);
double getAlphabeticalRank(char* word);
void replaceSpaces(char* name);
void matchCase(char* word);

// just runs the specialized functions in the right order, handles initial login condition
int main(void) {

	// declare linked list head pointer
	RideShareOrg* rideShareListHead = NULL;

	RideShareOrg * rideshare1 = NULL;			// initialize later selected rideshare pointer

	// begin program with admin mode to enter data
	if (adminLogin(CORRECT_ID, CORRECT_PASSCODE) == true) {
		
		createRideShareList(&rideShareListHead);
		puts("");

		// repeat ridermode for rideshares as entered by the user
		do {
			// print out ratings for all rideshares in system, then allow a user to select one
			displayAllRatings(rideShareListHead);
			rideshare1 = selectRideShare(rideShareListHead);
			
		// run rider mode with selected rideshare until sentinel entered
		}while(runRiderMode(rideshare1) != 0);			// ridermode will return 0 on admin shutdown
		
		// admin shutdown: write the business summary to files
		writeInfoFile(rideShareListHead);

		// de allocate all the memory used by the linked list at the end
		deAllocateList(rideShareListHead);

	}
	
	//otherwise initial admin login failed, so just exit
	else {
		printf("Too many invalid login attempts: Exiting program. ");
	}
	
	
	return 0;
}// end main


/*	Initiates the main rider-side functionality of the program
*	Parameter: rideshare, the structure with all relevant information to the rideshare organization
*	Return: 1 to keep looping, or 0 when sentinel entered for admin shutdown
*   
*/
int runRiderMode(RideShareOrg* rideshare) {

	// relevant variables for output
	
	double miles = 0;		// variable to store user input
	int minutes = 0;		// randomly calculated minutes
	double finalFare = 0;	// final fare that the rider has to pay

	// minMiles and maxMiles for input validation
	int minMiles = 1;
	int maxMiles = 100;

	// get the company name in a more convenient variable
	char name[STRING_LENGTH] = " ";
	strcpy(name, rideshare->orgName);
	bool returnVal = true;

	printf("Welcome to %s. We can only provide services for rides from %d to %d miles. \n", name, minMiles, maxMiles);

	miles = getValidDouble(minMiles, maxMiles, SENTINEL_VALUE);

	// while miles isnt the sentinel, run as normal
	// ride accepted
	if (miles != SENTINEL_VALUE) {

		// increment rider number each loop
		rideshare->riderCount++;

		// calculate minutes using random estimate formula
		minutes = getRandTime(miles);

		// plug everything into the fare formula
		finalFare = calculateFare(rideshare, miles, minutes);

		// print the user's basic ride summary
		printf("%s", "Current Ride Information\n\n");
		printFare(rideshare->riderCount, miles, minutes, finalFare);

		// update running totals for business summary
		rideshare->totalMinutes = rideshare->totalMinutes + minutes;
		rideshare->totalFares = rideshare->totalFares + finalFare;
		rideshare->totalMiles = rideshare->totalMiles + miles;

		printf("Would you like to rate your rideshare experience? ");

		if (getYesNo() == true) {

			// if user enters y/Y, increment surveyCount and store their ratings
			rideshare->surveyCount++;
			getRatings(rideshare);

		}

			printf("%s", "\nThanks for riding with us. \n\n");

		}
	// when sentinel is entered, check admin login
	else if (adminLogin(CORRECT_ID, CORRECT_PASSCODE) == true) {

		// With successful login, program exits loop.
		returnVal = false;
		printf("%s", "\nAdmin shutdown. Exiting RideShare Mode\n");
		printf("%s Ride Share Business Summary \n\n", name);

	// output no rides if no rides, otherwise give the summary
	if (rideshare->riderCount == 0) {
		printf("%s", "There were no rides.");
	}

	// if surveys entered, calculate and output the averages
	else if (rideshare->surveyCount > 0){
		// summary with totals
		printTotals(rideshare);

		// summary of average ratings
		calcAverages(rideshare);
		displayAverages(rideshare->ratingAverages);
	}
	// if no ratings but still processed riders, just output the summary
	else {
		// summary with totals
		printTotals(rideshare);

		printf("%s", "There were no ratings.");
	}

	}// end if/else

	// true if sentinel wasnt entered
	return returnVal;
}// end rider mode function

/* Validates admin credentials for admin mode
* 
* Parameters:
* correctId (string) the correct ID the admin should enter to login
* correctPass (string) the correct password the admin should enter to login
* 
* Return:
* true or false based on whether admin credentials entered matched the correct ones.
*/
bool adminLogin(char correctId[STRING_LENGTH], char correctPass[STRING_LENGTH]) {

	bool returnval = false;
	int attemptsMax = LOGIN_MAX_ATTEMPTS;
	int attempts = 0;

	// variables for the input to be compared against correct ones
	// user input
	char id[STRING_LENGTH] = "";
	char pass[STRING_LENGTH] = "";

	while (returnval == false && attempts < attemptsMax)  {

		// prompt and get input with stringNoNewLine to get a string without newline
		printf("%s", "Enter admin ID: ");
		stringNoNewLine(id, STRING_LENGTH);

		printf("%s", "Enter admin Passcode: ");
		stringNoNewLine(pass, STRING_LENGTH);

		// compare the input to correct values
		int idValidity = strcmp(id, correctId);
		int passValidity = strcmp(pass, correctPass);

		// if both id and password are correct, login
		// 0 since strcmp returns 0 for match
		if (idValidity == 0 && passValidity == 0) {
			returnval = true;
		}
		else {
			printf("%s", "Error: incorrect ID or Passcode. \n");
		}

		attempts++;
	}
	

	// otherwise login failed, return false
	return returnval;
}

/*
*  Gets a string from keyboard input and removes the newline
*  Parameters:
*  stringReturn string to store result
*  maxLength the length of the string for fgets
*  Returns:
*  none, but stores string in provided address pointer
*/
void stringNoNewLine(char * stringReturn, int maxLength) {
	fgets(stringReturn, maxLength, stdin);

	// length to find the end
	size_t length = strlen(stringReturn);
	
	// with fgets you always get a newline at the penultimate value.
	// so replace that newline with null character to remove newline and keep the string format
	stringReturn[length - 1] = '\0';

}

/* Initializes the rideShare company structure with their unique data
*  Parameter: rideShareOrg *rideshare, pointer to the struct to store all the data
*  no returns
*/
void adminSetup(RideShareOrg *rideshare) {

	// just to make the following code slightly more readable and compact
	double min = MIN_SET_UP;
	double max = MAX_SET_UP;
	
	printf("\nSet up rideshare information \n\n");
	// for each value, get valid input and provide prompt
	rideshare->baseFare = getValidDoublePrompt(min, max, "base fare");
	rideshare->costPerMinute = getValidDoublePrompt(min, max, "cost per minute");
	rideshare->costPerMile = getValidDoublePrompt(min, max, "cost per mile");
	rideshare->minFlatRate = getValidDoublePrompt(min, max, "minimum flat rate");

	// no validation for the name, so just needs one prompt
	printf("%s", "Enter the organization name: ");
	stringNoNewLine(rideshare->orgName, STRING_LENGTH);

	// show the admin what they entered
	printf("\nSetup complete for %s \n", rideshare->orgName);

	// $ literal since its cost-related and %.2lf is to format as double with 2 decimal precision
	printf("baseFare = $%.2lf \n", rideshare->baseFare);
	printf("costPerMinute = $%.2lf \n", rideshare->costPerMinute);
	printf("costPerMile = $%.2lf \n", rideshare->costPerMile);
	printf("minFlatRate = $%.2lf \n\n", rideshare->minFlatRate);

	// initialize remaining parameters in the struct
	rideshare->surveyCount = 0;
	rideshare->riderCount = 0;
	rideshare->totalMiles = 0;
	rideshare->totalMinutes = 0;
	rideshare->totalFares = 0;

	rideshare->nextRideSharePtr = NULL;


	// here I should have initialized the arrays to 0 but I couldnt figure it out, 
	// and at least so far there are no problems I can see, but later def should fix
}

// repeatedly asks for a rideshare til admin is done, initializes each one in the linked list.
// Parameter RideShareOrg **headPtr - the address of the start of the linked list,
// headpointer may change depending on input so it is given as a double pointer
// no returns
void createRideShareList(RideShareOrg** headptr) {

	// enter rideshares til admin says no more
	do {
		
		addRideShareList(headptr);
		printf("%s", "Would you like to enter another rideshare? y/n ");

	} while (getYesNo() == true);		// y/Y or n/N input
	
}
// Adds a rideshare to the linked list, calls initialization function too
// Parameter: RideShareOrg **headPtr - the address of the start of the rideshare linked list
// headpointer may change so it's given as a double pointer
void addRideShareList(RideShareOrg** headPtr) {

	// create memory on the heap for a rideshare
	RideShareOrg* newRideSharePtr = malloc(sizeof(RideShareOrg));
	char myString[STRING_LENGTH] = "";

	if (newRideSharePtr != NULL) {

		// setup the rideshare, initialize everything 
		adminSetup(newRideSharePtr);

		// set up pointers for tracking position in the linked list
		RideShareOrg* previousPtr = NULL;
		RideShareOrg* currentPtr = *headPtr;

		// when the alphabetical rank of the new rideShare's name is higher than the current one,
		// move it down the list
		while (currentPtr != NULL && getAlphabeticalRank(currentPtr->orgName) <= getAlphabeticalRank(newRideSharePtr->orgName)) {

			// save previous element so the rideshare can be added in the right order
			previousPtr = currentPtr;

			// Go to next element in linked list
			currentPtr = currentPtr->nextRideSharePtr;
		} 

		// This means while above never ran, so new element has lowest alphabetical rank. It goes at the head of the list
		if (previousPtr == NULL) {

			// change address of head on the other stack to make this element the new head
			*headPtr = newRideSharePtr;
		}
		// otherwise the element finds its location from the while loop
		// then gets assigned there
		else {
			
			previousPtr->nextRideSharePtr = newRideSharePtr;
		}
		
		// rideshare data now at correct order in list, make it point to the next value in order
		newRideSharePtr->nextRideSharePtr = currentPtr;
	}
	// add a case if malloc fails the user knows what happened
	else {
		printf("%s", "No memory to create node \n");
	}
}

/* replaces scanf in the getValid functions
*  Parameter: double * result pointer to the variable to store input
*  Return: true if valid double read, false if invalid data
*/
bool getDouble(double* result) {

	// assume valid data before testing for errors
	bool returnval = true;
	double tempResult = 0; 
	char stringReturn[STRING_LENGTH];
	int errnum = 0;

	// set movable pointer for comparison later
	char* doubleEndPtr = stringReturn;

	// get the string from input
	stringNoNewLine(stringReturn, STRING_LENGTH);

	// this will move stringStartPtr if it has found a valid double,
	tempResult = strtod(stringReturn, &doubleEndPtr);

	// if pointer has not moved, there is no double
	if (doubleEndPtr == stringReturn) {
		returnval = false;
	}
	// if pointer has moved, there is a double
	// if double too big, invalid
	if (ERANGE == errnum) {
		returnval = false;
	}
	
	// if characters at the end of the double, invalid
	if (*doubleEndPtr != '\0') {
		returnval = false;
	}

	// if valid up to this point, store the valid data at given address
	if (returnval == true) {
		*result = tempResult;
	}
	// else invalid, returns false below.
	
	// return validity
	return returnval;
}

/*
* validates user input from terminal using min and max bounds, and sentinel for program exit/summary
* Parameters:
* int min - min bounds
* int max - max bounds 
* int sentinel - exit character
* 
* Returns: the valid user input
* 
*/
double getValidDouble(double min, double max, double sentinel) {

	// initialize variables
	double returnval = 0;
	bool getReturn = false;
	bool validity = false;

	// prompt user for input, repeat if data is invalid
	while (validity == false) {
		
		printf("%s", "Enter the number of miles to your destination: "); 

		// save input to returnval, save scanf return value to validate input
		// %lf for double
		getReturn = getDouble(&returnval);
		
		// display error if outside bounds
		if ((getReturn == true) 	// double must be read 
			&& (returnval > max		// error if number too high 
			|| (returnval < min		// error if number too low 
			&& returnval != sentinel))) {	// dont error if sentinel 
			
			printf("Error: Not within %.1lf and %.1lf miles. \n", min, max);
		}
		// error if not a valid decimal number
		else if (getReturn == false) {
				printf("%s", "Error: You did not enter a number. \n");
			}

		// if above error conditions not met, then input is valid, break the loop
		else {
			validity = true;
		}// end if/else

	}// end while
	
	// return the final valid data 
	return returnval;
}

// getValidDoublePrompt no sentinel value, but providing a prompt for the input loop
/*
* validates user input from terminal using min and max bounds
* Parameters:
* double min - min bounds
* double max - max bounds
* char * prompt (string) - what to tell the user to input
*
* Returns: the valid user input
*
*/
double getValidDoublePrompt(double min, double max, char *prompt) {

	// initialize variables
	double returnval = 0;
	bool getReturn = false;
	bool validity = false;

	// prompt user for input, repeat if data is invalid
	while (validity == false) {

		printf("Enter the %s: ", prompt);

		// save input to returnval, save getDouble return value to validate input
		getReturn = getDouble(&returnval);

		// display error if outside bounds
		if ((getReturn == true) 	// double must be read 
			&& (returnval > max		// error if number too high 
			|| (returnval < min))) {	// error if number too low

			printf("Error: Not between %.1lf and %.1lf. \n", min, max);
		}
		// error if not a valid decimal number
		else if (getReturn == false) {
			printf("%s", "Error: You did not enter a number. \n");
		}

		// if above error conditions not met, then input is valid, break the loop
		else {
			validity = true;
		}// end if/else

	}// end while

	// return the final valid data 
	return returnval;
}

/* 
* calculates the fare charge given miles
* 
* Parameters:
* RideShareOrg *rideshare pointer to struct with all relevant cost parameters
*               const, as it will not be changed by the fare calculation
* int minutes - minutes from random generation 
* double miles - ride distance
* 
* Returns: double value of the fare charge
*/ 

double calculateFare(const RideShareOrg *rideshare, double miles, int minutes) {

    // get fare from general formula	
	double finalFare = rideshare->baseFare + (rideshare->costPerMinute * minutes) + (rideshare->costPerMile * miles);

	// use the min flat rate of fare if the calulated fare is too low
	if (finalFare < rideshare->minFlatRate) {
		finalFare = rideshare->minFlatRate;
	}
	// return calculated fare charge
	return finalFare;
}

/*
* Prints ride details for rider, or for the summary, formatted
* 
* Parameters: 
* int count, double miles, int minutes, double fare
* all parameters are for output
* 
* Returns: nothing
*/
void printFare(int count, double miles, int minutes, double fare) {
		
		// \t formats a tab which helps with allignment
		printf("Rider   Number of Miles   Number of Minutes   Ride Fare Amount \n %d  \t %.1lf \t\t %d   \t\t %.2lf \n\n", count, miles, minutes, fare);
	
}

// Prints out a business summary of total riders/miles/minutes/fares accumulated over the program's runtime
// Parameters: 
// RideShareOrg * rideshare - the rideshare structure that stores totals (pointer to it)
void printTotals(RideShareOrg* rideshare) {

	// take info from struct to make the big printf slightly more readable 
	int count = rideshare->riderCount;
	double miles = rideshare->totalMiles;
	int minutes = rideshare->totalMinutes;
	double fare = rideshare->totalFares;

	// \t formats a tab which helps with allignment
	printf("Riders   Number of Miles   Number of Minutes   Ride Fare Amount \n %d  \t %.1lf \t\t %d   \t\t %.2lf \n\n", count, miles, minutes, fare);

}
// Writes to file the business summary totals for one rideshare
// Parameters: 
// RideShareOrg rideshare - the rideshare whose totals are to be written
// FILE * filePtr - the file to write in
//
void filePrintTotals(RideShareOrg* rideshare, FILE * filePtr) {

	// take info from struct to make the big printf slightly more readable 
	int count = rideshare->riderCount;
	double miles = rideshare->totalMiles;
	int minutes = rideshare->totalMinutes;
	double fare = rideshare->totalFares;

	// \t formats a tab which helps with allignment
	fprintf(filePtr, "Riders   Number of Miles   Number of Minutes   Ride Fare Amount \n %d  \t %.1lf \t\t %d   \t\t %.2lf \n\n", count, miles, minutes, fare);

}

/*
* Generates a random number of minutes given a number of miles
* 
* Parameters: double miles
* 
* Returns: the number of minutes 
*/
int getRandTime(double miles) {

	srand(time(0));	// seed random number generator

	// define bounds for generation
	double maxBound = miles * MAX_RAND_MINUTES_FACTOR;
	double minBound = miles * MIN_RAND_MINUTES_FACTOR;
	int range = (int) maxBound - minBound;
	
	// generate the number
	// range+1 to handle very low mile numbers, as this would become a divide by zero error
	int returnval = (rand() % (range+1) + minBound);	

	return returnval;
}

/*  take a y/Y or n/N input from the user for general confirmation checks
*   no Params
*	Return: true (y or Y) or false (n or N)
*   will loop until valid input given
*/
bool getYesNo(void) {

	char input = ' ';
	bool validity = false;		// for loop exit
	bool returnVal = false;

	// loop until valid data entered
	while (validity == false){

		// get one character from input, then clear buffer
		input = getchar();
		while (getchar() != '\n');

		// "no" inputted
		if (input == 'n' || 
			input == 'N' ) {
			validity = true;	// break loop
			returnVal = false;	// set return to false for "no"

		}
		// "yes" inputted
		else if (input == 'y' || 
			     input == 'Y') {
			validity = true;	// break loop
			returnVal = true;	// set return to true for "yes"
		}

		// invalid data will trigger this else
		else {
			printf("%s", "You did not enter a y or n.\n");
			// (keep looping til they enter valid data)
		}

	}
	return returnVal;
}

/* display ratings from the 2d array, only display how many are filled in.
*  Parameters: 
*  RideShareOrg - the struct with all necessary information (rating array, rider count etc)
* 
*  Returns nothing
*/ 
void displayRatings(RideShareOrg* rideshare) {

	int surveyCount = rideshare->surveyCount;

	printf("%s ratings: \n", rideshare->orgName);
	if (surveyCount == 0) {			// if no ratings, say no ratings
		printf("No ratings so far. \n");
	}

	else {		
		// allign output 
		printf("%s", "\t\t");

		// otherwise there are ratings, so print categories
		printCategories(surveyCategories);
		
		// then print one survey per row, with its results for each category
		for (int survey = 0; survey < surveyCount; survey++) {
			printf("Survey %d:", survey+1);
			for (int column = 0; column < SURVEY_CATEGORIES; column++) {
				printf("\t %d", rideshare->ratingsArr[survey][column]);
			}
			// format for the next output
			printf("\n");
		}
	}
}
/* display ratings for every rideshare in linked list
*  Parameters:
*  RideShareOrg * headPtr - the linked list head 
*
*  Returns nothing
*/
void displayAllRatings(RideShareOrg* headPtr) {

	// if the head pointer is actually pointing
	if (headPtr != NULL) {
		printf("%s", "Ratings for each rideshare Organization: \n");
		RideShareOrg* currentPtr = headPtr;			// begin iterating through the list

		// while not at end of list
		while (currentPtr != NULL) {
			displayRatings(currentPtr);
			currentPtr = currentPtr->nextRideSharePtr;		// move to next element
		}
	}
	else {
		printf("Error: List Empty");
	}

}

/*	gets the rating for each category from one user, stores in 2d array in the struct
*	Parameters: 
*	RideShareOrg * rideshare - the rideshare struct with rating arrays
*   Returns nothing
*/
void getRatings(RideShareOrg * rideshare) {
	for (int category = 0; category < SURVEY_CATEGORIES; category++) {
		// surveyCount is incremented before this is called so subtract 1
		rideshare->ratingsArr[rideshare->surveyCount-1][category] = getRatingPrompt(1, 5, surveyCategories[category]);
	}
}

/* basically getValidDouble but for integers, for the getRatings function
*  Parameters:
*  int min, minimum value for input
*  int max, maximum value for input
*  char * prompt (string), the category for which input is being taken
* 
*  Return: 
*  int returnval the valid integer within bounds
*/
int getRatingPrompt(int min, int max, char *prompt) {

	// initialize variables
	int returnval = 0;			// for final return
	bool getReturn = false;
	bool validity = false;

	// prompt user for input, repeat if data is invalid
	while (validity == false) {

		printf("Enter your rating for %s: ", prompt);

		// save input to returnval, save getDouble return value to validate input
		getReturn = getInt(&returnval);


		// display error if outside bounds
		if ((getReturn == true) 	// int must be read 
			&& (returnval > max		// error if number too high 
				|| (returnval < min))) {	// error if number too low

			printf("Error: Not between %d and %d. \n", min, max);
		}
		// error if not a valid decimal number
		else if (getReturn == false) {
			printf("%s", "Error: You did not enter a number. \n");
		}

		// if above error conditions not met, then input is valid, break the loop
		else {
			validity = true;
		}// end if/else

	}// end while

	// return the final valid data 
	return returnval;
}

/*  Takes an integer value from keyboard and stores in given address
*   Parameter: int * result the pointer to the variable which should store the given integer
*   Return: true if integer successfully read, or false if invalid data 
*/
bool getInt(int* result) {
	
	bool returnval = false;
	int tempResult = 0;
	char stringReturn[STRING_LENGTH];

	// set movable pointer for comparison later
	char* stringStartPtr = stringReturn;

	// get the string from input
	fgets(stringReturn, STRING_LENGTH, stdin);

	// using strtol as functionality is almost the same as strtod
	// base 10 since that is all this program will use
	// long is Basically int so it works
	tempResult = strtol(stringReturn, &stringStartPtr, 10);


	// if pointer has moved, valid int found. Store int at pointer provdied
	if (stringStartPtr != stringReturn) {
		returnval = true;
		*result = tempResult;
	}
	// else strtol has not moved the pointer, it has not found a valid int, so dont do anything just return False for bad data

	return returnval;
}

// Prints the categories for user ratings
// Parameter: const char * surveyCategories[] the array of categorey names for output
// no returns
void printCategories(const char* surveyCategories[]) {

	// for every category print the average, \t for formatting
	for (int category = 0; category < SURVEY_CATEGORIES; category++) {
		printf("%s\t", surveyCategories[category]);
	}
	// format so the next output will be readable
	printf("%s", "\n");
}

/* Calculates the average rating for each category in the 2d array
*  Parameters:
*  RideShareOrg *rideshare - the rideshare struct with necessary arrays
*  Returns nothing
*/
void calcAverages(RideShareOrg *rideshare) {

	double totalTemp = 0;
	int surveyCount = rideshare->surveyCount;

	// iterate through the each category of the 2d array
	for (int category = 0; category < SURVEY_CATEGORIES; category++) {

		// sum up the results for each category
		for (int row = 0; row < surveyCount; row++) {
			totalTemp = totalTemp + rideshare->ratingsArr[row][category];
		}
		// calculate the average and assign to array
		rideshare->ratingAverages[category] = totalTemp/surveyCount;			// result stored will be double since totalTemp is double

		// reset temp total for the next category
		totalTemp = 0;
	}
}
/*  Prints the category names and averages of each category to the terminal
*   Parameter:
*   const double avgArr[] the array with averages
*   Returns nothing
*/
void displayAverages(const double avgArr[]) {

	printf("Category Rating Averages \n");
	printCategories(surveyCategories);			// Global constant

	for (int category = 0; category < SURVEY_CATEGORIES; category++) {
		// display each result to 1 decimal precision and \t tab to allign with categories
		printf("%.1lf\t", avgArr[category]);
	}

}
// function for alphabetical sorting
// Parameter word - the string to check ranking
// no returns
double getAlphabeticalRank(char* word) {
	size_t length = strlen(word);
	char character = " ";
	double total = 0;

	// iterate through the string and increase its alphabetical rank based on the value of each digit in ASCII
	for (size_t index = 0; index < length; index++) {

		// grab each character in the string
		character = word[index];

		// check that the ascii value is 
		if (character >= 97 &&		// between a
			character <= 112)		// and z
		{
			character = character - 32;		// convert to uppercase ASCII to equalize
		}
		character = character - 64;			// make it so A gets a value of 1, b 2, and so on til z 26 
		total = total + (character / pow(100, index));			// add to total 
		// basically it adds a smaller value with each additional character, so a < aa < aaa < aaaa 
		// This accounts for the thing where aaa is lower than abc to correctly order
		// modulus is a power of 100 because the range is 1-26, and with just 10^index it yields AZ < B or something like that
	}

	// return the final numerical ranking of the string to be compared with other strings' rankings
	return total;
}

// prompt user for input and has a basic loop
// for entering a valid rideshare currently stored in the linked list.
RideShareOrg* selectRideShare(RideShareOrg* headPtr) {

	// initialize
	RideShareOrg* returnPtr = NULL;

	if (headPtr != NULL) {

		// iteration controller
		bool found = false;
		RideShareOrg* currentPtr = headPtr;
		
		// while valid data not yet entered
		while (found == false) {

			// start currentPtr at the first element in the list
			currentPtr = headPtr;
			printf("Enter the name of the rideshare service you would like to use: ");
			char inputName[STRING_LENGTH] = "";

			// get desired rideshare from user
			stringNoNewLine(inputName, STRING_LENGTH);
		
			while (found == false &&		// while not found
				   currentPtr != NULL) {	// while not at end of list

				char currentName[STRING_LENGTH] = "";
				strcpy(currentName, currentPtr->orgName);			// avoid editing the name in the rideshare itself

				// match case between strings for non-case sensitive comparison
				matchCase(currentName);
				matchCase(inputName);

				// if found, break the loop and assign return pointer to the element 
				if (strcmp(currentName, inputName) == 0) {
	
					returnPtr = currentPtr;
					found = true;
				}
				// otherwise keep looking
				else {
					currentPtr = currentPtr->nextRideSharePtr;
				}
			}

			// reached end of list and not found
			if (currentPtr == NULL) {
				printf("rideshare %s not found. \n", inputName);

				// reset linked list iteration to try again for valid input
				currentPtr = headPtr;
				
			}
		}
	}

	// return the valid pointer

	return returnPtr;
}

// Convert the alphabetical letters in a string to upper case
// Parameter: word the string to convert
// no returns
void matchCase(char* word) {

	// get length so right number of loops are performed
	size_t length = strlen(word);
	char character = " ";

	// iterate through the string and increase its alphabetical rank based on the value of each digit in ASCII
	for (size_t index = 0; index < length; index++) {

		// grab each character in the string
		character = word[index];

		// check that the ascii value is 
		if (character >= 97 &&		// between a
			character <= 122)		// and z
		{
			character = character - 32;		// convert to uppercase ASCII to equalize string case
		}
		// change that index in string if its case needs to change
		word[index] = character;
	}

}

// write all necessary admin summary information to a file
// Parameter: RideShareOrg *headPtr - the head of the linked list storing all rideshare structures
// no returns
void writeInfoFile(RideShareOrg * headPtr) {

	FILE* filePtr = NULL;

	// initialize filepath
	char filePath[STRING_LENGTH] = FILE_PATH;
	char name[STRING_LENGTH] = "";				// for removing spaces from rideshare orgName

	// go through the linked list and write to a unique file for each rideshare there
	if (headPtr != NULL) {

		RideShareOrg* currentPtr = headPtr;

		// go through 
		while (currentPtr != NULL) {

			// reset filepath for each rideshare organization
			strcpy(filePath, FILE_PATH);

			
			strcpy(name, currentPtr->orgName);
			replaceSpaces(name);
			// add rideshare name to filepath, and .txt to create the file
			strcat(filePath, name);
			strcat(filePath, ".txt");

			// make sure file can actually open before adding
			if ((filePtr = fopen(filePath, "w")) == NULL) {
				printf("Could not open file for %s \n", currentPtr->orgName);
			}
			else {

				// calculate and write the business summary info
				calcAverages(currentPtr);
				fprintf(filePtr, "Business Summary for %s: \n", currentPtr->orgName);

				// account for no rides case
				if (currentPtr->riderCount == 0) {
					fprintf(filePtr, "%s", "There were no rides. \n");
				}
				else {
					filePrintTotals(currentPtr, filePtr);
				}

				// account for no surveys case
				if (currentPtr->surveyCount == 0) {
					fprintf(filePtr, "%s", "There were no ratings. \n");
				}
				else {
					writeFileAverages(currentPtr->ratingAverages, filePtr);
					fprintf(filePtr, "%s", "\n");
				}

				// go to the next rideshare organization
				currentPtr = currentPtr->nextRideSharePtr;
			}
		}
	}
	// if the list is empty somehow then print this (shouldn't happen with how adminSetup works but just in case) 
	else {
			printf("List Empty");
	}
		
}
	
// writes to file the contents of averages array, basically the same as displayAverages
// Parameters: avgArr[] the array of averages to write
// filePtr the file to write to.
// No returns
void writeFileAverages(const double avgArr[], FILE * filePtr) {

	fprintf(filePtr, "Category Rating Averages \n");
	writeFileCategories(surveyCategories, filePtr);			// Global constant

	for (int category = 0; category < SURVEY_CATEGORIES; category++) {
		// display each result to 1 decimal precision and \t tab to allign with categories
		fprintf(filePtr, "%.1lf\t", avgArr[category]);
	}

}

// writes to file the categories of a survey
// Basically printsurveys but for files
// Parameters: 
// surveyCategories[] the array of categories given at top of program
// filePtr the file to write to
// 
// No returns
void writeFileCategories(const char* surveyCategories[], FILE * filePtr) {

	// write each category to file, \t for spacing
	for (int category = 0; category < SURVEY_CATEGORIES; category++) {
		fprintf(filePtr, "%s\t", surveyCategories[category]);
	}
	// format so the next output will be readable
	fprintf(filePtr, "%s", "\n");
}

// replaces spaces with _ in a given string
// Param name the string to replace spaces in
// no returns
void replaceSpaces(char* name) {

	size_t length = strlen(name);

	// iterate through string and replace each space with _
	for (size_t index = 0; index < length; index++) {
		if (name[index] == ' ') {
			name[index] = '_';
		}
	}
}
// de-allocates memory of a linked list
// parameter RideShareOrg *headPtr - the head of the linked list to de-allocate
// no returns
void deAllocateList(RideShareOrg* headPtr) {
	// if the head pointer is actually pointing
	if (headPtr != NULL) {
		RideShareOrg* currentPtr = headPtr;			// begin iterating through the list
		RideShareOrg* previousPtr = NULL;

		// while not at end of list
		while (currentPtr != NULL) {
			previousPtr = currentPtr;	// need to do this before free or else there is no structure to get the next node
			
			currentPtr = currentPtr->nextRideSharePtr;		// move to next element
			free(previousPtr);		// de allocate the memory associated with that node
		}
		printf("\nDe-allocated memory of list. ");
	}
	else {
		printf("\nError: List Empty");
	}
}
