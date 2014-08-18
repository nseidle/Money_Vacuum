/*
Code originally from Aron Steg: http://forums.electricimp.com/discussion/comment/7904
Modified February 1st, 2014 by Nathan Seidle
Many great fixes were made by Aaron Steg, May 2014.

Currently, the only difference between this code and Aaron's original is we invert
the reset line logic to work with standard Arduinos.

Original license:

Copyright (c) 2014 Electric Imp
The MIT License (MIT)
http://opensource.org/licenses/MIT
*/

server.log("Agent started, URL is " + http.agenturl());

//Keys to the channel on data.sparkfun.com
local sparkfun_public_key = "dZNaDj6ZNgtZEm11vg19";
local sparkfun_private_key = "private key";

local override_reported = false;

local dollarsRaised = 0;
local inputTriggers = 0;
local outputTriggers = 0;

local local_hour_offset = 7;

const MAX_PROGRAM_SIZE = 0x20000;
const ARDUINO_BLOB_SIZE = 128;
program <- null;

//------------------------------------------------------------------------------------------------------------------------------
html <- @"<HTML>
<BODY>

<form method='POST' enctype='multipart/form-data'>
Program the ATmega328 via the Imp.<br/><br/>
Step 1: Select an Intel HEX file to upload: <input type=file name=hexfile><br/>
Step 2: <input type=submit value=Press> to upload the file.<br/>
Step 3: Check out your Arduino<br/>
</form>

</BODY>
</HTML>
";

//------------------------------------------------------------------------------------------------------------------------------
// Parses a HTTP POST in multipart/form-data format
function parse_hexpost(req, res) {
    local boundary = req.headers["content-type"].slice(30);
    local bindex = req.body.find(boundary);
    local hstart = bindex + boundary.len();
    local bstart = req.body.find("\r\n\r\n", hstart) + 4;
    local fstart = req.body.find("\r\n\r\n--" + boundary + "--", bstart);
    return req.body.slice(bstart, fstart);
}


//------------------------------------------------------------------------------------------------------------------------------
// Parses a hex string and turns it into an integer
function hextoint(str) {
    local hex = 0x0000;
    foreach (ch in str) {
        local nibble;
        if (ch >= '0' && ch <= '9') {
            nibble = (ch - '0');
        } else {
            nibble = (ch - 'A' + 10);
        }
        hex = (hex << 4) + nibble;
    }
    return hex;
}


//------------------------------------------------------------------------------------------------------------------------------
// Breaks the program into chunks and sends it to the device
function send_program() {
    if (program != null && program.len() > 0) {
        local addr = 0;
        local pline = {};
        local max_addr = program.len();
        
        device.send("burn", {first=true});
        while (addr < max_addr) {
            program.seek(addr);
            pline.data <- program.readblob(ARDUINO_BLOB_SIZE);
            pline.addr <- addr / 2; // Address space is 16-bit
            device.send("burn", pline)
            addr += pline.data.len();
        }
        device.send("burn", {last=true});
    }
}        

//------------------------------------------------------------------------------------------------------------------------------
// Parse the hex into an array of blobs
function parse_hexfile(hex) {
    
    try {
        // Look at this doc to work out what we need and don't. Max is about 122kb.
        // https://bluegiga.zendesk.com/entries/42713448--REFERENCE-Updating-BLE11x-firmware-using-UART-DFU
        server.log("Parsing hex file");
        
        // Create and blank the program blob
        program = blob(0x20000); // 128k maximum
        for (local i = 0; i < program.len(); i++) program.writen(0x00, 'b');
        program.seek(0);
        
        local maxaddress = 0, from = 0, to = 0, line = "", offset = 0x00000000;
        do {
            if (to < 0 || to == null || to >= hex.len()) break;
            from = hex.find(":", to);
            
            if (from < 0 || from == null || from+1 >= hex.len()) break;
            to = hex.find(":", from+1);
            
            if (to < 0 || to == null || from >= to || to >= hex.len()) break;
            line = hex.slice(from+1, to);
            // server.log(format("[%d,%d] => %s", from, to, line));
            
            if (line.len() > 10) {
                local len = hextoint(line.slice(0, 2));
                local addr = hextoint(line.slice(2, 6));
                local type = hextoint(line.slice(6, 8));

                // Ignore all record types except 00, which is a data record. 
                // Look out for 02 records which set the high order byte of the address space
                if (type == 0) {
                    // Normal data record
                } else if (type == 4 && len == 2 && addr == 0 && line.len() > 12) {
                    // Set the offset
                    offset = hextoint(line.slice(8, 12)) << 16;
                    if (offset != 0) {
                        server.log(format("Set offset to 0x%08X", offset));
                    }
                    continue;
                } else {
                    server.log("Skipped: " + line)
                    continue;
                }

                // Read the data from 8 to the end (less the last checksum byte)
                program.seek(offset + addr)
                for (local i = 8; i < 8+(len*2); i+=2) {
                    local datum = hextoint(line.slice(i, i+2));
                    program.writen(datum, 'b')
                }
                
                // Checking the checksum would be a good idea but skipped for now
                local checksum = hextoint(line.slice(-2));
                
                /// Shift the end point forward
                if (program.tell() > maxaddress) maxaddress = program.tell();
                
            }
        } while (from != null && to != null && from < to);

        // Crop, save and send the program 
        server.log(format("Max address: 0x%08x", maxaddress));
        program.resize(maxaddress);
        send_program();
        server.log("Free RAM: " + (imp.getmemoryfree()/1024) + " kb")
        return true;
        
    } catch (e) {
        server.log(e)
        return false;
    }
    
}


