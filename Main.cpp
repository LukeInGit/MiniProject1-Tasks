#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <ctime> //for time
#include <chrono>
#include <iomanip>
#include <sstream> //^
#include <fstream>
#include "nlohmann/json.hpp"//nlohmann 
#include "Tasks.h"
#include "MainDeclarations.h"

void ignoreLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

enum MenuChoices
{
    CREATE_TASK = 1,
    VIEW_ALL_TASKS,
    DELETE_TASK,
    SET_STATUS,
    SAVE,
    EXIT,
};

void ShowMenu()
{
    std::cout << "\n\nWhat would you like to do?\n";
    std::cout << CREATE_TASK<<". Create a task\n";
    std::cout << VIEW_ALL_TASKS <<". View all tasks\n";
    std::cout << DELETE_TASK<<". Delete a task\n";
    std::cout << SET_STATUS<<". Edit task status\n";
    std::cout << SAVE<<". Save\n";
    std::cout << EXIT<<". Save and Exit\n\n";
}

void DisplayTasks(std::vector<Task>& taskArray)
{
    std::cout << "\n========================================================================\n";//makes it look prettier

    //Potentially overload an operator to print task class instead
    int id{ 1 }; //for use in accessing individual tasks i.e picking which task to delete/mark as complete
    for(auto task : taskArray)
    {
        std::cout << "\nArray Number: " << id<<'\n'; 
        id++;
        std::cout << "Date created: ";
        task.PrintDateCreated();
        std::cout << "Due: ";
        task.PrintDueDate();
        std::cout << "Task Name: ";
        task.PrintName();

        std::cout << "Description: ";
        task.PrintDescription();
        std::cout << '\n';
        std::cout << "Status: ";
        std::cout << (task.GetStatus() ? "complete" : "incomplete");
        std::cout << '\n';


    }
    std::cout << "========================================================================\n";
}

int getArrayIndex()
{
    std::cout << "Enter the array number you wish to modify: ";
    return GetUserInt() - 1;//-1 needed as index is displayed as index+1 for readability
}

void DeleteTask(std::vector<Task>& taskArray)
{


    int index{ getArrayIndex()};

    if (index >= 0 && index < taskArray.size())
    {
        taskArray.erase(taskArray.begin() + index);
    }
    else
    {
        std::cout << "\nInvalid input\n";

    }
}

void SetStatus(std::vector<Task>& taskArray)
{
    int index{ getArrayIndex() };

    std::cout << "Mark as (1.Complete | 2.Incomplete): ";

    int input{ GetUserInt()};

    if (input == 1)
    {
        taskArray[index].SetStatus(true);
    }
    else if (input == 2)
    {
        taskArray[index].SetStatus(false);
    }
    else
    {
        std::cout << "Invalid input\n";
    }

}

int GetUserInt()
{
    bool retry{ false };
    std::string str{};

    do {
        str = GetInput() ;
        retry = false;
        for (char c : str) {
            if (!std::isdigit(c)) {
                std::cout << "Non-digit character found, try again\n";
                retry = true;
                break;
            }
        }
        // prevent integer overflow for std::stoi
        if (str.length() > std::to_string(std::numeric_limits<int>::max()).length()-1) {
            std::cout << "Integer too large, try again\n";
            retry = true;
        }
    } while (retry);
    int userint{ std::stoi(str)};

    return userint;
}

std::tm GetTime()
{

    //get time as big number from system clock
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Convert the current time number to a tm structure
    std::tm currentDate;
    localtime_s(&currentDate, &currentTime);

    return currentDate;

}

std::tm GetDueDate()
{
    std::tm currentDate(GetTime());
    std::tm dueDate{};

    do {
        std::cout << "Enter the due date.\n" << "Year due (YYYY format): ";
        dueDate.tm_year = GetUserInt() - 1900;
    } while (dueDate.tm_year < currentDate.tm_year || dueDate.tm_year>9999);

    do {
        std::cout << "Enter the due date.\n" << "Month due (MM format): ";
        dueDate.tm_mon = GetUserInt() - 1;
    } while (dueDate.tm_mon <= -1 || dueDate.tm_mon > 11);

    do {
        std::cout << "Enter the due date.\n" << "Day due (DD format):  ";
        dueDate.tm_mday = GetUserInt();
    } while (dueDate.tm_mday < 1 || dueDate.tm_mday>31);

    return dueDate;

}

