//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Joey Grippi
// Version     : 1.0
// Copyright   : Copyright © 2023
// Description : Project Two, Computer Science at ABCU written in C++
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

/*
* Each course has an ID and a title.
* A vector will hold the prerequisites
* for the courses.
*/

// Creating a structure to group course elements
struct Course {
	
	// Course elements
	string courseId;
	string courseTitle;
	vector<string> prereqs;

	// This is the default constructor
	Course() {}
};

/*
* Now we have to create the internal
* structure for the tree node.
*/

// Creating a structure for the tree node
struct Node {
	
	// Node elements
	Course course;
	Node* left;
	Node* right;

	// This is the default constructor
	Node() {
		
		left = nullptr;
		right = nullptr;
	}

	// Initializing the node with a course
	Node(Course aCourse) : Node() {

		this->course = aCourse;
	}

};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/*
 * Now we define the BST class which
 * contains the data members and methods 
 * to implement a BST.
 */

class BinarySearchTree {

// Defining the private class attributes and methods
private:
		
	Node* root;
	void addNode(Node* node, Course course);
	void printSampleSchedule(Node* node); 
	void printCourseInformation(Node* node, string courseId);

// Defining the public class attributes and methods
public:
		
	BinarySearchTree();
	virtual ~BinarySearchTree();
	void Insert(Course course); 
	void PrintSampleSchedule();
	int NumPrerequisiteCourses(Course course);
	void PrintCourseInformation(string courseId); 
};

/*
 * Here we create the BST default constructor
 * and initialize the house keeping variables.
 */

// Binary search tree default constructor
BinarySearchTree::BinarySearchTree() {

	// Initializing house keeping variables
	// Setting root to nullptr 
	root = nullptr;
}

/*
 * Here we create the BST destructor.
 */

// Binary search tree destructor
BinarySearchTree::~BinarySearchTree() {
	
	// Recurse from root deleting every node
}

/*
 * Now we define the Insert function,
 * which is how we will insert a
 * course node in the tree.
 */

void BinarySearchTree::Insert(Course course) {

	// If root is equal to nullptr
	if (root == nullptr) {

		// Set root equal to new node course
		root = new Node(course);
	}

	// Else, we call addNode method
	else {

		// Calling addNode method to insert the course
		addNode(root, course);
	}
}

/*
 * This is the addNode function defintion, 
 * which is recursive. This is the helper
 * function for the Insert function to
 * add a course node into the tree.
 */

void BinarySearchTree::addNode(Node* node, Course course) {

	// If the course Id is less than the node's course Id, 
	// we will add to the left
	if (course.courseId < node->course.courseId) {

		// If there is no left node
		// If the left node is equal to nullptr
		if (node->left == nullptr) {

			// This node becomes the left
			node->left = new Node { course };
		}

		// Else, we will recurse down the left node
		else {

			// Recursive call in order to traverse left
			addNode(node->left, course);
		}
	}

	// Else, we need to add to the right
	else {

		// If there is no right node
		// If there right node is equal to nullptr
		if (node->right == nullptr) {

			// This node becomes the right
			node->right = new Node { course };
		}

		// Else, we will recurse down the left node
		else {

			// Recursive call in order to traverse right
			addNode(node->right, course);
		}
	}
}

/*
 * This function defines how we will
 * traverse the BST in order.
 */

void BinarySearchTree::PrintSampleSchedule() {

	// Calling helper function printSampleSchedule and pass the root
	printSampleSchedule(root);
}

/*
 * This is the printSampleSchedule function defintion,
 * which is recursive. This is the helper
 * function for traversing the tree in order.
 */

void BinarySearchTree::printSampleSchedule(Node* node) {

	// If the node is not equal to nullptr
	if (node != nullptr) {

		// Recursively call printSampleSchedule using the left
		printSampleSchedule(node->left);

		// Outputting course ID and the course title
		cout << node->course.courseId << ", " << node->course.courseTitle << endl;

		// Recursively call printSampleSchedule using the right
		printSampleSchedule(node->right);
	}
}

/*
 * This function will find the number of 
 * prerequisites of a given course and
 * return the number as an integer value.
 */

