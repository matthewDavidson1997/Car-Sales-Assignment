#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 

// Give error number
extern int errno;

#define bool int

// Constants
#define TWO_MULTIBUY_DISCOUNT 0.1f
#define THREE_MULTIBUY_DISCOUNT 0.15f
#define MORE_THAN_THREE_MULTIBUY_DISCOUNT 0.2f
#define MAX_SALES 100
// File variables and constants
  // Add your own file path here
#define CSV_FILE "C\\Data.csv"
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2
// Will hold the file stream once opened
FILE* file;
unsigned char fileStatus = FILE_CLOSED;
// Improves readability
#define MENU_OPTION_VIEW_CARS 'a'
#define MENU_OPTION_BUY_CAR 'b'
#define MENU_OPTION_VIEW_SALES 'c' 
#define MENU_OPTION_EXIT 'x' 
#define TRUE 1
#define FALSE 0

/* VARIABLES */
// Shorts for integers
unsigned short carsNeeded = 0, carsSold = 0, carAvailable = 0, carTypeChoice = 0, totalCarCount = 0;
// Bool type for True/False responses
bool correctInput = FALSE;
// Floats for values with decimal places
float totalPrice = 0.0f, discountPercentage = 0.0f, discountSaving = 0.0f, salesValue = 0.0f, priceBeforeDiscount = 0.0f;
// Chars for Character inputs
char userChoice, completePurchase;
// Char for backslash, I was using this in attempt to create a generic filepath function
// char backslash = 92;

// Array for value of each sale
float totalSales[MAX_SALES];
// Array for discount of each sale
float discountPerSale[MAX_SALES];
// Keep track of how many sales have taken place
int numberOfSales = 0;
// Track number of cars sold per transaction
int carsSoldPerSale[MAX_SALES];
// Array to hold carTypeChoicePerSale
int carTypeChoicePerSale[MAX_SALES];

// Array to hold name of each customer
char customerNames[MAX_SALES][201];
// Array to hold type of car sold for each transaction
char carTypeSold[MAX_SALES][11];


/* Information about the cars available which will not change */
// Types of cars available
char carTypes[][100] = { "2017 Model", "2018 Model", "2019 Model", "2020 Model", "2021 Model" };
// Car Prices
float carPrices[] = { 20000.00f, 21000.00f, 23000.00f, 25000.00f, 30000.00f };
// Availability of each type of car (this will change with the introduction of reading previous sales from file)
int carsAvailable[] = { 10, 10, 10, 10, 10 };


/* Functions */

// void getFilePath() {
// I did not manage to get this function to make a generic filepath for use on any machine to work. I have included my workings to try and show my thought process...
	// What I was trying to do was save a filepath for use in a .txt file to then be read and used in #define CSV_FILE
	// I did manage to get what I wanted to do working when writing it in a .bat file but c didn't like having \ characters 
		// which I tried to solve by by creating a char with the ascii code for it and calling it as you can see below
		// However I still don't seem to be able to make it work... 
		// I will give it a bit more thought but if you are reading this then it means I didn't manage to come up with a solution
		// Because I didn't manage to successfully write a filepath to a .txt file I have not attempted to read from it, 
		// but I would have done this with an fscanf saving the filepath as a string variable to be used in #define CSV_FILE
	//if (system(NULL) != 0) {
		//char cmd[100], cmd1[100], cmd2[100];
		// Create strings to be run in system()
		//strcpy(cmd, "cd %HOMEPATH%");
		// Here I convert the format of HOMEPATH which is \Users\user to \\Users\\user because this is the format needed for the filepath in c, I save this new filepath in a variable filepath_reference
		//strcpy(cmd1, "set filepath_reference=%HOMEPATH:%c=%c%c%", backslash, backslash, backslash);
		// Here I want to print the variable created to a file 'filepath.txt'
		//strcpy(cmd2, "echo %filepath_reference% >> filepath.txt");
		//system(cmd);
		//system(cmd1);
		//system(cmd2);
	//}
	//else {
		// This else condition is for if the program cannot access the command processor
		//printf("Command processor is not present");
	//}
