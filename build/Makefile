#delay discounting (ddGEM) executable standalone
#options
#debug=t		//defaults to release
#noinstall=t		//defaults to install ../bin
#targets 
#make			//equivalent to make all 
#make all		//todo make targets for individual exes	
#make clean
#on surface need to remove boost 141, just vc, and also define -DBOOST_ALL_NO_LIB

target:= ddgem.exe
cgiTarget:= c:/Apache24/cgi-bin/ddgem
cpp_libs:= ../../../../documents/cpp_libs/
boost:= $(cpp_libs)boost_1_66_0/stage/lib/
mysql:= $(cpp_libs)mysqlConnector/
cgi:= $(cpp_libs)cgi-cc3.2.19/
nlopt:= $(cpp_libs)nlopt/build/
nloptsrc:= ../../../../documents/cpp_libs/nlopt/src/
include:= -I../include -I$(cpp_libs)boost_1_66_0 -I$(cgi) -I$(nlopt) -I$(nloptsrc)algs -I$(nloptsrc)api -I$(nloptsrc)util
CXX:= cl 
versionFile:= ddgemVersion.txt
VERSION:= $(shell perl ../../../perl/uscripts/versionBump/bump.pl -inc -dry -f $(versionFile))
CXXFLAGS:= -EHsc -std:c++17 -DBOOST_ALL_DYN_LINK $(include) -D$(addprefix VER=,\"$(VERSION)\")
VPATH:= ../src ../src/exe ../src/tests ../include


ifdef debug		#make debug=t
    CXXFLAGS += -Zi -MDd -DDEBUG -I$(mysql)debug/include/
oud:= ../bin/debug/
LIBS:= $(addsuffix boost_program_options-vc141-mt-gd-x64-1_66.lib,$(boost)) $(addprefix $(mysql), debug/lib64/vs14/libcrypto.lib debug/lib64/vs14/libssl.lib) $(addprefix $(mysql), debug/lib64/vs14/debug/mysqlcppconn8-2-vs14_d.lib) $(addprefix $(cgi), win/Debug/cgicc_d.lib) $(addprefix $(nlopt), Debug/nlopt_d.lib)
LDFLAGS:= -DEBUG $(LIBS)
else
    CXXFLAGS += -O2 -Ob2 -MD -DNDEBUG -I$(mysql)release/include/ 
oud:= ../bin/release/
LIBS:= $(addsuffix boost_program_options-vc141-mt-x64-1_66.lib,$(boost)) $(addprefix $(mysql), release/lib64/vs14/libcrypto.lib release/lib64/vs14/libssl.lib release/lib64/vs14/mysqlcppconn8-2-vs14_r.lib) $(addprefix $(cgi), win/Release/cgicc.lib) $(addprefix $(nlopt), Release/nlopt.lib)
LDFLAGS:= $(LIBS)
endif
ifdef noinstall		#make noinstall=t
oud:= ./
endif

target:= $(addsuffix $(target),$(oud))

$(target) : ddgem.obj ddgemModelBoost.obj ddgemModel.obj tinyxml2.obj TestXMLGenerator.obj lsk.obj mlk.obj ddgemModelNLopt.obj abcrit.obj ddFuncs.obj ddFuncsML.obj
	link $^ $(LDFLAGS) /OUT:$(target) 
	perl ../../../perl/uscripts/versionBump/bump.pl -inc -f $(versionFile)
	copy "$(subst /,\,$(target))" "$(subst /,\,$(cgiTarget))" /Y

ddgem.obj : ddgem.cpp ddgem.h
	$(CXX) $(CXXFLAGS) /c $<

tinyxml2.obj : tinyxml2.cpp tinyxml2.h
	$(CXX) $(CXXFLAGS) /c $<

ddgemModel.obj : ddgemModel.cpp ddgemModel.h
	$(CXX) $(CXXFLAGS) /c $<

ddgemModelBoost.obj : ddgemModelBoost.cpp ddgemModelBoost.h
	$(CXX) $(CXXFLAGS) /c $<

TestXMLGenerator.obj : TestXMLGenerator.cpp TestXMLGenerator.h
	$(CXX) $(CXXFLAGS) /c $<

lsk.obj : lsk.cpp lsk.h
	$(CXX) $(CXXFLAGS) /c $<

mlk.obj : mlk.cpp mlk.h
	$(CXX) $(CXXFLAGS) /c $<

ddgemModelNLopt.obj : ddgemModelNLopt.cpp ddgemModelNLopt.h
	$(CXX) $(CXXFLAGS) /c $<

abcrit.obj : abcrit.cpp abcrit.h
	$(CXX) $(CXXFLAGS) /c $<

ddFuncs.obj : ddFuncs.cpp ddFuncs.h
	$(CXX) $(CXXFLAGS) /c $<

ddFuncsML.obj : ddFuncsML.cpp ddFuncsML.h
	$(CXX) $(CXXFLAGS) /c $<

.PHONY: clean
clean:
	rm *.obj *.pdb *.dll *.exp *.lib *.exe

.PHONY: doc
doc:
	perl ../../../perl/uscripts/doxyFix.pl -pname "ddgem EP PRER" -pbrief "delay discounting project"
	Doxygen mydoxy
