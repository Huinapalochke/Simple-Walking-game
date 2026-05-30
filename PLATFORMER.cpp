/*
File: PLATFORMER.cpp
Purpose:
  A minimal console "platformer" that loads a map from `map.txt` and renders it
  in the console. The player's position is stored as 'P' in the map encoding
  and is displayed as '&' on screen. Player movement is controlled with the
  arrow keys (VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN).

Map encoding (expected characters in `map.txt`):
  This info you can read in map.txt or Readme

Key variables:
  - std::string MAPCODE  : raw, encoded contents of `map.txt`.
  - std::string ReadyMap : rendered string that is printed to the console.
  - int PlrSpeed         : delay in milliseconds between moves.
  - int LineStep         : number of characters that correspond to one map row
						  (used as index offset for up/down movement).
  - bool CountingMAP     : temporary flag used while computing LineStep.

High-level behavior:
  1. Read entire `map.txt` into `MAPCODE`. If the file cannot be opened,
	 `MAPCODE` is set to "?" and the program later shows an error message and exits.
  2. Compute `LineStep` using a simple scan: count consecutive '#' characters,
	 then add 2 when a non-'#' is found. (This heuristic is fragile — see notes.)
  3. Enter the main loop:
	 - Convert `MAPCODE` into `ReadyMap` by replacing encoded symbols with display symbols.
	 - Clear the console and print `ReadyMap`.
	 - Check arrow key states with `GetAsyncKeyState` and attempt to move 'P'
	   by swapping characters in `MAPCODE` (left/right by ±1, up/down by ±LineStep).
	 - Sleep briefly after a successful move to control player speed.
*/

#include <iostream>
#include <list>
#include <windows.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <string>
#include <iterator>
#include <regex>

int main() {

	// Raw encoded map loaded from `map.txt`
	std::string MAPCODE = "";

	// Attempt to open the map file from the working directory
	std::ifstream inFile("map.txt");

	if (inFile.is_open()) {
		// Read the entire file into MAPCODE
		MAPCODE.assign(std::istreambuf_iterator<char>(inFile), {});
		inFile.close();
	}
	else
	{
		// Marker used later to show a load error
		MAPCODE = "?";
	}

	// Renderable version of the map (human-readable)
	std::string ReadyMap = "";

	// Player movement delay in milliseconds
	int PlrSpeed = 100;

	// LineStep: number of characters that represent one map row in MAPCODE
	int LineStep = 0;
	bool CountingMAP = true;

	// Simple heuristic to determine LineStep:
	// count '#' characters until a different character appears, then add 2.
	for (size_t i = 0; i < MAPCODE.length(); i++) {
		if (CountingMAP == false) {
			break;
		}
		if (MAPCODE[i] == '#') {
			LineStep += 1;
		}
		else
		{
			CountingMAP = false;
			LineStep += 2;
		}
	}

	// Debug output of computed LineStep
	std::cout << std::endl << LineStep;

	// Main loop
	while (true) {

		//--------------------------------------------------------------------------------------//

		// RE-checking if any changes on the map in live time

		std::ifstream inFile("map.txt");

		if (inFile.is_open()) {
			MAPCODE.assign(std::istreambuf_iterator<char>(inFile), {});
			inFile.close();
		}

		//--------------------------------------------------------------------------------------//

		ReadyMap = "";
		// Convert encoded characters to their display equivalents
		for (size_t i = 0; i < MAPCODE.length(); i++) {
			if (MAPCODE[i] == '#') {
				ReadyMap += "#";
			}
			else if (MAPCODE[i] == '^') {
				ReadyMap += "\n";
			}
			else if (MAPCODE[i] == '-') {
				ReadyMap += " ";
			}
			else if (MAPCODE[i] == 'P') {
				ReadyMap += "&";
			}
			else if (MAPCODE[i] == '!') {
				ReadyMap += "[  Use arrows to control Player  ]";
			}
			else if (MAPCODE[i] == '?') {
				// Map load failed earlier; notify user and exit
				MessageBoxA(NULL, "map.txt Not found, Can't load!", "ERROR", MB_OK | MB_ICONERROR);
				exit(-1);
			}
			else if (MAPCODE[i] == '+') {
				// Treat '+' as end-of-map marker for rendering
				break;
			}
		}
		// Clear console and render the map
		system("cls");
		std::cout << ReadyMap;

		// Move right
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			size_t PlayerPos = MAPCODE.find("P");
			if (MAPCODE[PlayerPos + 1] != '#') {
				std::swap(MAPCODE[PlayerPos], MAPCODE[PlayerPos + 1]);
				Sleep(PlrSpeed / 1.9);
			}
		}
		// Move left
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			size_t PlayerPos = MAPCODE.find("P");
			if (MAPCODE[PlayerPos - 1] != '#') {
				std::swap(MAPCODE[PlayerPos], MAPCODE[PlayerPos - 1]);
				Sleep(PlrSpeed / 1.9);
			}
		}
		// Move down (offset by LineStep)
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			size_t PlayerPos = MAPCODE.find("P");
			if (MAPCODE[PlayerPos + LineStep] != '#') {
				std::swap(MAPCODE[PlayerPos], MAPCODE[PlayerPos + LineStep]);
				Sleep(PlrSpeed);
			}
		}
		// Move up (offset by LineStep)
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			size_t PlayerPos = MAPCODE.find("P");
			if (MAPCODE[PlayerPos - LineStep] != '#') {
				std::swap(MAPCODE[PlayerPos], MAPCODE[PlayerPos - LineStep]);
				Sleep(PlrSpeed);
			}
		}
	}

}
