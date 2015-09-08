#include <Windows.h>
//#include <iostream>

int main( int argc, char *argv[] )
{
    HWND taskbarHandle = FindWindow( L"Shell_TrayWnd", L"" );
    HWND startButtonHandle = FindWindow( L"Button", L"Start" );
    BOOL isVisible = IsWindowVisible( taskbarHandle );
    
    //std::cout << "taskbarHandle = " << taskbarHandle << "\n";
    //std::cout << "startButtonHandle = " << startButtonHandle << "\n";
    //std::cout << "isVisible = " << isVisible << " (about to show)\n";

    ShowWindow( taskbarHandle, TRUE );
    ShowWindow( startButtonHandle, TRUE );
}
