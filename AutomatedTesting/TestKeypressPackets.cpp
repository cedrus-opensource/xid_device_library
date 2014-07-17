
#include <gtest/gtest.h>
#include <string>



class TestKeypressPackets : public testing::Test
{
protected:
	virtual void SetUp()
	{
		//logNull = new wxLogNull;
	}

	virtual void TearDown()
	{
		//delete logNull;
	}

	//wxLogNull *logNull;

};



TEST_F( TestKeypressPackets, TestRejectInvalidFile_01 )
{
    int x = 0;
    int y = 1;

	EXPECT_FALSE( x == y );
}
