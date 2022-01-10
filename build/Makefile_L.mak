#delay discounting (ddGEM) executable standalone
#make			//equivalent to make all 
#make all		//todo make targets for individual exes	
#make clean
#on surface need to remove boost 141, just vc, and also define -DBOOST_ALL_NO_LIB
#./usr/lib/x86_64-linux-gnu/libmysqlcppconn.so.7
#./usr/lib/x86_64-linux-gnu/libmysqlcppconn.so.7.1.1.9
#./usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.2
#./usr/lib/x86_64-linux-gnu/libmysqlcppconn8.so.2.8.0.22
#./usr/lib/libcgicc.so
#/opt/bitnami/apache/cgi-bin
#/data32/workspace/cpp_libs/boost_1_75_0/stage/lib/libboost_program_options.so

#NEED NLopt lib and include
target:= ddgem
cgiTarget:= /opt/bitnami/apache/cgi-bin

mysql_lib_dir:= /usr/lib/x86_64-linux-gnu
mysql_h_dir:= /usr/include/mysql-cppconn-8/
boost_dir:= /data32/workspace/cpp_libs/boost_1_75_0
boost_po_dir:= $(boost_dir)/stage/lib
cgi_dir:= /usr/lib
cgi_h_dir:= /usr/include/cgicc
nlopt_dir:= /usr/local/lib
nlopt_h_dir:= /usr/local/include

include:= -I../include -I$(mysql_h_dir) -I$(boost_dir) -I$(cgi_h_dir) -I$(nlopt_h_dir)

CXX:= g++
CXXFLAGS:= -fPIC -std=c++1z -O3 -g0  $(include) \
-DBOOST_ALL_DYN_LINK -DAWS -DNDEBUG \
-L/$(boost_po_dir) -lboost_program_options \
-L/$(mysql_lib_dir) -lmysqlcppconn8 \
-L/$(cgi_dir) -lcgicc \
-L/$(nlopt_dir) -lnlopt

VPATH:= ../src ../src/exe ../src/tests ../include

$(target) : ddgem.o ddgemModelBoost.o ddgemModel.o tinyxml2.o TestXMLGenerator.o lsk.o mlk.o ddgemModelNLopt.o abcrit.o ddFuncs.o ddFuncsML.o

	$(CXX) $(CXXFLAGS) -o $@ $^
	cp ddgem $(cgiTarget)

ddgem.o : ddgem.cpp ddgem.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tinyxml2.o : tinyxml2.cpp tinyxml2.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ddgemModel.o : ddgemModel.cpp ddgemModel.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ddgemModelBoost.o : ddgemModelBoost.cpp ddgemModelBoost.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TestXMLGenerator.o : TestXMLGenerator.cpp TestXMLGenerator.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

lsk.o : lsk.cpp lsk.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mlk.o : mlk.cpp mlk.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ddgemModelNLopt.o : ddgemModelNLopt.cpp ddgemModelNLopt.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

abcrit.o : abcrit.cpp abcrit.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ddFuncs.o : ddFuncs.cpp ddFuncs.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ddFuncsML.o : ddFuncsML.cpp ddFuncsML.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<


