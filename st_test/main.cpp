#include "../xid_device_driver/xid_device_scanner_t.h"
#include "../xid_device_driver/base_device_t.h"
#include <boost/shared_ptr.hpp>
#include <iostream>

#ifdef __APPLE__
#   include <unistd.h>
#   define GetCurrentDir getcwd
#   define sleep_func usleep
#   define sleep_inc 1000
#elif defined(_WIN32)
#   include <windows.h>
#   include <direct.h>
#   define GetCurrentDir _getcwd
#   define sleep_func Sleep
#   define sleep_inc 1
#endif

using namespace cedrus;

int main(int argc, char** argv)
{
    char cCurrentPath[FILENAME_MAX];
    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));

    xid_device_scanner_t scanner(std::string(cCurrentPath) + "/../../../../devconfig");

    std::cout << "Detecting XID devices, stand by." << std::endl;
    scanner.detect_valid_xid_devices();

    if(scanner.device_count() == 0)
    {
        std::cout << "No devices found, returning..." << std::endl;
        return 0;
    }
    /*
    else
    {
        scanner.device_connection_at_index(0)->open_connection();
        std::cout << "Found a " << scanner.device_connection_at_index(0)->get_config()->get_device_name() << ", press any key on it to continue." << std::endl;

        //while(!scanner.device_connection_at_index(0)->has_queued_responses())
        {
	        scanner.device_connection_at_index(0)->poll_for_response();
        }

        scanner.device_connection_at_index(0)->close_connection();
        std::cout << "Rad, moving on to the flashy test." << std::endl << std::endl;
		sleep_func(500*sleep_inc);
    }
    */

    std::cout << "Found the following devices:" << std::endl;
    for (int i = 0; i < scanner.device_count(); i++)
        std::cout << scanner.device_connection_at_index(i)->get_device_config()->get_device_name() << std::endl;

    for (int i = 0; i < scanner.device_count(); i++)
        scanner.device_connection_at_index(i)->open_connection();

    std::cout << "Raising all lines" << std::endl;

    for (int i = 0; i < scanner.device_count(); i++)
        scanner.device_connection_at_index(i)->raise_lines(255);
 
    sleep_func(1000*sleep_inc);

    std::cout << "Clearing lines" << std::endl;

    for (int i = 0; i < scanner.device_count(); i++)
        scanner.device_connection_at_index(i)->clear_lines();

    sleep_func(1000*sleep_inc);

    for (int i = 0; i < scanner.device_count(); i++)
        scanner.device_connection_at_index(i)->set_pulse_duration(100);

    sleep_func(1000*sleep_inc);

    std::cout << "OOOH!! FLASHY!! (Hit Ctrl-C to exit)" << std::endl;

    int line = 1;
    while(true)
    {
        for (int i = 0; i < scanner.device_count(); i++)
            scanner.device_connection_at_index(i)->raise_lines(line);

        line = line << 1;
        if(line > 255)
            line = 1;
        sleep_func(200*sleep_inc);
    }

    return 0;
}