//for being able to save dates to JSON
std::string TmToString(const std::tm& timeStruct) {
    std::ostringstream oss;
    oss << std::put_time(&timeStruct, "%Y-%m-%d");
    return oss.str();
}

//for being able to read dates from JSON
std::tm StringToTm(const std::string& str) {
    std::tm timeStruct = {};
    std::istringstream iss(str);
    iss >> std::get_time(&timeStruct, "%Y-%m-%d");
    return timeStruct;
}

int main()
{
    std::vector<Task> taskArray{};
    int input{};

    LoadSave(taskArray);
    bool run{ true };
    do
    {
        ShowMenu();

        input = GetUserInt();
        MenuChoices Choice{ static_cast<MenuChoices>(input) };

        switch (Choice) {
        case CREATE_TASK:
            CreateTask(taskArray);
            break;
        case VIEW_ALL_TASKS:
            DisplayTasks(taskArray);
            break;
        case DELETE_TASK:
            DeleteTask(taskArray);
            break;
        case SET_STATUS:
            SetStatus(taskArray);
            break;
        case SAVE:
            Save(taskArray);
            break;
        case EXIT:
            run = false;
            break;

        default://error handling
            std::cout << "Not a valid option\n";
        }



    } while (run);

    Save(taskArray);
    return 0;
}

void LoadSave(std::vector<Task>& taskArray)
{
    using json = nlohmann::json;

    std::ifstream f("task.json");

    if (f.is_open())
    {
        json data;
        f >> data;

        for (std::size_t i{ 0 }; i < data.size(); i++)
        {
            std::string f_name{ data[i]["name"] };
            std::string f_description{ data[i]["description"] };
            std::tm f_dueDate{ StringToTm(data[i]["due_date"]) };
            std::tm f_dateCreated{ StringToTm(data[i]["date_created"]) };
            bool f_status{ data[i]["status"] };

            Task temp(f_name, f_description, f_dueDate, f_dateCreated);
            if (f_status)
            {
                temp.SetStatus(f_status);
            }

            taskArray.push_back(temp);
        }


    }
    else
    {
        std::cerr << "Error loading or first time operation";
    }
}

void Save(std::vector<Task>& taskArray)
{

    std::cout << "saving...\n";
    nlohmann::json saveFile;

    for(auto task : taskArray)
    {

        saveFile.push_back({
            {"name", task.GetName()},
            {"description", task.GetDescription()},
            {"status", task.GetStatus()},
            {"due_date", TmToString(task.GetDueDate())},
            {"date_created", TmToString(task.GetDateCreated())},
            });
    }

    std::ofstream fileOut("task.json");
    if (fileOut.is_open())
    {
        fileOut << std::setw(4) << saveFile;  // Pretty print with indentation
        fileOut.close();
        std::cout << "saved to \"task.json\"" << std::endl;
    }
    else
    {
        std::cerr << "Error saving";
    }
}

std::string GetInput()
{
    std::string input{};
    std::getline(std::cin, input);
    while (true)
    {
        // Check for failed extraction
        if (!std::cin) // if the previous extraction failed
        {
            if (std::cin.eof()) // if the stream was closed
            {
                exit(0); // shut down the program 
            }


            std::cin.clear(); 
            ignoreLine();     

            std::cout << "Invalid. Try again: ";
        }
        else if (input.empty()|| (!std::cin || input.empty() || (std::find_if(input.begin(), input.end(), [](char c) { return !std::isspace(c); }) == input.end()))) {
            //If input is empty or whitespace
            std::cout << "Input can't be empty. Try again: ";
            std::getline(std::cin, input);
            std::cin.clear();
        }
        else
        {
            return input;
        }
    }
}

void CreateTask(std::vector<Task>& taskArray)
{
    std::string name{};
    std::string description{};

    std::cout << "Enter task name: ";
    name = GetInput();
    std::cout << "Enter the task details: ";
    description = GetInput();

    Task todo(name,description,GetDueDate(), GetTime());
    taskArray.push_back(todo);

}