//------------------------------------------------------------------------------------------------------------------------------
// Handle the agent requests
http.onrequest(function (req, res) {
    // return res.send(400, "Bad request");
    // server.log(req.method + " to " + req.path)
    if (req.method == "GET") {
        res.send(200, html);
    } else if (req.method == "POST") {

        if ("content-type" in req.headers) {
            if (req.headers["content-type"].len() >= 19
             && req.headers["content-type"].slice(0, 19) == "multipart/form-data") {
                local hex = parse_hexpost(req, res);
                if (hex == "") {
                    res.header("Location", http.agenturl());
                    res.send(302, "HEX file uploaded");
                } else {
                    device.on("done", function(ready) {
                        res.header("Location", http.agenturl());
                        res.send(302, "HEX file uploaded");                        
                        server.log("Programming completed")
                    })
                    server.log("Programming started")
                    parse_hexfile(hex);
                }
            } else if (req.headers["content-type"] == "application/json") {
                local json = null;
                try {
                    json = http.jsondecode(req.body);
                } catch (e) {
                    server.log("JSON decoding failed for: " + req.body);
                    return res.send(400, "Invalid JSON data");
                }
                local log = "";
                foreach (k,v in json) {
                    if (typeof v == "array" || typeof v == "table") {
                        foreach (k1,v1 in v) {
                            log += format("%s[%s] => %s, ", k, k1, v1.tostring());
                        }
                    } else {
                        log += format("%s => %s, ", k, v.tostring());
                    }
                }
                server.log(log)
                return res.send(200, "OK");
            } else {
                return res.send(400, "Bad request");
            }
        } else {
            return res.send(400, "Bad request");
        }
    }
})


//------------------------------------------------------------------------------------------------------------------------------
// Handle the device coming online
device.on("ready", function(ready) {
    if (ready) send_program();
});

//------------------------------------------------------------------------------------------------------------------------------

// When we hear something from the device, split it apart and post it
device.on("postToInternet", function(dataString) {
    
    //server.log("Incoming: " + dataString);
    
    //Break the incoming string into pieces by comma
    a <- mysplit(dataString,',');

    if(a[0] != "$" || a[4] != "#")
    {
        server.log("Error: incorrect frame received: " + dataString);
        return(0);
    }
    
    //Pull the various bits from the blob
    
    //a[0] is $
    local eventCode = a[1];
    local eventText = a[2];
    local recordNumber = a[3];
    //a[16] is #
    
    if(eventCode == "7") //Override on
    {
        if(override_reported == true) //Do nothing
        {
            //server.log("Ignoring override report:" + recordNumber)
            return(0);
        }
        else
            override_reported = true; //Now go report this one
    }
    else
        override_reported = false

    if(eventCode == "3") //We've got a dollar!
    {
        dollarsRaised++;
        inputTriggers++;
    }

    if(eventCode == "5") //We saw the bill exit
        outputTriggers++;

    //Now post to data.sparkfun.com
    //Here is a list of datums: localTime, dollarsRaised, eventCode, eventText, recordNumber, inputTriggers, outputTriggers
    //A donation machine at the WOW Kid's Museum in Lafayette, CO. The machine has IR photo gates to detect when a dollar bill is being inserted. It then kicks on a dust collection system to pull the bill through 40 ft. of clear pipe. This stream monitors different aspects of the system. 

    //Get the local time that this measurement was taken
    local localTime = calcLocalTime(); 

    //Now we form the large string to pass to sparkfun
    local strSparkFun = "http://data.sparkfun.com/input/";

    local bigString = "";
    bigString = strSparkFun;
    bigString += sparkfun_public_key;
    bigString += "?" + "private_key=" + sparkfun_private_key;
    bigString += "&" + "localTime=" + localTime;
    bigString += "&" + "dollarsRaised=" + dollarsRaised;
    bigString += "&" + "eventCode=" + eventCode;
    bigString += "&" + "eventText=" + eventText;
    bigString += "&" + "recordNumber=" + recordNumber;
    bigString += "&" + "inputTriggers=" + inputTriggers;
    bigString += "&" + "outputTriggers=" + outputTriggers;

    //server.log("string to send: " + bigString);

    //Push to SparkFun
    local request = http.get(bigString);
    local response = request.sendsync();
    server.log("SparkFun response = " + response.body);

    server.log("Update complete!");
}); 

