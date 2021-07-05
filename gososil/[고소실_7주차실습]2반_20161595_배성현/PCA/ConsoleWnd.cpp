/* This class can be used to re

*/
#include "StdAfx.h"
#include "ConsoleWnd.h"

ConsoleWnd::ConsoleWnd()
{

}

ConsoleWnd::~ConsoleWnd()
{
}

void ConsoleWnd::CreateConsole()
{
	if (debug)
	{
		// This function makes the console window. 
		AllocConsole();     
		HANDLE ostreamhandle;

		ostreamhandle = GetStdHandle(STD_OUTPUT_HANDLE); 

		// This tests that the console is working.  
		// You can use WriteConsole to write anything you want. 
		//WriteConsole(ostreamhandle, "Test", 4, 0, 0); 

		// Redirect unbuffered stdout to the console 
		int outHandle = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		FILE *outFile = _fdopen(outHandle, "w"); 
		*stdout = *outFile; 
		setvbuf(stdout, NULL, _IONBF, 0);      

		// Redirect unbuffered stdin to the console 
		int inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT); 
		FILE *infile = _fdopen(inHandle, "r"); 
		*stdin = *infile; 
		setvbuf(stdin, NULL, _IONBF, 0); 

		//Redirect stderr to the console 
		int errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT); 
		FILE *errfile = _fdopen(errHandle, "w"); 
		*stderr = *errfile; 
		setvbuf(stderr, NULL, _IONBF, 0); 

		// This command makes it so cin, cout, etc work with the new console 
		ios::sync_with_stdio(); 

		cout << "Console Window Started:\n\n";
	}
}