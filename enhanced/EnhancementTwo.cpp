//============================================================================
// Name        : EnhancementTwo.cpp
// Author      : Joey Grippi
// Version     : 2.0
// Copyright   : Copyright © 2025
// Description : Enhanced Course Management System with DFS and Topological Sort
//               Implements a Binary Search Tree to manage course data and
//               validate prerequisite relationships using graph algorithms
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

//============================================================================
// Utility functions for formatting and display
// These functions provide consistent formatting throughout the application
//============================================================================

// Prints a line of specified characters with proper indentation
void printLine(char symbol = '=', int length = 78) {
    cout << "  " << string(length, symbol) << endl;
}

// Prints a menu separator line with consistent formatting
void printMainMenuLine(char symbol = '-', int length = 78) {
    cout << "  " << string(length, symbol) << endl;
}

// Displays the menu selection prompt with consistent formatting
void printMenuPrompt() {
    cout << "\n    Please select an option (1-9): ";
}

// Displays input prompts with consistent indentation
void printInputPrompt(const string& prompt) {
    cout << "    " << prompt;
}

// Prints the main menu header with proper formatting
void printHeader() {
    printLine();
    cout << "\n                                MAIN MENU\n" << endl;
    printMainMenuLine();
}

// Prints section headers with consistent formatting
void printSubHeader(const string& text) {
    cout << "\n  " << text << endl;
    cout << "  " << string(58, '-') << endl;
}

// Print functions for different types of messages with consistent formatting
void printSuccess(const string& message) {
    cout << "\n  [SUCCESS] " << message << "\n" << endl;
}

void printError(const string& message) {
    cout << "\n  [ERROR] " << message << "\n" << endl;
    printLine();
}

void printWarning(const string& message) {
    cout << "\n  [WARNING] " << message << "\n" << endl;
}

// Displays the main menu with all available options
void displayMainMenu() {
    printHeader();
    cout << "    1. Import Course Data          - Load course information from a file" << endl;
    cout << "    2. Display All Courses         - View complete course catalog" << endl;
    cout << "    3. Search Course Details       - Find specific course information" << endl;
    cout << "    4. View Prerequisite Path      - See required course sequence" << endl;
    cout << "    5. Check Prerequisites         - Validate prerequisite requirements" << endl;
    cout << "    9. Exit Program                - Close the application" << endl;
    printMainMenuLine();
    printMenuPrompt();
}

//============================================================================
// Course structure definition
// Represents a course with its properties and prerequisite relationships
//============================================================================

struct Course {
    string courseId;              // Unique identifier for the course
    string courseTitle;           // Full name of the course
    vector<string> prereqs;       // List of prerequisite course IDs
    vector<string> dependentCourses;  // Courses that require this as prerequisite
    bool isVisited;              // Used in DFS traversal
    bool isProcessing;           // Used for cycle detection

    // Default constructor
    Course() : isVisited(false), isProcessing(false) {}

    // Constructor with initialization
    Course(string id, string title) :
        courseId(id),
        courseTitle(title),
        isVisited(false),
        isProcessing(false) {}
};

//============================================================================
// Node structure for BST
// Represents a node in the binary search tree using smart pointers
//============================================================================

struct Node {
    unique_ptr<Course> course;    // Smart pointer to course data
    unique_ptr<Node> left;        // Left child node
    unique_ptr<Node> right;       // Right child node

    // Default constructor
    Node() = default;

    // Constructor with course initialization
    explicit Node(Course* aCourse) : course(aCourse) {}
};

//============================================================================
// Binary Search Tree class definition
// Manages course data and provides operations for course management
//============================================================================

class BinarySearchTree {
private:
    unique_ptr<Node> root;                    // Root node of the BST
    unordered_map<string, Course*> courseMap; // Hash map for O(1) course lookup