//}
void assumeFalseChoice() {
	correctInput = FALSE;
}
void clearScreen() {
	system("cls");
}
void pauseProgram(char userChoice) {
	// give the user a chance to read the previous output, allow them to continue when ready
		// customise the output depending on the user's choice
	if (userChoice == MENU_OPTION_EXIT) {
		printf("\n\nPress Enter to Exit...");
	}
	else {
		printf("\n\nPress Enter to return to the Menu...");
	}
	// Two get chars to catch newline characters likely to be floating around in the program

	if (userChoice != '_') {
		getchar();
	}

	getchar();
}
void swapUnsignedShort(unsigned short* a, unsigned short* b) {
	unsigned short temp;
	// copy the value found at the pointer a's address into the newly created temp variable
	temp = *a;
	// copy the value found at the pointer a's address into the address represented by the pointer b
	*a = *b;
	// copy the value of the temp variable to the pointer b's address
	*b = temp;
}
void swapFloat(float* a, float* b) {
	float temp;
	// copy the value found at the pointer a's address into the newly created temp variable
	temp = *a;
	// copy the value found at the pointer a's address into the address represented by the pointer b
	*a = *b;
	// copy the value of the temp variable to the pointer b's address
	*b = temp;
}


// Start File Functions
FILE* createFile(char fileName[201]) {
	// Open file for writing
	file = fopen(fileName, "w");
	// Close file if successful
	if (file != NULL) {
		fclose(file);
	}
	return file;
}
void openFile(char fileName[201], char mode[4]) {
	// Open the file
	file = fopen(fileName, mode);

	// if file's value is NULL, then possible error
	if (file == NULL) {

		// Try to create a file and then check if it's still NULL meaning something failed
		if (createFile(fileName) == NULL) {
			fileStatus = FILE_ERROR;

			// Will produce the output: There was an error opening [filelocation]\data.csv: No such file of directory
			printf("There was an error opening %s: %s", fileName, strerror(errno));
		}
		else {
			// Try to execute function again which should now succeed
			openFile(fileName, mode);
		}

	}

	else {
		// Update file status
		fileStatus = FILE_OPENED;
	}
}
void closeFile() {
	// Only attempt to close the file if it's already open
	if (fileStatus == FILE_OPENED) {
		fclose(file);
		fileStatus = FILE_CLOSED;
	}
}
void readDataFromFile() {
	// Will keep track of how many lines were read from the file
	int lineCounter = 0;

	while (1) {

		int carsSoldPerSaleValue = 0;
		int carTypeChoicePerSaleValue = 0;
		float totalSalesValue = 0.0f, discountPerSaleValue = 0.0f;
		char carTypeSoldValue[35] = "", customerNamesValue[201] = "";

		int scanResult = fscanf(
			file,
			"%d,%d,%[^,],%f,%f,%[^\n]s", 

			&carTypeChoicePerSaleValue,
			&carsSoldPerSaleValue,
			&carTypeSoldValue,
			&totalSalesValue,
			&discountPerSaleValue,
			&customerNamesValue
		);

		// If we reached the end of the file
		if (scanResult == EOF) {
			// Then, stop the loop
			break;
		}

		// Add read data to correct arrays
		carTypeChoicePerSale[lineCounter] = carTypeChoicePerSaleValue;
		carsSoldPerSale[lineCounter] = carsSoldPerSaleValue;
		strcpy(carTypeSold[lineCounter], carTypeSoldValue);
		totalSales[lineCounter] = totalSalesValue;
		discountPerSale[lineCounter] = discountPerSaleValue;
		// strcopy for sting values
		strcpy(customerNames[lineCounter], customerNamesValue);

		// update variables and arrays with previous sales data
		carsSold += carsSoldPerSaleValue;
		carsAvailable[carTypeChoicePerSaleValue - 1] -= carsSoldPerSaleValue;

		// increment the lineCounter, ready for next line that might be read
		lineCounter++;
	}

	// update numberOfSales with linecounter value
	numberOfSales = lineCounter;

}
void getDataFromFile() {
	openFile(CSV_FILE, "r");

	if (fileStatus == FILE_OPENED) {
		readDataFromFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf(" There was an error trying to read from the file %s.", CSV_FILE);
		// Function requires a char
		pauseProgram('_');
	}

	closeFile();
}
void writeDataToFile() {
	// loop through every sale
	for (int i = 0; i < numberOfSales; i++) {

		char line[201];
		char data[50];

		// Convert the value into a string
		_itoa(carTypeChoicePerSale[i], data, 10);
		// add carsTypeChoicePerSale to line
		strcpy(line, data);
		// Comma to seperate next value
		strcat(line, ",");

		// Convert the value into a string
		_itoa(carsSoldPerSale[i], data, 10);
		// add carsSoldPerSale to line
		strcat(line, data);
		// Comma to seperate next value
		strcat(line, ",");

		// Add carTypeSold sold to line
		strcat(line, carTypeSold[i]);
		// Comma to seperate next value
		strcat(line, ",");

		// reset the data variable
		strcpy(data, "");
		// convert the value into a string
		sprintf(data, "%.2f", totalSales[i]);
		// add the totalSales to the line
		strcat(line, data);
		// Comma to seperate next value
		strcat(line, ",");

		// reset the data variable
		strcpy(data, "");
		// convert the value into a string
		sprintf(data, "%.2f", discountPerSale[i]);
		// add the discountPerSale to the line
		strcat(line, data);
		// Comma to seperate next value
		strcat(line, ",");

		// Add customer name to the line
		strcat(line, customerNames[i]);
		// write line to file
		fprintf(file, line);
		// Last value on line so no need for another comma

		// Add new line character unless we're at the end of the file
		if (i < numberOfSales - 1) {
			fprintf(file, "\n");
		}

	}

}
void saveDataToFile() {
	openFile(CSV_FILE, "w");

	if (fileStatus == FILE_OPENED) {
		writeDataToFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to write to the file %s.", CSV_FILE);
		// this function requires a char value, so we give it one 
		pauseProgram('_');
	}

	closeFile();
}
// End file functions

