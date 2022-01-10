#started from scratch.mak, build ddParamRec project
#options
#debug=t		//default release
#make			//equivalent to make all 
#on surface need to remove boost 142, just vc, and also define -DBOOST_ALL_NO_LIB

target:= ddParamRec.exe
cpp_libs:= ../../../../documents/cpp_libs/
boost:= $(cpp_libs)boost_1_77_0/
boostlib:= $(cpp_libs)boost_1_77_0/stage/lib/
gutils:=../../gutils/
mutils:=../../mutils/
iutils:=../../iutils/
nlopt:= $(cpp_libs)nlopt/build/
nloptsrc:= ../../../../documents/cpp_libs/nlopt/src/
mysql:= $(cpp_libs)mysqlConnector/
include:= -I../include -I$(boost) -I$(nlopt) -I$(nloptsrc)algs -I$(nloptsrc)api -I$(nloptsrc)util -I$(gutils)include -I$(mutils)include -I$(iutils)/include -I$(mysql)debug/include/

CXX:= cl 

#define USINGUTILS need to prevent inclusion of getEnumValue in types.h
CXXFLAGS:= -EHsc -std:c++17 -DUSINGUTILS -DBOOST_ALL_DYN_LINK $(include) 
VPATH:= ../src ../src/exe ../src/tests ../include

ifdef debug
CXXFLAGS += -Zi -MDd -DDEBUG 
oud:= ../bin/debug/

LIBS:=msvcprtd.lib $(addsuffix boost_program_options-vc142-mt-gd-x64-1_77.lib,$(boostlib)) $(addsuffix bin/Debug/utils_d.lib, $(gutils)) $(addsuffix bin/Debug/mutils_d.lib, $(mutils)) $(addsuffix bin/Debug/iutils_d.lib, $(iutils)) $(addprefix $(mysql), debug/lib64/vs14/libcrypto.lib debug/lib64/vs14/libssl.lib) $(addprefix $(mysql), debug/lib64/vs14/debug/mysqlcppconn8-2-vs14_d.lib) $(addprefix $(nlopt), Debug/nlopt_d.lib)
LDFLAGS:= -DEBUG $(LIBS)
else
    CXXFLAGS += -O2 -Ob2 -MD -DNDEBUG -I$(mysql)release/include/ 
oud:= ../bin/release/

LIBS:=msvcprt.lib $(addsuffix boost_program_options-vc142-mt-x64-1_77.lib,$(boostlib)) $(addsuffix bin/Release/utils.lib, $(gutils)) $(addsuffix bin/Release/mutils.lib, $(mutils)) $(addsuffix bin/Release/iutils.lib, $(iutils)) $(addprefix $(mysql), release/lib64/vs14/libcrypto.lib release/lib64/vs14/libssl.lib) $(addprefix $(mysql), release/lib64/vs14/mysqlcppconn8-2-vs14_r.lib) $(addprefix $(nlopt), Release/nlopt.lib)
LDFLAGS:= $(LIBS)

endif
target:= $(addsuffix $(target),$(oud))

$(target) : ddParamRec.obj ddFuncs.obj
	link $^ $(LDFLAGS) /OUT:$(target) 

ddParamRec.obj : ddParamRec.cpp ddParamRec.h
	$(CXX) $(CXXFLAGS) /c $<

ddFuncs.obj : ddFuncs.cpp ddFuncs.h
	$(CXX) $(CXXFLAGS) /c $<

.PHONY: clean
clean:
	rm *.obj *.pdb *.dll *.exp *.lib *.exe