    // Private helper methods
    void addNode(Node* node, Course* course);
    void printSampleSchedule(const Node* node) const;
    void printCourseInformation(const Node* node, const string& courseId) const;
    bool hasCycle(Course* course, unordered_set<string>& visited, unordered_set<string>& recursionStack);
    void topologicalSortUtil(Course* course, unordered_set<string>& visited, stack<Course*>& Stack);
    void destroyTree(unique_ptr<Node>& node);
    bool isValidCourseId(const string& courseId) const;
    void validatePrerequisites(const Course* course) const;

public:
    // Constructors and assignment operators
    BinarySearchTree() = default;
    ~BinarySearchTree() { destroyTree(root); }

    // Prevent copying to maintain proper memory management
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    // Core functionality
    void Insert(Course* course);
    void PrintSampleSchedule() const;
    void PrintCourseInformation(const string& courseId) const;

    // Enhanced functionality for prerequisite management
    vector<Course*> GetPrerequisiteOrder(const string& courseId);
    bool ValidateAllPrerequisites() const;
    bool HasPrerequisiteCycle(const string& courseId);
    Course* FindCourse(const string& courseId) const;
    void BuildDependencyGraph();
};

//============================================================================
// BST Method Implementations
//============================================================================

// Recursively destroys the tree, freeing memory
void BinarySearchTree::destroyTree(unique_ptr<Node>& node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        node.reset();
    }
}

// Validates course ID format (2-4 letters followed by 3+ numbers)
bool BinarySearchTree::isValidCourseId(const string& courseId) const {
    if (courseId.empty() || courseId.length() > 20) return false;

    // Check for valid prefix (letters)
    size_t i = 0;
    while (i < courseId.length() && isalpha(courseId[i])) {
        i++;
    }
    if (i < 2 || i > 4) return false;

    // Check for valid number sequence
    size_t numCount = 0;
    while (i < courseId.length()) {
        if (!isdigit(courseId[i])) return false;
        numCount++;
        i++;
    }
    if (numCount < 3) return false;

    return true;
}

// Ensures all prerequisites exist in the course catalog
void BinarySearchTree::validatePrerequisites(const Course* course) const {
    for (const auto& prereqId : course->prereqs) {
        if (!FindCourse(prereqId)) {
            throw runtime_error("Invalid prerequisite: " + prereqId + " for course " + course->courseId);
        }
    }
}

// Inserts a new course into the BST and course map
void BinarySearchTree::Insert(Course* course) {
    if (!course) {
        throw invalid_argument("Cannot insert null course");
    }

    if (!isValidCourseId(course->courseId)) {
        throw invalid_argument("Invalid course ID format: " + course->courseId);
    }

    // Add to hash map for O(1) lookups
    courseMap[course->courseId] = course;

    // Add to BST for ordered traversal
    if (!root) {
        root = make_unique<Node>(course);
    }
    else {
        addNode(root.get(), course);
    }
}

// Helper function for recursive course insertion
void BinarySearchTree::addNode(Node* node, Course* course) {
    if (course->courseId < node->course->courseId) {
        if (!node->left) {
            node->left = make_unique<Node>(course);
        }
        else {
            addNode(node->left.get(), course);
        }
    }
    else {
        if (!node->right) {
            node->right = make_unique<Node>(course);
        }
        else {
            addNode(node->right.get(), course);
        }
    }
}

// O(1) course lookup using hash map
Course* BinarySearchTree::FindCourse(const string& courseId) const {
    auto it = courseMap.find(courseId);
    return (it != courseMap.end()) ? it->second : nullptr;
}

// Builds graph of course dependencies for prerequisite analysis
void BinarySearchTree::BuildDependencyGraph() {
    // Clear existing dependencies
    for (auto& pair : courseMap) {
        pair.second->dependentCourses.clear();
    }

    // Build new dependency relationships
    for (const auto& pair : courseMap) {
        Course* course = pair.second;
        for (const auto& prereqId : course->prereqs) {
            Course* prereq = FindCourse(prereqId);
            if (prereq) {
                prereq->dependentCourses.push_back(course->courseId);
            }
        }
    }
}

