#include<iostream>
#include<sstream>
#include<string>
#include<chrono>

#include <boost/date_time/posix_time/posix_time.hpp>
#include<leveldb/db.h>
#include"sqlite3.h"
#include"CppSQLite3.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
using namespace std;

int main()
{
	boost::posix_time::ptime timeTemp = boost::posix_time::second_clock::local_time();                    // 获取当前时间，秒级别;
	stringstream stream;
	stream << timeTemp.time_of_day();
	string res;
	stream >> res;
	
	std::cout << res << std::endl;                // 当前时间;
	std::cout << timeTemp.date() << std::endl;
	//open database
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status s = leveldb::DB::Open(options, "test.db", &db);
	if (!s.ok()) cerr << s.ToString() << endl;
	auto begin3 = chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; i++)
	{	
		string test = "zzd"+to_string(i);
		s = db->Put(leveldb::WriteOptions(), test, "zzdtest"+ test);
	}
	
	auto end3 = chrono::high_resolution_clock::now();
	auto ms3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - begin3).count();
	cout << "libevent: " << ms3 << endl;
	if (!s.ok()) cerr << s.ToString() << endl;
	string res2;
	db->Get(leveldb::ReadOptions(), "zzd",&res2);
	cout << res2 << endl;
	
	//close database
	delete db;


	//sqlite
	CppSQLite3DB sqlitedb;
	sqlitedb.open("testsqldb.db");
	auto begin = chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; i++)
	{
		string test = "zzd" + to_string(i);
		string sql = "insert into Users(username, password) VALUES(\"" 
			+ test + "\", \"zhangzidi\")";
		sqlitedb.execQuery(sql.c_str());
	}
	auto end = chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	sqlitedb.close();
	cout << "Sqlite: " << ms << endl;

	auto sys_logger = spdlog::rotating_logger_mt<spdlog::synchronous_factory>
		("sys_logger", "./API/logs" + to_string(time(0)) + "/sys.txt", 20481 * 1024, 850);
	auto begin2 = chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; i++)
	{
		string test = "zzd" + to_string(i);
		sys_logger->info(test);
	}
	auto end2 = chrono::high_resolution_clock::now();
	auto ms2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - begin2).count();
	cout << "File system: " << ms2 << endl;
	//xml tree
	using boost::property_tree::ptree;
	ptree pt;
	read_xml("config", pt);
	std::string listening_port 
		= pt.get<std::string>("server.listening-port");
	cout << "listening port: " << listening_port << endl;
	return 0;
}