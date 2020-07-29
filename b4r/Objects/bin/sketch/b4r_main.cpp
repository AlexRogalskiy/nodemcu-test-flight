#include "B4RDefines.h"

B4R::Serial* b4r_main::_serial1;
static B4R::Serial be_gann1_3;


 void b4r_main::_appstart(){
const UInt cp = B4R::StackMemory::cp;
 //BA.debugLineNum = 14;BA.debugLine="Private Sub AppStart";
 //BA.debugLineNum = 15;BA.debugLine="Serial1.Initialize(74880)";
b4r_main::_serial1->Initialize((ULong) (74880));
 //BA.debugLineNum = 16;BA.debugLine="Log(\"AppStart\")";
B4R::Common::LogHelper(1,102,F("AppStart"));
 //BA.debugLineNum = 17;BA.debugLine="End Sub";
B4R::StackMemory::cp = cp;
}

void b4r_main::initializeProcessGlobals() {
     B4R::StackMemory::buffer = (byte*)malloc(STACK_BUFFER_SIZE);
     b4r_main::_process_globals();

   
}
void b4r_main::_process_globals(){
const UInt cp = B4R::StackMemory::cp;
 //BA.debugLineNum = 8;BA.debugLine="Sub Process_Globals";
 //BA.debugLineNum = 11;BA.debugLine="Public Serial1 As Serial";
b4r_main::_serial1 = &be_gann1_3;
 //BA.debugLineNum = 12;BA.debugLine="End Sub";
}
