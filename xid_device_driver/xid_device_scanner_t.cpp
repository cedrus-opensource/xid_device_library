/* Copyright (c) 2010, Cedrus Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.  
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Cedrus Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xid_device_scanner_t.h"
#include "xid_con_t.h"

#include <boost/shared_ptr.hpp>

cedrus::xid_device_scanner_t::xid_device_scanner_t(void)
{
    load_available_com_ports();
}


cedrus::xid_device_scanner_t::~xid_device_scanner_t(void)
{
}

void cedrus::xid_device_scanner_t::load_available_com_ports()
{
    available_com_ports_.clear();
    for(int i=1; i < MAX_PORTS; ++i)
    {
        wchar_t port_name[10];
        wsprintf(port_name, L"COM%d", i);

        cedrus::xid_con_t conn(port_name);

        int status = conn.open();
        
        if(status == NO_ERR)
        {
            conn.close();
            available_com_ports_.push_back(port_name);
        }
    }
}

int cedrus::xid_device_scanner_t::detect_valid_xid_devices()
{
    load_available_com_ports();

    rb_connections_.clear();
    st_connections_.clear();
    int devices = 0;

    for(std::vector<std::wstring>::iterator iter = available_com_ports_.begin(),
                                              end = available_com_ports_.end();
        iter != end; ++iter)
    {
        bool device_found = false;
        const int baud_rate[] = { 115200, 19200, 9600, 57600, 38400 };
        const int num_bauds   = sizeof(baud_rate)/sizeof(int);
        
        for(int i = 0; i < num_bauds && !device_found; ++i)
        {
            boost::shared_ptr<cedrus::xid_con_t> xid_con(
                new xid_con_t(*iter, baud_rate[i]));

            if(xid_con->open() == NO_ERR)
            {
                char return_info[200];
                xid_con->flush_input();
                xid_con->flush_output();
                xid_con->send_xid_command(
                    "_c1",
                    0,
                    return_info,
                    sizeof(return_info),
                    5,
                    1000,
                    100);

                std::string info;
                if(return_info[0] == NULL)
                {
                    // there's a possibility that the device is in E-Prime mode.
                    // Go through the buffer and discard NULL characters, and
                    // only keep the non NULL characters.
                    for(int j = 0; j < sizeof(return_info); ++j)
                    {
                        if(return_info[j] != NULL)
                        {
                            info.append(&return_info[j], 1);
                        }
                    }
                }
                else
                {
                    info = std::string(return_info);
                }
                
                if(strstr(info.c_str(), "_xid"))
                {
                    // found an XID device
                    ++devices;

                    device_found = true;

                    if(strcmp(info.c_str(), "_xid0") != 0)
                    {
                        // device is not in XID mode.  Currently this library
                        // only supports XID mode so we issue command 'c10' to
                        // set the device into XID mode
                        char empty_return[10];
                        xid_con->send_xid_command(
                            "c10",
                            0,
                            empty_return,
                            sizeof(empty_return),
                            0);

                        xid_con->flush_input();
                        xid_con->flush_output();
                    }

                    char dev_type[10];
                    xid_con->send_xid_command(
                        "_d2",
                        0,
                        dev_type,
                        sizeof(dev_type),
                        1,
                        1000);

                    if(dev_type[0] == 'S')
                    {
                        st_connections_.push_back(xid_con);
                    }
                    else
                    {
                        rb_connections_.push_back(xid_con);
                    }
                }
            }
            
            xid_con->close();
        }
    }

    return devices;
}

boost::shared_ptr<cedrus::xid_con_t> 
cedrus::xid_device_scanner_t::response_device_connection_at_index(unsigned int i)
{
    if(i >= rb_connections_.size())
        return boost::shared_ptr<xid_con_t>();

    return rb_connections_[i];
}

boost::shared_ptr<cedrus::xid_con_t>
cedrus::xid_device_scanner_t::stimtracker_connection_at_index(unsigned int i)
{
    if( i > st_connections_.size())
        return boost::shared_ptr<xid_con_t>();

    return st_connections_[i];
}

int cedrus::xid_device_scanner_t::rb_device_count() const
{
    return rb_connections_.size();
}

int cedrus::xid_device_scanner_t::st_device_count() const
{
    return st_connections_.size();
}