// Recursive DFS to detect cycles in prerequisite relationships
bool BinarySearchTree::hasCycle(Course* course,
    unordered_set<string>& visited,
    unordered_set<string>& recursionStack) {
    if (!course) return false;

    // Mark current course as visited and add to recursion stack
    visited.insert(course->courseId);
    recursionStack.insert(course->courseId);

    // Check all prerequisites for cycles
    for (const auto& prereqId : course->prereqs) {
        Course* prereq = FindCourse(prereqId);
        if (!prereq) continue;

        // If course is in recursion stack, found a cycle
        if (recursionStack.find(prereqId) != recursionStack.end()) {
            return true;
        }

        // Continue DFS if course hasn't been visited
        if (visited.find(prereqId) == visited.end()) {
            if (hasCycle(prereq, visited, recursionStack)) {
                return true;
            }
        }
    }

    // Remove course from recursion stack when backtracking
    recursionStack.erase(course->courseId);
    return false;
}

// Public interface for cycle detection
bool BinarySearchTree::HasPrerequisiteCycle(const string& courseId) {
    Course* course = FindCourse(courseId);
    if (!course) {
        throw invalid_argument("Course not found: " + courseId);
    }

    unordered_set<string> visited;
    unordered_set<string> recursionStack;
    return hasCycle(course, visited, recursionStack);
}

// Helper function for topological sort using DFS
void BinarySearchTree::topologicalSortUtil(Course* course,
    unordered_set<string>& visited,
    stack<Course*>& Stack) {
    visited.insert(course->courseId);

    // Recursively visit all prerequisites
    for (const auto& prereqId : course->prereqs) {
        Course* prereq = FindCourse(prereqId);
        if (!prereq) continue;

        if (visited.find(prereqId) == visited.end()) {
            topologicalSortUtil(prereq, visited, Stack);
        }
    }

    // Push current course to stack after visiting all prerequisites
    Stack.push(course);
}

// Returns prerequisites in order they should be taken
vector<Course*> BinarySearchTree::GetPrerequisiteOrder(const string& courseId) {
    Course* course = FindCourse(courseId);
    if (!course) {
        throw invalid_argument("Course not found: " + courseId);
    }

    // Verify no cycles exist before attempting topological sort
    if (HasPrerequisiteCycle(courseId)) {
        throw runtime_error("Circular prerequisite dependency detected for: " + courseId);
    }

    unordered_set<string> visited;
    stack<Course*> Stack;
    vector<Course*> result;

    // Process each prerequisite
    for (const auto& prereqId : course->prereqs) {
        Course* prereq = FindCourse(prereqId);
        if (!prereq) continue;

        if (visited.find(prereqId) == visited.end()) {
            topologicalSortUtil(prereq, visited, Stack);
        }
    }

    // Convert stack to vector and reverse for proper order
    while (!Stack.empty()) {
        result.push_back(Stack.top());
        Stack.pop();
    }

    reverse(result.begin(), result.end());
    return result;
}

// Validates prerequisites for all courses in the catalog
bool BinarySearchTree::ValidateAllPrerequisites() const {
    for (const auto& pair : courseMap) {
        try {
            validatePrerequisites(pair.second);
        }
        catch (const runtime_error& e) {
            cerr << "Validation error: " << e.what() << endl;
            return false;
        }
    }
    return true;
}

// Displays complete course catalog in alphabetical order
void BinarySearchTree::PrintSampleSchedule() const {
    printSubHeader("Complete Course Catalog");
    cout << "    COURSE ID  | COURSE TITLE" << endl;
    cout << "  " << string(73, '-') << endl;

    if (!root) {
        cout << "    No courses available." << endl;
        return;
    }

    // Perform in-order traversal to print courses alphabetically
    printSampleSchedule(root.get());
    cout << "\n    End of course catalog.\n" << endl;
    printLine();
}

