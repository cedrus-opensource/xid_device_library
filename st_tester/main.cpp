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
    scanner.detect_valid_xid_devices(std::string(cCurrentPath) + "/../../../../devconfig");
    if(scanner.st_device_count() == 0)
    {
        std::cout << "No devices found" << std::endl;
        return 1;
    }

    boost::shared_ptr<stim_tracker_t> st = scanner.stimtracker_connection_at_index(0);
    st->open_connection();

    for (int i = 0; i < scanner.rb_device_count(); i++)
        scanner.response_device_connection_at_index(i)->open_connection();

    std::cout << st->get_device_name() << std::endl;

    for (int i = 0; i < scanner.rb_device_count(); i++)
        std::cout << scanner.response_device_connection_at_index(i)->get_device_name() << std::endl;

    st->set_pulse_duration(UINT_MAX);

    std::cout << "Raising all lines" << std::endl;

    st->raise_lines(255);
    for (int i = 0; i < scanner.rb_device_count(); i++)
        scanner.response_device_connection_at_index(i)->raise_lines(255);
 
    sleep_func(1000*sleep_inc);

    std::cout << "Clearing lines" << std::endl;

    st->clear_lines(255);
    for (int i = 0; i < scanner.rb_device_count(); i++)
        scanner.response_device_connection_at_index(i)->clear_lines(255);

    sleep_func(1000*sleep_inc);

    std::cout << "Setting pulse duration to 100ms" << std::endl;

    st->set_pulse_duration(100);

    sleep_func(1000*sleep_inc);

    std::cout << "OOOH!! FLASHY!! (Hit Ctrl-C to exit)" << std::endl;

    int line = 1;
    while(true)
    {
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
