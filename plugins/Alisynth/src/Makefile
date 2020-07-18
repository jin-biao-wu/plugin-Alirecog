
ifeq ($(shell uname -s),Linux)
    GPP=g++
endif

GPP_PRAM= -std=c++11 -fPIC -g -c -Wall -Wno-unknown-pragmas -march=x86-64 -gdwarf-2 -gstrict-dwarf
GPP_FLAG=$(GPP_PRAM) -DUNIX -DLINUX -DUNIX_SERVICE -D_REENTRANT -D_GNU_SOURCE -DFS_DEBUG
#-Wno-unknown-pragmas -march=x86-64 -gdwarf-2 -gstrict-dwarf
#-DUNIX -DLINUX -DUNIX_SERVICE -D_REENTRANT -D_GNU_SOURCE

#obj include
OBJ_INC=../include

#mrcp
MRCP_INC=../../../libs/mrcp-engine/include \
         ../../../libs/mrcp/include \
         ../../../libs/mrcp/resources/include \
         ../../../libs/mrcp/control/include \
         ../../../libs/mrcp/message/include \
         ../../../libs/apr-toolkit/include \
         ../../../libs/mpf/include

MRCP_LIB=/usr/local/unimrcp/lib
         #../../../bin/plugin

#apr          
APR_INC=../../../libs/apr/include
#/usr/local/apr/include/apr-1

#apr-util
APR_UTIL_INC=../../../libs/apr-util/include\
#/usr/local/apr-util/include/apr-1

#apr-toolkit/include
APR_TOOLKIT_INC=../../../libs/apr-toolkit/include

#ali NlsSdkCpp2 sdk
ALI_NLSSDK_INC= ../aliSdk2.0/NlsSdkCpp2.0/include
#ALI_NLSFILE_INC=../aliSdk2.0/NlsSdkCpp2.0/include/nlsCommonSdk

#json
JSON_INC=../aliSdk2.0/NlsSdkCpp2.0/include

#LIBS= -L$(MRCP_LIB) -L$(APR_LIB) -L$(APR_UTIL_LIB) -L$(ALI_SDK_LIB) -L$(EVENT_LIB)
#-L$(THIRD_SQLITE)

LIBS_LINK=	\
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -lcrypto \
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -lcurl \
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -ljsoncpp \
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -luuid\
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -lopus \
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -lnlsCppSdk \
			-L../aliSdk2.0/NlsSdkCpp2.0/lib/linux -lnlsCommonSdk \
			-L/usr/local/apr/lib -lapr-1 \
			-L/usr/local/apr/lib -laprutil-1 \
			-L/usr/local/unimrcp/lib -lunimrcpserver 

            
INCLUDE_FLAG=$(addprefix -I,${MRCP_INC} ${APR_INC} ${APR_UTIL_INC} ${ALI_NLSSDK_INC} ${OBJ_INC})
INCLUDE_ALL=$(MRCP_INC) $(APR_INC) $(APR_UTIL_INC) $(ALI_NLSSDK_INC) $(OBJ_INC)

ASR_OBJ=Ali_Synth_engine.o \
		Ali_SynthPool.o \
		Ali_SynthUtil.o \
		Ali_SynthChannelFactory.o \
		Ali_SynthChannel.o\
		Ali_SynthEngine.o
        
COMPLIE_CMD=$(GPP) $(GPP_FLAG) $(INCLUDE_FLAG)  $(LIBS_LINK)
#$(LIBS)

Alisynth.so:$(ASR_OBJ)
	$(GPP) $(ASR_OBJ) $(LIBS_LINK) -shared -o Alisynth.so
	rm -rf *.o


# obj======================================    

Ali_Synth_engine.o:Ali_Synth_engine.cpp 
	$(COMPLIE_CMD) Ali_Synth_engine.cpp

Ali_SynthPool.o:Ali_SynthPool.cpp 
	$(COMPLIE_CMD) Ali_SynthPool.cpp

Ali_SynthUtil.o:Ali_SynthUtil.cpp 
	$(COMPLIE_CMD) Ali_SynthUtil.cpp

Ali_SynthChannelFactory.o:Ali_SynthChannelFactory.cpp 
	$(COMPLIE_CMD) Ali_SynthChannelFactory.cpp

Ali_SynthChannel.o:Ali_SynthChannel.cpp 
	$(COMPLIE_CMD) Ali_SynthChannel.cpp

Ali_SynthEngine.o:Ali_SynthEngine.cpp 
	$(COMPLIE_CMD) Ali_SynthEngine.cpp
   
clean:
	rm -rf *.o Alisynth.so
