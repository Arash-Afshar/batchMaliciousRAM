#pragma once
#include "Common/Defines.h"


#include <deque>
#include <mutex>
#include <future> 


#include "boost/asio.hpp"
#include "Network/BtIOService.h"

namespace libBDX { 

	class WinNetIOService;
	class ChannelBuffer;


	struct BoostIOOperation
	{
		enum class Type
		{
			RecvName,
			RecvData,
			CloseRecv,
			SendData,
			CloseSend,
			CloseThread
		};

		BoostIOOperation()
		{
			clear();
		}

		BoostIOOperation(const BoostIOOperation& copy)
		{
			mType = copy.mType;
			mSize = copy.mSize;
			mBuffs[0] = boost::asio::buffer(&mSize, sizeof(u32));
			mBuffs[1] = copy.mBuffs[1];
			mOther = copy.mOther;
			mPromise = copy.mPromise;
		}

		void clear()
		{
			mType = (Type)0;
			mSize = 0; 
			mBuffs[0] = boost::asio::buffer(&mSize, sizeof(u32));
			mBuffs[1] = boost::asio::mutable_buffer();
			mOther = nullptr;
			mPromise = nullptr;
		} 


		std::array<boost::asio::mutable_buffer,2> mBuffs;
		Type mType;
		u32 mSize;

		void* mOther;
		std::promise<void>* mPromise;
		std::exception_ptr mException;
		//std::function<void()> mCallback;
	};



	class BtSocket
	{
	public:
		BtSocket(BtIOService& ios);

		boost::asio::ip::tcp::socket mHandle;
		boost::asio::strand mSendStrand, mRecvStrand;

		std::deque<BoostIOOperation> mSendQueue, mRecvQueue;
		bool mStopped;

		std::atomic<u64> mOutstandingSendData, mMaxOutstandingSendData, mTotalSentData;
	};

	

}
