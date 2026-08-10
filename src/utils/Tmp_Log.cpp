#include <iostream>
#include <iomanip>

void    nl(int n)
{
    for (int i = 0; i < n; i++)
        std::cout << std::endl;
}

void    printTitle(std::string title)
{
    std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "                     " << title << std::endl;
	std::cout << "--------------------------------------------------" << std::endl << std::endl;
}

void    printLog(std::string logLevel, std::string message)
{
    std::cout << "[" << logLevel << std::setw(2) << "] " << message << std::endl;
}

void    printSection(std::string section)
{
    std::cout << "===== " << section << " =====\n\n";
}