// =============================================================================
//
// Copyright (c) 2016 Brannon Dorsey <brannon@brannondorsey.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"

string reorganize(string message);

void ofApp::setup()
{
    //create the socket and bind to port 11999
	udpConnection.Create();
	udpConnection.Bind(11999);
	udpConnection.SetNonBlocking(true);
    
    std::string port = "";
    
//    if (getenv("ESCPOSPORT") != NULL) {
//        port = getenv("ESCPOSPORT");
//    } else {
//        port = "/dev/tty.Repleo-PL2303-00001014";
//    }
    
    // We can get all of the connected serial devices using the
    // IO::SerialDeviceUtils::getDevices() method.
    // See documentation for more information.
    ofx::IO::SerialDeviceInfo::DeviceList devices = ofx::IO::SerialDeviceUtils::listDevices();
    for (int i = 0; i < devices.size(); i++) {
        cout << devices[i].getPort() << endl;
        // OSX: /dev/cu.Repleo-
        if (ofIsStringInString(devices[i].getPort(), "/dev/ttyUSB0")) {
            port = devices[i].getPort();
            break;
        }
    }

    
    if (port == "") {
        ofLogError("ofApp::setup") << "No thermal printer detected, exiting.";
        ofExit(1);
    }

    if (!printer.setup(port))//,
//                       9600,
//                       IO::SerialDevice::DATA_BITS_EIGHT,
//                       IO::SerialDevice::PAR_NONE,
//                       IO::SerialDevice::STOP_ONE,
//                       IO::SerialDevice::FLOW_CTRL_HARDWARE,
//                       serial::Timeout::simpleTimeout(10)))
    {
        ofLogError("ofApp::setup") << "Unable to connect to: " << port;
        ofExit();
    }
    
    // Set up hardware flow control if needed.
    printer.setDataTerminalReady();
    printer.setRequestToSend();

    // Initialize the printer.
    printer.initialize();
    
    printer.setUpsideDown(true);
    
    times.push_back("12:21 AM");
    messages.push_back("NEW JOB: BED: PEDS Room 0295 Bed 01 BED ID: 29501 STATUS: Dirty");
    
    times.push_back("12:22 AM");
    messages.push_back("CHANDRA EVELYN/INGALLS pt:FALYN TAMAYO 9/7/84 708-915-5290 DR TO DR KHALID");
    
    times.push_back("12:23 AM");
    messages.push_back("RQSTD: MCDONALD, EMERICK 59 Medical #: 000145104 Visit #: 217023753 M Origin Unit: ED Admitting: VYAS-MD, AMIT D TRI Level of Care: Admit to Inpatient Services Diagnosis: AMS, prob alcohol withdrawl, poss sepsis, head contusion");
    
    times.push_back("12:24 AM");
    messages.push_back("773-296-0071 FROM: KATE-RN AIMMC 531/541 - ONCOL 531- PLEASE D/C CONTINUOUS PULSE OX ORDER, CANNOT HAVE ON THIS FLOOR");
    
    times.push_back("12:25 AM");
    messages.push_back("From:FaxAndPage@extendedcare.com<LF>Msg:Allscripts Alert: Referral Received response from Providence Heal<LF>thcare and Rehabilitation Center - Downers Grove re: Referral 324<LF>38948 for patient in PC424210-01: Referral Received");
    
    times.push_back("12:26 AM");
    messages.push_back("CODE YELLOW TRAUMA TEAM TO THE EMERGENCY ROOM");
    
    times.push_back("12:27 AM");
    messages.push_back("CODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOMCODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOMCODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOMCODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOMCODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOMCODEYELLOWTRAUMATEAMTOTHEEMERGENCYROOM");

    messageIndex = 0;
}

void ofApp::update() {
    
    char udpBuffer[10000];
	udpConnection.Receive(udpBuffer, 10000);
    string message = udpBuffer;
    if (message.length() > 0){
        if (message == "[printer invert true]") {
            printer.setInvert(true);
        } else if (message == "[printer invert false]") {
            printer.setInvert(false);
        } else {
            printer.println(reorganize(message));
        }
    }
}

// When printing upside down with the ESCPOS printer multi-line mesages are
// printed in reverse order. This function returns a formatted message that
// fixes this and adds non-justified line breaks on the right side of the text.
string reorganize(string message) {
    
    int numCharsPerLine = 47;
    
    if (message.size() < numCharsPerLine) {
        return message;
    }
    
    vector<string> lines;
    
    while (message.size() >= numCharsPerLine) {
        string fragment = message.substr(0, numCharsPerLine);
//        cout << "FRAGMENT: " << fragment << endl;
        int lastSpace = fragment.find_last_of(' ');
        if (lastSpace != string::npos) {
            lines.push_back(message.substr(0, lastSpace));
//            cout << "Found space, adding following to lines\n\t" <<
//                lines[lines.size() - 1] << endl;
            if (lastSpace + 1 < message.size())
                message = message.substr(lastSpace + 1);
//            cout << "Message is now\n\t" << message << endl;
        } else {
            lines.push_back(message.substr(0, numCharsPerLine));
            if (numCharsPerLine < message.size())
                message = message.substr(numCharsPerLine);
//            cout << "DID NOT find space, adding the following to line\n\t" <<
//            lines[lines.size() - 1] << endl;
//            cout << "Message is now\n\t" << message << endl;
        }
    }
    
    lines.push_back(message);
    reverse(lines.begin(), lines.end());
    
    std::stringstream ss;
    for(size_t i = 0; i < lines.size(); ++i)
    {
        if(i != 0)
            ss << "\n";
        ss << lines[i];
    }
    
    return ss.str();
}
