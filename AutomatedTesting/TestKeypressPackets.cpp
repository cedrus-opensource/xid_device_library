#include <gtest/gtest.h>

#include <string>
#include <boost/property_tree/ptree.hpp>

#include "response_mgr.h"
#include "xid_con_test_only.h"
#include "xid_device_config_t.h"

#include "CedrusAssert.h"

class TestKeypressPackets : public testing::Test
{
protected:
	virtual void SetUp()
	{
        m_dummyDevicePort.port_name = "Test Port";
        m_dummyDevicePort.port_number = 0;
        m_dummyDevicePort.number_of_lines = 8;
        m_dummyDevicePort.key_map[0] = 1;
        m_dummyDevicePort.key_map[1] = 2;
        m_dummyDevicePort.key_map[2] = 3;
        m_dummyDevicePort.key_map[3] = 4;
        m_dummyDevicePort.key_map[4] = 5;
        m_dummyDevicePort.key_map[5] = 6;
        m_dummyDevicePort.key_map[6] = 7;
        m_dummyDevicePort.key_map[7] = 8;

        boost::property_tree::ptree pt;
        m_dummyConfig = cedrus::xid_device_config_t::config_for_device(&pt);
        m_dummyConfig->m_device_ports.insert(std::make_pair(0, m_dummyDevicePort));
    }

	virtual void TearDown()
	{
		//delete logNull;
	}

    static cedrus::response_mgr m_responseMgr;
    static boost::shared_ptr<cedrus::xid_con_test_only> m_dummyConnection;
    boost::shared_ptr<cedrus::xid_device_config_t> m_dummyConfig;
    cedrus::device_port m_dummyDevicePort;
};

cedrus::response_mgr TestKeypressPackets::m_responseMgr;
boost::shared_ptr<cedrus::xid_con_test_only> TestKeypressPackets::m_dummyConnection( new cedrus::xid_con_test_only(NULL, 0) );

// Read in a single valid keypress. It should resolve to "Port 0, Button 8 Pressed"
TEST_F( TestKeypressPackets, TestSingleValidKeypress )
{
    char testPacket[6];
    testPacket[0] = 'k';
    testPacket[1] = 240;
    testPacket[2] = '1';
    testPacket[3] = '1';
    testPacket[4] = '1';
    testPacket[5] = 0;

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == true);
}

// Read in a single valid key press and release. It should resolve to:
// "Port 0, Button 8 Pressed"
// "Port 0, Button 8 Released"
TEST_F( TestKeypressPackets, TestSingleValidKeyPressRelease )
{
    char testPacket[12];
    testPacket[0] = 'k';
    testPacket[1] = 240;
    testPacket[2] = '1';
    testPacket[3] = '1';
    testPacket[4] = '1';
    testPacket[5] = 0;
    testPacket[6] = 'k';
    testPacket[7] = 224;
    testPacket[8] = '1';
    testPacket[9] = '1';
    testPacket[10] = '1';
    testPacket[11] = 0;

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == true);

    res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == false);
}

// Read in a single keypress. While it is would appear valid otherwise, it fails to meet the
// "6th byte must be NULL". Unfortunately, this will happen whenever a device has been connected
// for longer than 4.66 hours.
TEST_F( TestKeypressPackets, TestSingleInvalidKeypress )
{
    Cedrus::Suppress_All_Assertions();

    char testPacket[7];
    testPacket[0] = 'k';
    testPacket[1] = 240;
    testPacket[2] = '1';
    testPacket[3] = '1';
    testPacket[4] = '1';
    testPacket[5] = '1';
    testPacket[6] = 0;

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_FALSE(TestKeypressPackets::m_responseMgr.has_queued_responses());

    Cedrus::UnSuppress_All_Assertions();
}

// I've never seen something like this happen, but in case of some freak garbled output,
// we want to be able to salvage at least one complete packet. NOTE: this test is being
// slightly thrown out of what by the previous one, as there's a leftover 0 in the
// buffer that we have to flush out.
TEST_F( TestKeypressPackets, TestOverlappingKeypressSimultaneous )
{
    Cedrus::Suppress_All_Assertions();

    char testPacket[9];
    testPacket[0] = 'k'; // incomplete packet start
    testPacket[1] = 240;
    testPacket[2] = '1';
    testPacket[3] = 'k'; // complete packet start
    testPacket[4] = 224;
    testPacket[5] = '1';
    testPacket[6] = '1';
    testPacket[7] = '1';
    testPacket[8] = 0;

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    // This extra check is now needed to 'complete' this attempt to read in a packet
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == false);

    Cedrus::UnSuppress_All_Assertions();
}

