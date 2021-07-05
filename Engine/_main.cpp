#include <iostream>
#include <Application/redtea_app.h>


using namespace redtea;
int main(int argc, char *argv[])
{
	RedteaApp& app = RedteaApp::GetInstance();
	app.Initialize();
	app.Run();
	return 0;
}