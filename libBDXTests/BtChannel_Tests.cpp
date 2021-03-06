//#include "stdafx.h"
#include <thread>
#include <vector>
#include <memory>

#include "Common/Defines.h"
#include "Network/BtIOService.h"

#include "Network/BtEndpoint.h"
#include "Network/Channel.h"

#include "Common/ByteStream.h"
#include "Common/Logger.h"


#include "BtChannel_Tests.h"

#include "Common.h"


using namespace libBDX;

void BtNetwork_Connect1_Boost_Test()
{

	Lg::setThreadName("Test_Host");

	std::string channelName{ "TestChannel" };
	std::string msg{ "This is the message" };
	ByteStream msgBuff((u8*)msg.data(), msg.size());

	BtIOService ioService(0);
	auto thrd = std::thread([&]()
	{
		Lg::setThreadName("Test_Client");

		//Lg::out << "client ep start" << Lg::endl;
		BtEndpoint endpoint(ioService, "127.0.0.1", 1212, false, "endpoint");
		//Lg::out << "client ep done" << Lg::endl;

		Channel& chl = endpoint.addChannel(channelName, channelName);
		//Lg::out << "client chl done" << Lg::endl;


		std::unique_ptr<ByteStream> srvRecv(new ByteStream());

		chl.recv(*srvRecv);


		if (*srvRecv != msgBuff)
		{

			throw UnitTestFail();
		}


		chl.asyncSend(std::move(srvRecv));

		//Lg::out << " server closing" << Lg::endl;

		chl.close();

		//Lg::out << " server nm closing" << Lg::endl;

		//netServer.CloseChannel(chl.Name());
		endpoint.stop();
		//Lg::out << " server closed" << Lg::endl;

	});

	//BtIOService ioService;

	//Lg::out << "host ep start" << Lg::endl;

	BtEndpoint endpoint(ioService, "127.0.0.1", 1212, true, "endpoint");

	//Lg::out << "host ep done" << Lg::endl;

	auto& chl = endpoint.addChannel(channelName, channelName);
	//Lg::out << "host chl done" << Lg::endl;

	//Lg::out << " client channel added" << Lg::endl;

	chl.asyncSend(msgBuff.data(), msgBuff.size());

	ByteStream clientRecv;
	chl.recv(clientRecv);

	if (clientRecv != msgBuff)
		throw UnitTestFail();
	//Lg::out << " client closing" << Lg::endl;


	chl.close();
	//netClient.CloseChannel(channelName);
	endpoint.stop();
	//Lg::out << " client closed" << Lg::endl;


	thrd.join();

	ioService.stop();

}


void BtNetwork_OneMegabyteSend_Boost_Test()
{
	//InitDebugPrinting();

	Lg::setThreadName("Test_Host");

	std::string channelName{ "TestChannel" };
	std::string msg{ "This is the message" };
	ByteStream oneMegabyte((u8*)msg.data(), msg.size());
	oneMegabyte.reserve(1000000);
	oneMegabyte.setp(1000000);

	memset(oneMegabyte.data() + 100, 0xcc, 1000000 - 100);

	BtIOService ioService(0);

	auto thrd = std::thread([&]()
	{
		Lg::setThreadName("Test_Client");

		BtEndpoint endpoint(ioService, "127.0.0.1", 1212, false, "endpoint");
		Channel& chl = endpoint.addChannel(channelName, channelName);

		std::unique_ptr<ByteStream> srvRecv(new ByteStream());
		chl.recv(*srvRecv);

		if ((*srvRecv) != oneMegabyte)
			throw UnitTestFail();

		chl.asyncSend(std::move(srvRecv));

		chl.close();

		endpoint.stop();

	});


	BtEndpoint endpoint(ioService, "127.0.0.1", 1212, true, "endpoint");
	auto& chl = endpoint.addChannel(channelName, channelName);

	chl.asyncSend(oneMegabyte.data(), oneMegabyte.size());

	ByteStream clientRecv;
	chl.recv(clientRecv);

	if (clientRecv != oneMegabyte)
		throw UnitTestFail();

	chl.close();
	endpoint.stop();
	thrd.join();

	ioService.stop();
}


void BtNetwork_ConnectMany_Boost_Test()
{
	//InitDebugPrinting();
	Lg::setThreadName("Test_Host");

	std::string channelName{ "TestChannel" };

	u64 numChannels(100);
	u64 messageCount(100);

	bool print(false);

	ByteStream buff(64);
	buff.setp(64);

	buff.data()[14] = 3;
	buff.data()[24] = 6;
	buff.data()[34] = 8;
	buff.data()[44] = 2;

	std::thread serverThrd = std::thread([&]()
	{
		BtIOService ioService(1);
		Lg::setThreadName("Test_client");

		BtEndpoint endpoint(ioService, "127.0.0.1", 1212, false, "endpoint");

		std::vector<std::thread> threads;

		for (u64 i = 0; i < numChannels; i++)
		{
			threads.emplace_back([i, &buff, &endpoint, messageCount, print, channelName]()
			{
				Lg::setThreadName("Test_client_" + std::to_string(i));
				auto& chl = endpoint.addChannel(channelName + std::to_string(i), channelName + std::to_string(i));
				ByteStream mH;

				for (u64 j = 0; j < messageCount; j++)
				{
					chl.recv(mH);
					if (buff != mH)throw UnitTestFail();
					chl.asyncSend(buff.data(), buff.size());
				}

				chl.close();

				//std::stringstream ss;
				//ss << "server" << i << " done\n";
				//Lg::out << ss.str();
			});
		}


		for (auto& thread : threads)
			thread.join();


		endpoint.stop();
		ioService.stop();
		//Lg::out << "server done" << Lg::endl;
	});

	BtIOService ioService(1);

	BtEndpoint endpoint(ioService, "127.0.0.1", 1212, true, "endpoint");

	std::vector<std::thread> threads;

	for (u64 i = 0; i < numChannels; i++)
	{
		threads.emplace_back([i, &endpoint, &buff, messageCount, print, channelName]()
		{
			Lg::setThreadName("Test_Host_" + std::to_string(i));
			auto& chl = endpoint.addChannel(channelName + std::to_string(i), channelName + std::to_string(i));
			ByteStream mH(buff);

			for (u64 j = 0; j < messageCount; j++)
			{
				chl.asyncSendCopy(mH);
				chl.recv(mH);

				if (buff != mH)throw UnitTestFail();
			}


			chl.close();
		});
	}



	for (auto& thread : threads)
		thread.join();

	endpoint.stop();

	serverThrd.join();

	ioService.stop();

}