int BinarySearchTree::NumPrerequisiteCourses(Course course) {

	// We will initialize a counter variable for the loop
	// which will be returned for the number of prerequisites

	// Initializing the counter variable to 0 as default
	int counter = 0;

	// For loop that loops through the prerequisites
	for (unsigned int i = 0; i < course.prereqs.size(); i++) {

		// If the length of prerequisites is greater
		// than zero, increment the counter variable
		if (course.prereqs.at(i).length() > 0)
			counter++;
	}

	// Return the counter variable, which is the
	// number of prerequisites for the course
	return counter;
}

/*
 * This function defines how we will
 * print a specific course and
 * the required prerequisites.
 */

void BinarySearchTree::PrintCourseInformation(string courseId) {

	// Calling helper function printSampleSchedule and pass the root and courseId 
	printCourseInformation(root, courseId);
}

/*
 * Here we define the printCourseInformation function
 * which will print a specific course's info
 * including the required prerequisites.
 */

void BinarySearchTree::printCourseInformation(Node* current, string courseId) {

	// We need to create a loop to traverse down the tree
	// until we reach the bottom or a match is found

	// While the current node is not equal to null
	while (current != nullptr) {

		// If the course ID that is entered matches the current node,
		// we will print the course and call the function
		// to get the number of prerequisites.

		// If the course ID equals the current node's course ID
		if (courseId == current->course.courseId) {

			// Outputting course ID and the course title
			cout << current->course.courseId << ", " << current->course.courseTitle << endl;

			// Initializing an int variable that will store the
			// number of prerequisites, which is determined by
			// calling the NumPrerequisitesCourses function
			unsigned int numPrereqs = NumPrerequisiteCourses(current->course);

			// Output to user
			cout << "Prerequisite(s): ";

			// Initialzing an int variable and setting to zero for the loop
			unsigned int i = 0;

			// Loop to determine the number of prerequisites and
			// printing out the relevant info.

			// For loop that will execute if there are prerequisites
			for (i = 0; i < numPrereqs; i++) {

				// Printing the prerequisites, if there is more than one,
				// they will need to be separated by a comma
				cout << current->course.prereqs.at(i);

				// If there is still another prerequsite at the end of the loop,
				// print out a comma to separate the prerequisites for the 
				// next iteration.
				if (i != numPrereqs - 1) {

					cout << ", ";
				}
			}

			// If there are no prerequisites for the course,
			// output that there are none.
			if (i == 0) {

				cout << "\nThere are no prerequisites for " << current->course.courseId 
					<< " - " << current->course.courseTitle << ".";
			}

			cout << endl;
			return;
		}

		// Else, if the courseId is less than the current node's courseId, traverse left
		else if (courseId < current->course.courseId) {


			// Setting current node to current node left to traverse left
			current = current->left;
		}

		// Else, the courseId is larger than the current node's courseId, traverse right
		else {

			// Setting current node to current node right to traverse right
			current = current->right;
		}
	}

	// If there is no matching courseId in the tree, inform the user of invalid input
	cout << courseId << " not found." << endl;
}

/*
 * This is the function that will read in the
 * course information from an input file and
 * return a boolean value.
 */

