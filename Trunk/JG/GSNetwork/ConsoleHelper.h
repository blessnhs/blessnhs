#pragma once

#include <windows.h>

class ConsoleHelper
{
public:
	ConsoleHelper() {}
	~ConsoleHelper() {}

	static void DebugConsoleString(int y,char *message)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&csbi);

		int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

		COORD pos;
		pos.X = (SHORT)(columns - strlen(message));
		pos.Y = (csbi.dwCursorPosition.Y - rows > 0 ? csbi.dwCursorPosition.Y - rows : 0) + 1;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

		printf(message);

		pos.X = csbi.dwCursorPosition.X, pos.Y = csbi.dwCursorPosition.Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
      

	}

};

