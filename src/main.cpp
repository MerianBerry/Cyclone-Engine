#include "Selroc.h"
base_


int main(int argc, char* argv[])
{
	SelrocEng engine_H;

	engine_H.init("LunarGE : Starting");
	
	
	string new_title = "LunarGE "; new_title+=LUNARGE_V;
	engine_H.change_title(new_title.c_str());
	engine_H.run();

	engine_H.terminate();
	
	cout << "\nLunarGE elapsed time:: " << SELR::run_time(engine_H.start_time, engine_H.end_time).seconds << 's';
	char closing[1];
	cout << "\nEnter a character to continue... ";
	std::cin >> closing;
	return 0;
}