void BtNetwork_CrossConnect_Test()
{
	const block send = _mm_set_epi64x(123412156, 123546);
	const block recv = _mm_set_epi64x(7654333, 8765433);

	auto thrd = std::thread([&]() {
		BtIOService ioService(0);
		//Lg::setThreadName("Net_Cross1_Thread");
		BtEndpoint endpoint(ioService, "127.0.0.1", 1212, false, "endpoint");


		auto& sendChl1 = endpoint.addChannel("send", "recv");
		auto& recvChl1 = endpoint.addChannel("recv", "send");

		ByteStream buff;
		buff.append(send);

		sendChl1.asyncSendCopy(buff);
		block temp;

		recvChl1.recv(buff);
		buff.consume((u8*)&temp, 16);

		if (neq(temp, send))
			throw UnitTestFail();

		buff.setp(0);
		buff.append(recv);
		recvChl1.asyncSendCopy(buff);

		sendChl1.recv(buff);

		buff.consume((u8*)&temp, 16);

		if (neq(temp, recv))
			throw UnitTestFail();

		recvChl1.close();
		sendChl1.close();

		endpoint.stop();

		ioService.stop();
	});
	BtIOService ioService(0);
	BtEndpoint endpoint(ioService, "127.0.0.1", 1212, true, "endpoint");


	auto& recvChl0 = endpoint.addChannel("recv", "send");
	auto& sendChl0 = endpoint.addChannel("send", "recv");

	ByteStream buff;
	buff.append(send);

	sendChl0.asyncSendCopy(buff);
	block temp;

	recvChl0.recv(buff);
	buff.consume((u8*)&temp, 16);

	if (neq(temp, send))
		throw UnitTestFail();

	buff.setp(0);
	buff.append(recv);
	recvChl0.asyncSendCopy(buff);

	sendChl0.recv(buff);

	buff.consume((u8*)&temp, 16);

	if (neq(temp, recv))
		throw UnitTestFail();

	sendChl0.close();
	recvChl0.close();

	thrd.join();
	endpoint.stop();
	ioService.stop();

}


void BtNetwork_ManyEndpoints_Test()
{
	u64 nodeCount = 20;
	u32 basePort = 1712;
	std::string ip("127.0.0.1");
	//InitDebugPrinting();

	std::vector<std::thread> nodeThreads(nodeCount);

	Lg::setThreadName("main");

	for (u64 i = 0; i < nodeCount; ++i)
	{
		nodeThreads[i] = std::thread([&, i]() {

			Lg::setThreadName("node" + std::to_string(i));


			u32 port;// = basePort + i;
			BtIOService ioService(0);
			std::list<BtEndpoint> endpoints;
			std::vector<Channel*> channels;

			for (u64 j = 0; j < nodeCount; ++j)
			{
				if (j != i)
				{
					bool host = i > j;
					std::string name("endpoint:");
					if (host)
					{
						name += std::to_string(i) + "->" + std::to_string(j);
						port = basePort + (u32)i;
					}
					else
					{
						name += std::to_string(j) + "->" + std::to_string(i);
						port = basePort + (u32)j;
					}

					endpoints.emplace_back(ioService, ip, port, host, name);

					channels.push_back(&endpoints.back().addChannel("chl", "chl"));
				}
			}
			for (u64 j = 0, idx = 0; idx < nodeCount; ++j, ++idx)
			{
				if (j == i)
				{
					++idx;
					if (idx == nodeCount)
						break;
				}

				std::string msg = "hello" + std::to_string(idx);
				channels[j]->asyncSend(std::move(std::unique_ptr<ByteStream>(new ByteStream((u8*)msg.data(), msg.size()))));
			}

			std::string expected = "hello" + std::to_string(i);

			for (auto chl : channels)
			{
				ByteStream recv;
				chl->recv(recv);
				std::string msg((char*)recv.data(), recv.size());


				if (msg != expected)
					throw UnitTestFail();
			}
			//Lg::out << Lg::lock << "re " << i << Lg::endl << Lg::unlock;

			for (auto chl : channels)
				chl->close();

			for (auto& endpoint : endpoints)
				endpoint.stop();


			ioService.stop();
		});
	}

	for (u64 i = 0; i < nodeCount; ++i)
		nodeThreads[i].join();
}