// Helper function for recursive in-order traversal
void BinarySearchTree::printSampleSchedule(const Node* node) const {
    if (node) {
        printSampleSchedule(node->left.get());
        cout << "    " << left << setw(10) << node->course->courseId
            << " | " << node->course->courseTitle << endl;
        printSampleSchedule(node->right.get());
    }
}

// Public interface for course information display
void BinarySearchTree::PrintCourseInformation(const string& courseId) const {
    if (!root) {
        cout << "No courses available." << endl;
        return;
    }
    printCourseInformation(root.get(), courseId);
}

// Displays detailed information for a specific course
void BinarySearchTree::printCourseInformation(const Node* node, const string& courseId) const {
    if (!node) {
        printError("Course " + courseId + " not found");
        return;
    }

    if (courseId == node->course->courseId) {
        // Display course details with consistent formatting
        printSubHeader("Course Details");
        cout << "    Course ID:   " << node->course->courseId << endl;
        cout << "    Title:       " << node->course->courseTitle << endl;
        cout << "    Prerequisites:" << endl;

        // Display prerequisites or "None" if empty
        if (node->course->prereqs.empty()) {
            cout << "        None" << endl;
        }
        else {
            for (const auto& prereq : node->course->prereqs) {
                cout << "        - " << prereq << endl;
            }
        }

        // Display courses that require this course
        cout << "    Required by:" << endl;
        if (node->course->dependentCourses.empty()) {
            cout << "        None" << endl;
        }
        else {
            for (const auto& dep : node->course->dependentCourses) {
                cout << "        - " << dep << endl;
            }
        }
        cout << endl;
        printLine();
        return;
    }

    // Continue searching in appropriate subtree
    if (courseId < node->course->courseId) {
        printCourseInformation(node->left.get(), courseId);
    }
    else {
        printCourseInformation(node->right.get(), courseId);
    }
}

//============================================================================
// File loading function
// Reads course data from a CSV file and populates the BST
//============================================================================

bool loadDataStructure(const string& filepath, BinarySearchTree* bst) {
    // Validate BST pointer
    if (!bst) {
        cout << "  Unable to open BST pointer" << endl;
        return false;
    }

    // Attempt to open input file
    ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        cout << "  Unable to open file: " << filepath << endl;
        return false;
    }

    string line;
    try {
        // Process file line by line
        while (getline(inputFile, line)) {
            if (line.empty()) continue;

            // Parse CSV format into vector
            vector<string> courseInfo;
            size_t start = 0;
            size_t end = line.find(',');

            while (end != string::npos) {
                courseInfo.push_back(line.substr(start, end - start));
                start = end + 1;
                end = line.find(',', start);
            }
            courseInfo.push_back(line.substr(start));

            // Skip invalid lines
            if (courseInfo.size() < 2) continue;

            // Create new course object
            auto course = new Course(courseInfo[0], courseInfo[1]);

            // Add prerequisites if they exist
            for (size_t i = 2; i < courseInfo.size(); ++i) {
                if (!courseInfo[i].empty()) {
                    course->prereqs.push_back(courseInfo[i]);
                }
            }

            // Attempt to insert course into BST
            try {
                bst->Insert(course);
            }
            catch (const invalid_argument& e) {
                cerr << "Error processing file: " << e.what() << endl;
                inputFile.close();
                return false;
            }
        }

        // Build prerequisite relationships after all courses are loaded
        bst->BuildDependencyGraph();

        // Validate all prerequisites
        if (!bst->ValidateAllPrerequisites()) {
            cerr << "Warning: Some prerequisites could not be validated" << endl;
        }

        inputFile.close();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error processing file: " << e.what() << endl;
        inputFile.close();
        return false;
    }
}

//============================================================================
// Main function
// Implements the user interface and program flow control
//============================================================================

