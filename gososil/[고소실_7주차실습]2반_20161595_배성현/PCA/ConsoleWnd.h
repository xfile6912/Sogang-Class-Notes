#pragma once

#include <stdio.h> 
#include <iostream>
#include <io.h>
#include <fstream> 
#include <fcntl.h>

using namespace std;

class ConsoleWnd 
{
	
	public:
		ConsoleWnd();
		~ConsoleWnd();
		bool debug;	
		void CreateConsole();
};