bool loadDataStructure(string filepath, BinarySearchTree * bst) {

	// Creating ifstream object to open and
	// read from the given filepath
	ifstream inputFile(filepath);
	
	// If the input file is open a while loop
	// executes to read the file
	if (inputFile.is_open()) {
		
		// Loop to read data from each line
		// until the end of the file
		while (!inputFile.eof()) {
			
			// Creating a vector to store the courses
			vector<string> courseList;
			
			// Initializing a string variable to store the
			// words from each line of the input file
			string word;
			
			// Using getline to process each word
			// from the line in the input file
			getline(inputFile, word);
			
			// While the number of words per line is greater than zero
			while (word.length() > 0) {
				
				// Here we create an int variable to store
				// the number between each comma
				unsigned int delim = word.find(',');
				
				// If the field has less than 100 characters
				if (delim < 100) {

					// Add the word substring to the vector
					courseList.push_back(word.substr(0, delim));
					
					// Deleting the word from the string
					word.erase(0, delim + 1);
				}

				// Else, we will add the word after the last comma
				else {
					
					// Add the word substring to the vector
					courseList.push_back(word.substr(0, word.length()));
					
					// Setting word as an empty string
					word = "";
				}
			}
			
			// Now, we instiantiate the course object
			// to insert the courses into the BST
			Course course;

			// Setting the course ID as the 
			// first parameter in courseList
			course.courseId = courseList[0];

			// Setting the course title as the
			// second parameter in courseList
			course.courseTitle = courseList[1];
			
			// Loop for the the third parameter which
			// is the prerequisite, then add to the courseList
			for (unsigned int i = 2; i < courseList.size(); i++) {
				
				// Add the prerequisite to the vector
				course.prereqs.push_back(courseList[i]);
			}
			
			// Call the insert function to insert the
			// course into the tree
			bst->Insert(course);
		}

		// Closing the input file
		inputFile.close();
		
		// The file was successfully read,
		// so return true for the boolean
		return true;
	}

	// The file was not successfully read,
	// so return false for the boolean
	return false;
}
//==============================================================================//
// Main method 
//==============================================================================//

int main(int argc, char* argv[]) {
	
	// Initializing the variables for the main method
	// String to store the user filename
	string filepath; 

	// String to store the user course
	string userCourse;
	
	// Switch statement for command line arguments
	switch (argc) {

	case 2:
		filepath = argv[1];
		break;
	
	case 3:
		break;
	
	default:
		filepath = "infile.txt";
		break;
	}

	// Here we instantiate the binary search tree
	BinarySearchTree* bst = new BinarySearchTree();

	// Now we output the introduction to the user
	cout << "\nWelcome to the course planner.\n" << endl;


	// Creating an int variable to store the
	// user choice and setting equal to zero
	int choice = 0;

	// Menu loop, while user choice does
	// not equal 9
	while (choice != 9) {

		// Outputting the menu options
		cout << " 1. Load Data Structure." << endl;
		cout << " 2. Print Course List." << endl;
		cout << " 3. Print Course." << endl;
		cout << " 9. Exit" << endl;
		cout << "\nWhat would you like to do? ";
		
		// Reading in the user choice 
		cin >> choice;

		// Creating a boolean that verifies if the
		// user input file was openly successfully
		bool fileLoaded = 1;


		// Switch statement to pass the user input for the menu
		switch (choice) {
		
		// First case to load the input file and store
		// data into the binary search tree
		case 1:

			// Output to user
			cout << "Please enter the file path: ";
			
			// Read in filepath from user
			cin >> filepath;
			
			// Call the loadDataStructure function to 
			// attempt to load the input file
			fileLoaded = loadDataStructure(filepath, bst);
			
			// If the file was loaded successfully, 
			// output to user
			if (fileLoaded) {

				cout << "The file was loaded successfully.\n" << endl;
			}

			// Else, the file was not found, output to user
			else {

				cout << "File not found.\n" << endl;
			}

			// Break the case
			break;
			
		// Second case that will print the sorted course list
		case 2:
				
			// Outputting the sorted course list
			cout << "Here is a sample schedule:\n" << endl;
				
			// Calling the PrintSampleSchedule to print the courses
			bst->PrintSampleSchedule();
			cout << endl;
			
			// Break the case
			break;
		
		// Third case that will request for the user course ID
		// and then retrieve and print the relevant info
		case 3:
					
			// Output to user
			cout << "What course do you want to know about? ";
					
			// Read in the user input and store in userCourse
			cin >> userCourse;
					
			// For loop to convert user input to upper case
			for (auto& choice : userCourse) {
						
				// Converting input to uppercase equivalent
				choice = toupper(choice);
			}

			// Calling the PrintCourseInformation to print the course info
			bst->PrintCourseInformation(userCourse);
			cout << endl;

			// Break the case
			break;

		// Default case which verifies user input for the menu
		default:

			if (choice != 9) {
				// Output invalid selection to user
				cout << "Invalid selection. Please try again.\n" << endl;
			}
			
			// Break the case
			break;
		}
	}

	// Output exit message to user
	cout << "\nThank you for using the course planner!" << endl;

	// Exit the program
	return 0;
}