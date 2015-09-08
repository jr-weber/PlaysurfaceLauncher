#include <Windows.h>
//#include <iostream>

int main( int argc, char *argv[] )
{
    HWND taskbarHandle = FindWindow( L"Shell_TrayWnd", L"" );
    HWND startButtonHandle = FindWindow( L"Button", L"Start" );
    ShowWindow( taskbarHandle, FALSE );
    ShowWindow( startButtonHandle, FALSE );
}
