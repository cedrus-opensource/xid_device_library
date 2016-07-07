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

#ifndef XID_DEVICE_CONFIG_T_H
#define XID_DEVICE_CONFIG_T_H

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    struct device_port
    {
        device_port():
            port_name(""),
            port_number(-1),
            number_of_lines(-1),
            is_response_port(false)
        {
            // initialize all mappings to -1
            memset( key_map, -1, sizeof(key_map) );
        }

        std::string port_name;
        int port_number;
        int number_of_lines;
        int key_map[8];
        bool is_response_port;
    };

    /**
     * @class xid_device_config_t xid_device_config_t.h "xid_device_driver/xid_device_config.h"
     *
     * @brief device configuration class
     */
    class CEDRUS_XIDDRIVER_IMPORTEXPORT xid_device_config_t
    {
    private:
        xid_device_config_t(boost::property_tree::ptree * pt);

    public:
        // This is exported for testing purposes only!
        static boost::shared_ptr<xid_device_config_t> config_for_device(boost::property_tree::ptree * pt);

        ~xid_device_config_t(void);

        /**
         * Gets the actual button pressed based on the devconfig key mapping
         *
         * @param[in] key key reported by the response pad
         * @returns the mapped key number based on the .devconfig file.
         */
        int get_mapped_key(int port, int key) const;

        /**
         * Returns the name of the device
         *
         * @returns name of the device
         */
        std::string get_device_name() const;

        /**
         * product id of the device.
         *
         * @returns product id
         * 0: Lumina LP-400 response pad system
         * 1: SV-1 voice key system
         * 2: RB series response pad.
         */
        int get_product_id() const;

        /**
         * model ID of the device.
         *
         * This is generally only valid on RB series response pads.
         *
         * @returns model id of the device
         * 1: RB-530
         * 2: RB-730
         * 3: RB-830
         * 4: RB-834
         */
        int get_model_id() const;

        int get_num_lines_on_port(int port) const;

        const std::vector<device_port> * get_vector_of_ports() const;

        const cedrus::device_port * get_port_ptr_by_index(unsigned int portNum) const;

        bool does_config_match_device( int device_id, int model_id, int major_firmware_ver ) const;

    public:
        std::string m_device_name;
        int m_major_firmware_ver;
        int m_product_id;
        int m_model_id;

        std::vector<device_port> m_device_ports;
    };
} // namespace cedrus

#endif // XID_DEVICE_CONFIG_T