int main(int argc, char* argv[]) {
    // Initialize program variables
    string filepath = (argc == 2) ? argv[1] : "infile.txt";
    auto bst = make_unique<BinarySearchTree>();
    string userCourse;
    int choice = 0;

    // Display initial program header
    printLine();

    // Main program loop
    while (choice != 9) {
        displayMainMenu();

        // Validate user input
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input - Please enter a number");
            continue;
        }

        cout << endl;

        try {
            switch (choice) {
            case 1: {  // Load course data
                printSubHeader("Load Course Data");
                printInputPrompt("Enter file path (or press Enter for default 'infile.txt'): ");

                string input;
                cin.ignore();
                getline(cin, input);

                // Handle default file selection
                if (input.empty()) {
                    filepath = "infile.txt";
                    cout << "    Using default file: infile.txt" << endl;
                }
                else {
                    filepath = input;
                }

                cout << "\n    Loading...\n" << endl;

                if (loadDataStructure(filepath, bst.get())) {
                    printSuccess("Course data successfully loaded");
                }
                else {
                    printError("Failed to load course data");
                }
                break;
            }

            case 2:  // Display course catalog
                bst->PrintSampleSchedule();
                break;

            case 3: {  // Search for specific course
                printSubHeader("Course Search");
                printInputPrompt("Enter Course ID: ");
                cin >> userCourse;
                transform(userCourse.begin(), userCourse.end(), userCourse.begin(), ::toupper);
                bst->PrintCourseInformation(userCourse);
                break;
            }

            case 4: {  // Analyze prerequisites
                printSubHeader("Prerequisite Analysis");
                printInputPrompt("Enter Course ID: ");
                cin >> userCourse;
                transform(userCourse.begin(), userCourse.end(), userCourse.begin(), ::toupper);

                try {
                    vector<Course*> prereqOrder = bst->GetPrerequisiteOrder(userCourse);
                    cout << "\n    Prerequisite Sequence for " << userCourse << ":" << endl;
                    cout << "    " << string(50, '-') << endl;

                    if (prereqOrder.empty()) {
                        cout << "    No prerequisites required" << endl;
                    }
                    else {
                        // Display prerequisites in proper sequence
                        for (size_t i = 0; i < prereqOrder.size(); ++i) {
                            cout << "        " << i + 1 << ". " << setw(9) << left
                                << prereqOrder[i]->courseId << "| "
                                << prereqOrder[i]->courseTitle << endl;
                        }
                    }
                    cout << endl;
                }
                catch (const runtime_error& e) {
                    printError(e.what());
                }
                printLine();
                break;
            }

            case 5: {  // Validate prerequisites
                printSubHeader("Prerequisite Validation");
                printInputPrompt("Enter Course ID: ");
                cin >> userCourse;
                transform(userCourse.begin(), userCourse.end(), userCourse.begin(), ::toupper);

                try {
                    Course* course = bst->FindCourse(userCourse);
                    if (!course) {
                        printError("Course not found: " + userCourse);
                        break;
                    }

                    cout << "\n    Validating prerequisites for " << userCourse << "...\n" << endl;

                    // Check prerequisite status
                    if (course->prereqs.empty()) {
                        printSuccess("No prerequisites required - Entry level course");
                    }
                    else if (bst->HasPrerequisiteCycle(userCourse)) {
                        printWarning("Invalid prerequisite structure detected!");
                        cout << "        This course has a circular prerequisite dependency.\n" << endl;
                    }
                    else {
                        printSuccess("Valid prerequisite structure");
                        cout << "        Prerequisites: ";
                        for (const auto& prereq : course->prereqs) {
                            cout << prereq;
                            if (&prereq != &course->prereqs.back()) {
                                cout << ", ";
                            }
                        }
                        cout << "\n" << endl;
                    }
                }
                catch (const exception& e) {
                    printError(e.what());
                }
                printLine();
                break;
            }

            case 9:  // Exit program
                cout << "\n    Thank you for using the Course Management System!\n" << endl;
                printLine();
                break;

            default:
                printError("Invalid selection - Please choose 1-5, or 9 to exit");
                break;
            }

        }
        catch (const exception& e) {
            printError(e.what());
        }
    }

    return 0;
}