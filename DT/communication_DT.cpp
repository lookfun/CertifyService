#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "communication.h"

using namespace testing;

class VarifySocket_suite: public testing::Test
{

};

class Friend_VarifySocket : public VarifySocket
{
	
	Friend_VarifySocket(SOCKET sock):VarifySocket(sock){};
	FRIEND_TEST(VarifySocket_suite,RecvFrame_DT_1);
	MOCK_METHOD0(DealWithErr,void());
	MOCK_METHOD4(recv,int(SOCKET,char*,unsigned int ,unsigned int));
};

TEST_F(VarifySocket_suite,RecvFrame_DT_1)
{
	Friend_VarifySocket sock(1);
	Frame repa;

	ON_CALL(sock,DealWithErr()).WillByDefault(Return());
	EXPECT_CALL(sock,DealWithErr()).Times(1);
	ON_CALL(sock,recv(_,_,_,_)).WillByDefault(Return(0));
	EXPECT_CALL(sock,recv(_,_,_,_)).Times(1);
	sock.RecvFrame(&repa);
}