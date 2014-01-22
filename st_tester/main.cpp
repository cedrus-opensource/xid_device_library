#include "../xid_device_driver/xid_device_scanner_t.h"
#include "../xid_device_driver/xid_con_t.h"
#include "../xid_device_driver/stim_tracker_t.h"
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace cedrus;

int main(int argc, char** argv)
{
    xid_device_scanner_t scanner;
    scanner.detect_valid_xid_devices();
    if(scanner.st_device_count() == 0)
    {
        std::cout << "No StimTracker found" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "StimTracker found!" << std::endl;
    }
    
    boost::shared_ptr<xid_con_t> con = 
        scanner.stimtracker_connection_at_index(0);
    con->open();

    stim_tracker_t st(con);

    std::cout << st.get_device_name() << std::endl;

    st.set_pulse_duration(UINT_MAX);

    std::cout << "Raising all lines" << std::endl;

    st.raise_lines(255);
    
    Sleep(1000);

    std::cout << "Clearing lines" << std::endl;

    st.clear_lines(255);
    Sleep(1000);

    std::cout << "Setting pulse duration to 100ms" << std::endl;

    st.set_pulse_duration(100);

    Sleep(1000);

    std::cout << "OOOH!! FLASHY!! (Hit Ctrl-C to exit)" << std::endl;

    int line = 1;
    while(true)
    {
        st.raise_lines(line);

        line = line << 1;
        if(line > 255)
            line = 1;
        Sleep(200);
    }

    return 0;
}