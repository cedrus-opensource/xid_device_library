#include "../xid_device_driver/xid_device_scanner_t.h"
#include "../xid_device_driver/xid_con_t.h"
#include "../xid_device_driver/stim_tracker_t.h"
#include "../xid_device_driver/xid_device_t.h"
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
    xid_device_scanner_t scanner;
    char cCurrentPath[FILENAME_MAX];

    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
    std::cout << "Detecting XID devices, stand by." << std::endl;
    scanner.detect_valid_xid_devices(std::string(cCurrentPath) + "/../../../../devconfig");
    if(scanner.rb_device_count() == 0)
    {
        std::cout << "No response devices found, moving on..." << std::endl;
    }
    else
    {
        scanner.response_device_connection_at_index(0)->open_connection();
        std::cout << "Found a " << scanner.response_device_connection_at_index(0)->get_device_name() << ", press any key on it to continue." << std::endl;

        while(!scanner.response_device_connection_at_index(0)->has_queued_responses())
        {
	        scanner.response_device_connection_at_index(0)->poll_for_response();
        }

        scanner.response_device_connection_at_index(0)->close_connection();
        std::cout << "Rad, moving on to the flashy test." << std::endl << std::endl;
		sleep_func(500*sleep_inc);
    }

    boost::shared_ptr<stim_tracker_t> st;
    if(scanner.st_device_count() > 0)
    {
        st = scanner.stimtracker_connection_at_index(0);
        std::cout << "Found a " << st->get_device_name() << std::endl;
        st->open_connection();
    }
    else
        std::cout << "No StimTracker found." << std::endl;

    for (int i = 0; i < scanner.rb_device_count(); i++)
        scanner.response_device_connection_at_index(i)->open_connection();

    std::cout << "Found the following response devices:" << std::endl;
    for (int i = 0; i < scanner.rb_device_count(); i++)
        std::cout << scanner.response_device_connection_at_index(i)->get_device_name() << std::endl;

    std::cout << std::endl;

    if ( st != NULL )
        st->set_pulse_duration(UINT_MAX);

    std::cout << "Raising all lines" << std::endl;

    if ( st != NULL )
        st->raise_lines(255);
    for (int i = 0; i < scanner.rb_device_count(); i++)
    {
        scanner.response_device_connection_at_index(i)->raise_lines(255);
    }
 
    sleep_func(1000*sleep_inc);

    std::cout << "Clearing lines" << std::endl;

    if ( st != NULL )
        st->clear_lines(255);
    for (int i = 0; i < scanner.rb_device_count(); i++)
        scanner.response_device_connection_at_index(i)->clear_lines(255);

    sleep_func(1000*sleep_inc);

    std::cout << "Setting pulse duration to 100ms" << std::endl;

    if ( st != NULL )
        st->set_pulse_duration(100);

    sleep_func(1000*sleep_inc);

    std::cout << "OOOH!! FLASHY!! (Hit Ctrl-C to exit)" << std::endl;

    int line = 1;
    while(true)
    {
        if ( st != NULL )
        st->raise_lines(line);
        for (int i = 0; i < scanner.rb_device_count(); i++)
            scanner.response_device_connection_at_index(i)->raise_lines(line);

        line = line << 1;
        if(line > 255)
            line = 1;
        sleep_func(200*sleep_inc);
    }

    return 0;
}