//From Hugo: http://forums.electricimp.com/discussion/915/processing-nmea-0183-gps-strings/p1
//You rock! Thanks Hugo!
function mysplit(a, b) {
  local ret = [];
  local field = "";
  foreach(c in a) {
      if (c == b) {
          // found separator, push field
          ret.push(field);
          field="";
      } else {
          field += c.tochar(); // append to field
      }
   }
   // Push the last field
   ret.push(field);
   return ret;
}

function calcLocalTime()
{
    //Get the time that this measurement was taken
    local currentTime = date(time(), 'u');
    local hour = currentTime.hour; //Most of the work will be on the current hour

    //Since 2007 DST starts on the second Sunday in March and ends the first Sunday of November
    //Let's just assume it's going to be this way for awhile (silly US government!)
    //Example from: http://stackoverflow.com/questions/5590429/calculating-daylight-savings-time-from-only-date
    local dst = false; //Assume we're not in DST
    if(currentTime.month > 3 || currentTime.month < 11) dst = true; //DST is happening!
    local DoW = day_of_week(currentTime.year, currentTime.month, currentTime.day); //Get the day of the week. 0 = Sunday, 6 = Saturday
    //In March, we are DST if our previous Sunday was on or after the 8th.
    local previousSunday = currentTime.day - DoW;
    if (currentTime.month == 3)
    {
        if(previousSunday >= 8) dst = true; 
    } 
    //In November we must be before the first Sunday to be dst.
    //That means the previous Sunday must be before the 1st.
    if(currentTime.month == 11)
    {
        if(previousSunday <= 0) dst = true;
    }
    if(dst == true) hour++; //If we're in DST add an extra hour
    
    //Convert UTC hours to local current time using local_hour
    if(hour < local_hour_offset)
        hour += 24; //Add 24 hours before subtracting local offset
    hour -= local_hour_offset;
    
    local AMPM = "AM";
    if(hour > 12)
    {
        hour -= 12; //Get rid of military time
        AMPM = "PM";
    }

    currentTime = format("%02d", hour) + "%3A" + format("%02d", currentTime.min) + "%3A" + format("%02d", currentTime.sec) + "%20" + AMPM;
    //server.log("Local time: " + currentTime);
    return(currentTime);
}

//Given the current year/month/day
//Returns 0 (Sunday) through 6 (Saturday) for the day of the week
//Assumes we are operating in the 2000-2099 century
//From: http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week
function day_of_week(year, month, day)
{

  //offset = centuries table + year digits + year fractional + month lookup + date
  local centuries_table = 6; //We assume this code will only be used from year 2000 to year 2099
  local year_digits;
  local year_fractional;
  local month_lookup;
  local offset;

  //Example Feb 9th, 2011

  //First boil down year, example year = 2011
  year_digits = year % 100; //year_digits = 11
  year_fractional = year_digits / 4; //year_fractional = 2

  switch(month) {
  case 1: 
    month_lookup = 0; //January = 0
    break; 
  case 2: 
    month_lookup = 3; //February = 3
    break; 
  case 3: 
    month_lookup = 3; //March = 3
    break; 
  case 4: 
    month_lookup = 6; //April = 6
    break; 
  case 5: 
    month_lookup = 1; //May = 1
    break; 
  case 6: 
    month_lookup = 4; //June = 4
    break; 
  case 7: 
    month_lookup = 6; //July = 6
    break; 
  case 8: 
    month_lookup = 2; //August = 2
    break; 
  case 9: 
    month_lookup = 5; //September = 5
    break; 
  case 10: 
    month_lookup = 0; //October = 0
    break; 
  case 11: 
    month_lookup = 3; //November = 3
    break; 
  case 12: 
    month_lookup = 5; //December = 5
    break; 
  default: 
    month_lookup = 0; //Error!
    return(-1);
  }

  offset = centuries_table + year_digits + year_fractional + month_lookup + day;
  //offset = 6 + 11 + 2 + 3 + 9 = 31
  offset %= 7; // 31 % 7 = 3 Wednesday!

  return(offset); //Day of week, 0 to 6

  //Example: May 11th, 2012
  //6 + 12 + 3 + 1 + 11 = 33
  //5 = Friday! It works!
}