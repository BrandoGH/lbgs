#ifndef __BASE_DEF_H__
#define __BASE_DEF_H__

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

#define READ_XML boost::property_tree::xml_parser::read_xml
#define CAST_TO(type,value) boost::lexical_cast<type>((value))
#define CPU_MAX_THREAD boost::thread::hardware_concurrency()
#define CPU_MAX_CORE boost::thread::physical_concurrency()
#define BIND boost::bind
#define MOVE boost::move
#define MSG_BUFFER boost::asio::buffer
#define THREAD_SLEEP(msec) boost::this_thread::sleep_for(boost::chrono::milliseconds(msec))
#define GEN_UUID std::string(boost::uuids::to_string(boost::uuids::random_generator()()))


namespace CommonBoost
{
// asio
typedef boost::asio::io_service			IOServer;
typedef boost::asio::deadline_timer		DeadlineTimer;
typedef boost::asio::ip::tcp			TCP;
typedef boost::asio::ip::tcp::acceptor	Acceptor;
typedef boost::asio::ip::tcp::socket	Socket;
typedef boost::asio::ip::tcp::endpoint	Endpoint;
typedef boost::asio::ip::tcp::no_delay	NoDelay;

// thread
typedef boost::thread						Thread;
typedef boost::asio::thread_pool			ThreadPool;
typedef boost::mutex						Mutex;
typedef boost::unique_lock<boost::mutex>	UniqueLock;
typedef boost::system::error_code			ErrorCode;

// pointer
typedef boost::shared_ptr<Socket>			SocketPtr;
typedef boost::shared_ptr<IOServer::work>	WorkPtr;

// config
typedef boost::property_tree::ptree			PTree;


}




#endif
