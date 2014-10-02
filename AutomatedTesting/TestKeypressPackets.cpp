#include <gtest/gtest.h>

#include <string>
#include <boost/property_tree/ptree.hpp>

#include "response_mgr.h"
#include "xid_con_test_only.h"
#include "xid_device_config_t.h"

class TestKeypressPackets : public testing::Test
{
protected:
	virtual void SetUp()
	{
        dummy_device_port.port_name = "Test Port";
        dummy_device_port.port_number = 0;
        dummy_device_port.number_of_lines = 8;
        dummy_device_port.key_map[0] = 1;
        dummy_device_port.key_map[1] = 2;
        dummy_device_port.key_map[2] = 3;
        dummy_device_port.key_map[3] = 4;
        dummy_device_port.key_map[4] = 5;
        dummy_device_port.key_map[5] = 6;
        dummy_device_port.key_map[6] = 7;
        dummy_device_port.key_map[7] = 8;
	}

	virtual void TearDown()
	{
		//delete logNull;
	}

	cedrus::device_port dummy_device_port;

};



TEST_F( TestKeypressPackets, TestRejectInvalidFile_01 )
{
    int x = 0;
    int y = 1;

    // if you two FAILING ones, you'll see two failures.
    EXPECT_FALSE( x == y ); // make 1 fail, then the other.
}

TEST_F( TestKeypressPackets, TestSingleValidKeypress )
{
    char test_packet[6];
    test_packet[0] = 'k';
    test_packet[1] = 240;
    test_packet[2] = '1';
    test_packet[3] = '1';
    test_packet[4] = '1';
    test_packet[5] = 0;

    boost::shared_ptr<cedrus::xid_con_test_only> dummy_connection( new cedrus::xid_con_test_only(test_packet, sizeof(test_packet)) );

    boost::property_tree::ptree pt;
    boost::shared_ptr<cedrus::xid_device_config_t> dummy_config(cedrus::xid_device_config_t::config_for_device(&pt));
    dummy_config->m_device_ports.insert(std::make_pair(0, dummy_device_port));

    cedrus::response_mgr m_response_mgr;

    m_response_mgr.check_for_keypress(dummy_connection, dummy_config);
    cedrus::response res = m_response_mgr.get_next_response();

    EXPECT_TRUE(res.key == 8);
}