void menu_showMenu() {
	printf(" _________________________________\n");
	printf("|                                 |\n");
	printf("| Welcome to the Car Sales office!|\n");
	printf("|                                 |\n");

	// Present the various actions the user can choose from
	printf("| Menu:                           |\n");
	printf("| a. View Cars                    |\n");
	printf("| b. Buy Cars                     |\n");
	printf("| c. View Sales Stats             |\n");
	printf("| x. Exit                         |\n");
	printf("|_________________________________|\n\n");
}
char readCharFromConsole(char message[201]) {
	// This function takes a user input and can be used in various places with a different message prompting the user
	char userInput;

	printf(message);
	userInput = getchar();
	while (getchar() != '\n');
	return userInput;
}
unsigned short readUnsignedShortFromConsole(char message[201]) {
	// This function takes a user input and can be used in various places with a different message prompting the user
	unsigned short userInput;

	printf(message);
	scanf("%hd", &userInput);
	return userInput;
}
unsigned short readUnsignedShortFromConsole2(char message[201], char value[201]) {
	// This function takes a user input for how many cars they would like to purchase
	unsigned short userInput;

	printf("How many %s cars would you like to purchase? Quantity: ", value);
	scanf("%hd", &userInput);
	return userInput;
}
void makeMenuChoice() {
	// Loop for choice
	do {
		// assume choice will be incorrect
		assumeFalseChoice();
		// prompt user for their choice
		userChoice = readCharFromConsole("Please make a choice: ");
		// Validate the user has made a valid menu choice
		if (userChoice == MENU_OPTION_VIEW_CARS || userChoice == MENU_OPTION_BUY_CAR || userChoice == MENU_OPTION_VIEW_SALES || userChoice == MENU_OPTION_EXIT) {
			correctInput = TRUE;
		}
		// If user makes invalid choice give error message
		else {
			printf("\nSorry, invalid choice. Please try again. \n");
		}
	} while (correctInput != TRUE);

	printf("\n");
}
void sortArraysByNumberOfCarsAvailable() {

	// Loop to iterate over each carType in the array
	for (int i = 0; i < 6 - 1; i++) {

		// Second loop to iterate over the next value in the array to be compared to the previous value
		for (int j = i + 1; j < 6; j++) {

			// Check if the value at position i is greater than the value at position j
			if (carsAvailable[i] < carsAvailable[j]) {

				// If value at position j is bigger than position i we use our swapping functions to swap values so they become ordered from greatest to smallest
				swapUnsignedShort(&carsAvailable[i], &carsAvailable[j]);
				swapFloat(&carPrices[i], &carPrices[j]);

				// using strcpy instead of a function is simpler so we use this to swap the string values in the array
				char temp[201];
				// copy string from position i to the newly created temp variable
				strcpy(temp, carTypes[i]);
				// copy string from position j into position i
				strcpy(carTypes[i], carTypes[j]);
				// copy string from temp into position j
				strcpy(carTypes[j], temp);


			} // end of "if" statement

		} // end of second "for" loop

	} // end of first "for" loop

}
void presentSituationWithCars() {

	/* Present the situation with cars */
	sortArraysByNumberOfCarsAvailable();
	// Check that any value in array is greater than 0
	totalCarCount = 0;
	for (int i = 0; i < (sizeof(carsAvailable) / sizeof(carsAvailable[0])); i++)
		totalCarCount += carsAvailable[i];
	if (totalCarCount > 0) {
		printf("\nOption Number: | Car type:    | Price:         | Number Available:     \n");
		int i;
		for (i = 0; i < 5; i++) {

			printf("%d.             | %s   | %.2f GBP   | %hd\n",
				i + 1, carTypes[i], carPrices[i], carsAvailable[i]);
		}

	}
	// If there are no cars available for any car type the user will be told there are no cars left
	else {
		printf("Sorry, there are no cars available right now, come back later.\n");
		return;
	}
}
void askUserForCarChoice() {

	assumeFalseChoice();
	// Reset carTypeChoice to 0 so if the user types a non-numeric response it will give an error
	carTypeChoice = 0;
	do {
		/* Prompt user for type of car wanted */
		carTypeChoice = readUnsignedShortFromConsole("Which type of car would you like to purchase? Option Number: ");

		// Validate Car Type Choice
		// Ensure there are cars available for the choice
		// Ensure the selection is within a valid range
		if (carTypeChoice >= 1 && carTypeChoice <= 5) {
			if (carsAvailable[carTypeChoice - 1] > 0) {
				correctInput = TRUE;
			}
			else {
				printf("Sorry there are no %s cars available, please make a different selection.\n", carTypes[carTypeChoice - 1]);
			}
		}
		// Ask the user for a valid selection between 1 and the highest carTypeChoice
		else {
			printf("Please make a valid selection between 1-%hd.\n", (sizeof(carsAvailable) / sizeof(carsAvailable[0])));
		}

	} while (correctInput != TRUE);

}
void askUserQuantityOfCars() {

	assumeFalseChoice();
	do {
		// Validate that the user has entered a valid number of cars
		carsNeeded = 0;
		carsNeeded = readUnsignedShortFromConsole2("", carTypes[carTypeChoice - 1]);
		// If the user enters a number of cars which is available the input will be set to true
		if (carsNeeded > 0 && carsNeeded <= carsAvailable[carTypeChoice - 1]) {
			correctInput = TRUE;
		}
		// If there are fewer cars available than they have selected they will be given an appropriate error message and asked again
		else if (carsAvailable[carTypeChoice - 1] < carsNeeded) {

			printf("Sorry, there are fewer cars available than you have requested.\n");
		}
		// Catch all error to aks user to try again
		else {
			printf("Please enter a number between 1 and %hd\n", carsAvailable[carTypeChoice - 1]);
		}

	} while (correctInput != TRUE);
}
void calculateSaleDetails() {

	/* Calculate price */
	priceBeforeDiscount = (carPrices[carTypeChoice - 1] * carsNeeded);

	/* Apply discount for totalPrice*/
	if (carsNeeded == 1) {
		printf("\nNo Discount is applied.\n");
		discountPercentage = 0;
	}
	else if (carsNeeded == 2) {
		printf("\nYou get a discount of 10%%.\n");
		discountPercentage = TWO_MULTIBUY_DISCOUNT;
	}
	else if (carsNeeded == 3) {
		printf("\nYou get a discount of 15%%.\n");
		discountPercentage = THREE_MULTIBUY_DISCOUNT;
	}
	else if (carsNeeded > 3) {
		printf("\nYou get a discount of 20%%.\n");
		discountPercentage = MORE_THAN_THREE_MULTIBUY_DISCOUNT;
	}

	/* Ammend totalPrice and calculate saving */
	totalPrice = (priceBeforeDiscount * (1 - discountPercentage));
	discountSaving = (priceBeforeDiscount - totalPrice);
}
void showCheckoutOption() {

	/* Present purchase option*/
	printf("\n----------------------CHECKOUT----------------------\n\n");
	printf("You have selected %hd cars. \n", carsNeeded);
	if (carsNeeded > 1) {
		printf("Total price before discount is %.2f GBP.\n", priceBeforeDiscount);
		printf("Total price to pay is %.2f GBP.\n", totalPrice);
		printf("That is a saving of %.2f GBP.\n", discountSaving);
	}
	else {

		printf("Total price to pay is %.2f GBP.\n", totalPrice);
	}
	printf("\n----------------------------------------------------\n\n");
}
void userChoiceCompletePurchase() {

	assumeFalseChoice();

	// Ask user if they would like to complete purchase
	printf("\nWould you like to proceed with the purchase? Enter Y/N: \n");
	do {

		scanf("\n%c", &completePurchase);
		// Clear buffer of any unwanted characters
		while (getchar() != '\n');

		// Validate input of completePurchase
		// If the user types 'Y' or 'y' the transaction will complete 
		if (completePurchase == 'Y' || completePurchase == 'y') {
			correctInput = TRUE;
			// Ask user for their name
			printf("\nPlease enter your name: ");
			scanf("\n%[^\n]s", &customerNames[numberOfSales]);
			// Clear buffer of any unwanted characters
			while (getchar() != '\n');

			printf("\nThank you for your custom %s, have a great day!\n", customerNames[numberOfSales]);
			/* Update and present stock information */
			carsAvailable[carTypeChoice - 1] -= carsNeeded;
			strcpy(carTypeSold[numberOfSales], carTypes[carTypeChoice - 1]);
			printf("There are %hd %s cars remaining.\n", carsAvailable[carTypeChoice - 1], carTypes[carTypeChoice - 1]);
			// Update values from this transaction for sales stats option
			carsSoldPerSale[numberOfSales] = carsNeeded;

			totalSales[numberOfSales] = totalPrice;
			discountPerSale[numberOfSales] = discountSaving;
			carsSold += carsNeeded;
			numberOfSales++;
			carTypeChoicePerSale[numberOfSales - 1] = carTypeChoice;
		}
		// If user types upper or lower case 'N' it will cancel the transaction
		else if (completePurchase == 'N' || completePurchase == 'n') {
			correctInput = TRUE;
			printf("\nSorry we could not help you, have a good day.\n");
		}
		// If the user enters something other than 'Y' or 'N' it will ask for a valid choice
		else {

			printf("Please enter a valid choice. Y/N: \n");
		}

	} while (correctInput != TRUE);
}
void sortArraysByNumberOfCarsSoldPerSale() {

	// Loop to iterate over each sale in the array
	for (int i = 0; i < numberOfSales - 1; i++) {

		// Second loop to iterate over the next value in the array to be compared to the previous value
		for (int j = i + 1; j < numberOfSales; j++) {

			// Check if the value at position i is greater than the value at position j
			if (carsSoldPerSale[i] < carsSoldPerSale[j]) {

				// If value at position j is bigger than position i we use our swapping functions to swap values so they become ordered from greatest to smallest
				swapUnsignedShort(&carsSoldPerSale[i], &carsSoldPerSale[j]);
				swapFloat(&totalSales[i], &totalSales[j]);
				swapFloat(&discountPerSale[i], &discountPerSale[j]);



				// using strcpy instead of a function is simpler so we use this to swap the string values in the array
				char temp[201];
				// copy string from position i to the newly created temp variable
				strcpy(temp, customerNames[i]);
				// copy string from position j into position i
				strcpy(customerNames[i], customerNames[j]);
				// copy string from temp into position j
				strcpy(customerNames[j], temp);

				char temp2[201];
				// copy string from position i to the newly created temp variable
				strcpy(temp2, carTypeSold[i]);
				// copy string from position j into position i
				strcpy(carTypeSold[i], carTypeSold[j]);
				// copy string from temp into position j
				strcpy(carTypeSold[j], temp2);

			} // end of "if" statement

		} // end of second "for" loop

	} // end of first "for" loop

}
void viewSales() {
	// Call our sorting arrays function so when viewing cars we see them listed from greatest to smallest
	sortArraysByNumberOfCarsSoldPerSale();
	salesValue = 0;
	// Present the user with a list of all sales completed
	for (int i = 0; i < numberOfSales; i++) {

		printf("\n| Sale Index: %d    | Number Sold: %2.0hd    | Car Type: %s    | Sale Amount: %10.2f GBP    | Discount Amount: %10.2f GBP    | Customer Name: %s \n"
			" -------------------------------------------------------------------------------------------------------------------------------------------------------",
			i + 1, carsSoldPerSale[i], carTypeSold[i], totalSales[i], discountPerSale[i], customerNames[i]); 

		salesValue += totalSales[i];
	}

	// Give total sales figures
	printf("\n| Total Cars Sold: %hd    | Total Sales Value: %.2f GBP  \n", carsSold, salesValue);
	printf("\n A total of %hd car(s) have been sold with a total value of %.2f GBP. \n", carsSold, salesValue);
}
void thankUserBeforeExiting() {

	// Thank the user before exiting
	printf("\n\nThank you for using the Car Sales Office.\n\n");

	// Save data to file
	saveDataToFile();
}
void giveUserOptionToReturnToMenu() {

	// Give user option to return to menu unless they have selected exit
	if (userChoice == MENU_OPTION_EXIT) {
		printf("\n\nPress Enter to Exit...");
	}
	else {
		printf("\n\nPress Enter to return to the Menu...");
	}

	// Clear buffer of any unwanted characters
	while (getchar() != '\n');
}


