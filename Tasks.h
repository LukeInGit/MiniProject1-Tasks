#ifndef TASKS_H
#define TASKS_H

#include <iostream>
#include <string>
#include <ctime>


//#include "tasks.cpp" currently not in use - deleted, will bring back if any member function becomes non trivial

class Task
{
private:
	std::string m_name{""};
	std::string m_description{""};
	bool m_complete{};
	std::tm m_dueDate;
	std::tm m_dateCreated;

	void PrintDetails(std::string& x)
	{
		std::cout << x << '\n';
	}

	void PrintDetails(std::tm time)
	{
		std::cout << time.tm_year+1900<<"-"<<time.tm_mon+1<<"-"<<time.tm_mday<<'\n';
	}


public:
	Task(std::string name, std::string description, std::tm dueDate, std::tm dateCreated)
		:m_name{ name }, m_description{ description }, m_complete(false), m_dueDate{ dueDate }, m_dateCreated{ dateCreated } //Constructor
	{
		// :)
	}

	void PrintName()
	{
		PrintDetails(m_name);
	}

	void PrintDescription() 
	{
		PrintDetails(m_description);
	}

	void PrintDueDate()
	{
		PrintDetails(m_dueDate);
	}

	void PrintDateCreated()
	{
		PrintDetails(m_dateCreated);
	}

	std::string GetName() const
	{
		return m_name;
	}

	std::string GetDescription() const
	{
		return m_description;
	}

	bool GetStatus() const
	{
		return m_complete;
	}

	void SetStatus(bool status)
	{
		m_complete = status;
	}

	std::tm GetDueDate() const
	{
		return m_dueDate;
	}

	std::tm GetDateCreated() const
	{
		return m_dateCreated;
	}


	void MarkComplete()
	{
		m_complete = true;
	}

	void MarkIncomplete()
	{
		m_complete = false;
	}

};


#endif