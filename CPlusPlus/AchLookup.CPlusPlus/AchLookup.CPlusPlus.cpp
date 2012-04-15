// AchLookup.CPlusPlus.cpp : Defines the entry point for the console application.
//

//to manipulate the file
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

//other fucntion declarations
int create_database();

//does this leak memory?
//not quite sure i understand how it doesn't change the string
void trim_trailing_whitespace(char* last_character)
{
	char c = *last_character;
	while(isspace(c)) 
	{
		last_character--;
		c = *last_character;
	}
	last_character[1] = '\0';
}

void process_file() 
{
	auto file_name = "c:\\code\\achlookup\\fpddir.txt";
	std::ifstream ach_file;
	ach_file.open(file_name);
	int count = 0;
	if(!ach_file.bad())
	{
		char routing_number[9];
		char bank_name[37];

		while(!ach_file.eof()) 
		{
			if(count > 100) 
			{
				break;
			}
			
			//read the routing number
			ach_file >> std::setw(9) >> routing_number;
			std::cout << routing_number << std::endl;

			//advance past the 2nd column to get to the bank name
			ach_file.ignore(18);

			//read the bank number
			ach_file.get(bank_name,37);

			//remove whitespace at end of bank name
			//char* newEnd = std::remove_if(bank_name,bank_name + 37,isspace);
			trim_trailing_whitespace(&bank_name[35]);

			//advance to next line
			ach_file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

			std::cout << "|" << routing_number << " - " << bank_name << "|" << std::endl;

			count++;
		}
		ach_file.close();
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	//process_file();
	create_database();
	return 0;
}