void main() {

	// CAR SALES

	// Check if there is data to be retrieved from file
	getDataFromFile();

	// Menu loop
	do {
		//Keeps screen tidy
		clearScreen();
		//getFilePath();

		/* Greeting */
		menu_showMenu();

		makeMenuChoice();

		// Choose a different course of action depending on user input
		switch (userChoice) {

			// Case 'a' for viewing cars
		case MENU_OPTION_VIEW_CARS:

			clearScreen();

			presentSituationWithCars();

			break;


			// Case 'b' for buying cars
		case MENU_OPTION_BUY_CAR:

			clearScreen();

			presentSituationWithCars();

			askUserForCarChoice();

			askUserQuantityOfCars();

			calculateSaleDetails();

			showCheckoutOption();

			userChoiceCompletePurchase();

			break;

			//Case 'c' for seeing sales stats
		case MENU_OPTION_VIEW_SALES:

			clearScreen();

			viewSales();

			break;

			// Case 'x' for exiting the program
		case MENU_OPTION_EXIT:

			thankUserBeforeExiting();

			break;
		}

		giveUserOptionToReturnToMenu();

		//continue program until user selects to exit
	} while (userChoice != MENU_OPTION_EXIT);

	// Clear screen
	clearScreen();

	printf("\n\nHave a good day!\n\n");

}