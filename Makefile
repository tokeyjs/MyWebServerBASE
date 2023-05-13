mainfile=./test/firstTest.cpp ./ThreadPool/threadpool.cpp ./log/log.cpp
goalname=testHttpServer
$(goalname):
	g++ $(mainfile) -pthread -o $(goalname) 