/*
 This is more garbled packets, but with a small twist. 0 is actually a
 valid second byte, corresponding to "Port 0, Key 0 Pressed". Even
 though the packet starting at [4] is the valid one, the parser will
 mis-interpret it as a part of the packet at [0] and return a response
 of "Port 0, Key 8 Pressed" with an odd timestamp. There is nothing
 we can do in this case, but we want it logged here for posterity.
*/
TEST_F( TestKeypressPackets, TestOverlappingKeypressTricky )
{
    Cedrus::Suppress_All_Assertions();

    char testPacket[10];
    testPacket[0] = 'k'; // incomplete packet start
    testPacket[1] = 240;
    testPacket[2] = '1';
    testPacket[3] = '1';
    testPacket[4] = 'k'; // complete packet start
    testPacket[5] = 0;
    testPacket[6] = '1';
    testPacket[7] = '1';
    testPacket[8] = '1';
    testPacket[9] = 0;

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == true);

    Cedrus::UnSuppress_All_Assertions();
}

// This is something that can and does happen and steps should be taken to
// mitigate it when the library is used. There might be something in the buffer
// when we go to look for keypresses, and the response manager will eat it.
TEST_F( TestKeypressPackets, TestValidKeypressWithJunkSimultaneous )
{
    Cedrus::Suppress_All_Assertions();

    char testPacket[22];
    testPacket[0] = '_'; // return from a _c1
    testPacket[1] = 'x';
    testPacket[2] = 'i';
    testPacket[3] = 'd';
    testPacket[4] = '0';
    testPacket[5] = 'k'; // complete packet start
    testPacket[6] = 224;
    testPacket[7] = '1';
    testPacket[8] = '1';
    testPacket[9] = '1';
    testPacket[10] = 0;
    testPacket[11] = '_'; // return from a _lr
    testPacket[12] = 'l';
    testPacket[13] = 'r';
    testPacket[14] = '0';
    testPacket[15] = 'k'; // complete packet start
    testPacket[16] = 0;
    testPacket[17] = '1';
    testPacket[18] = '1';
    testPacket[19] = '1';
    testPacket[20] = 0;
    testPacket[21] = 'd';

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket, sizeof(testPacket));

    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);

    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == false);

    res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 1);
    EXPECT_TRUE(res.was_pressed == false);

    Cedrus::UnSuppress_All_Assertions();
}

// This is something that can and does happen and steps should be taken to
// mitigate it when the library is used. There might be something in the buffer
// when we go to look for keypresses, and the response manager will eat it.
// This is a variation with data coming in slowly over time.
TEST_F( TestKeypressPackets, TestValidKeypressWithJunkGradual )
{
    Cedrus::Suppress_All_Assertions();

    char testPacket1[8];
    testPacket1[0] = '_'; // return from a _c1
    testPacket1[1] = 'x';
    testPacket1[2] = 'i';
    testPacket1[3] = 'd';
    testPacket1[4] = '0';
    testPacket1[5] = 'k'; // complete packet start
    testPacket1[6] = 224;
    testPacket1[7] = '1';

    char testPacket2[6];
    testPacket2[0] = '1';
    testPacket2[1] = '1';
    testPacket2[2] = 0;
    testPacket2[3] = 'k'; // complete packet start
    testPacket2[4] = '0';
    testPacket2[5] = '1';

    char testPacket3[2];
    testPacket3[0] = '1';
    testPacket3[1] = '1';

    char testPacket4[6];
    testPacket4[0] = 0;
    testPacket4[1] = '_';
    testPacket4[2] = 'l';
    testPacket4[3] = 'r'; // return from a _lr
    testPacket4[4] = 0;
    testPacket4[5] = 'd';

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket1, sizeof(testPacket1));
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);

    EXPECT_FALSE(TestKeypressPackets::m_responseMgr.has_queued_responses());

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket2, sizeof(testPacket2));
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);

    cedrus::response res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 8);
    EXPECT_TRUE(res.was_pressed == false);

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket3, sizeof(testPacket3));
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);

    EXPECT_FALSE(TestKeypressPackets::m_responseMgr.has_queued_responses());

    TestKeypressPackets::m_dummyConnection->insert_more_data_into_buffer(testPacket4, sizeof(testPacket4));
    TestKeypressPackets::m_responseMgr.check_for_keypress(TestKeypressPackets::m_dummyConnection, m_dummyConfig);

    res = TestKeypressPackets::m_responseMgr.get_next_response();

    EXPECT_TRUE(res.port == 0);
    EXPECT_TRUE(res.key == 2);
    EXPECT_TRUE(res.was_pressed == true);

    Cedrus::UnSuppress_All_Assertions();
}
