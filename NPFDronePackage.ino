#include "Arduino.h"
/*
 Server-Sent Events / EventSource DEMO
 Claudius Coenen
 based on Web Server example by David A. Mellis and Tom Igoe
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog input attached to pins A0 (optional)
 * Digital input attached to pins 5 or 6 (optional)
 This is free software. Use, modify and tinker with it however you like!
 LICENSED UNDER CC-BY-4.0 http://creativecommons.org/licenses/by/4.0/
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 42);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
//EthernetServer server(80);

char ssid[32] = "DOM-Net";
const char* password = "WeLovePB";

ESP8266WebServer server(80);

void indexPage()
{
	String msg ="<html>\n";
	msg += "<head>\n";
	msg += "<!--\n";
	msg += " Server-Sent Events / EventSource DEMO\n";
	msg += " Claudius Coenen\n";
	msg += " This is free software. Use, modify and tinker with it however you like!\n";
	msg += " LICENSED UNDER CC-BY-4.0 http://creativecommons.org/licenses/by/4.0/\n";
	msg += "-->\n";
	msg += "<style>\n";
	msg += "	body {\n";
	msg += "		font-size: 3em;\n";
	msg += "		font-family: sans-serif;\n";
	msg += "	}\n";
	msg += "</style>\n";
	msg += "</head>\n";
	msg += "<body>\n";
	msg += "	<p id=\"output\">nothing received so far</p>\n";
	msg += "	<script>\n";
	msg += "	// change this line to match the IP address of your Arduino\n";
	msg += "	var source = new EventSource('http://192.168.4.1:80/');\n";
	msg += "	// or, in case you're using the PHP Script, use this line instead.\n";
	msg += "	// var source = new EventSource('source.php');\n";
	msg += "	var outputElement = document.getElementById('output');\n";
	msg += "	var eventCounter = 0;\n";
	msg += "	source.addEventListener('testeventcc', function(e) {\n";
	msg += "		eventCounter++;\n";
	msg += "		outputElement.innerText = e.data + \" (\" + eventCounter + \" Events)\";\n";
	msg += "	}, false);\n";
	msg += "	source.addEventListener('arduino', function(e) {\n";
	msg += "		eventCounter++;\n";
	msg += "		outputElement.innerText = e.data + \" (\" + eventCounter + \" Events)\";\n";
	msg += "		var inputs = JSON.parse(e.data);\n";
	msg += "		document.body.style.backgroundColor = inputs.in5 > 0 ? '#ff0000' : '#ffffff';\n";
	msg += "		document.body.style.color = inputs.in6 > 0 ? 'fuchsia' : 'black';\n";
	msg += "		outputElement.style.opacity = inputs.A0;\n";
	msg += "	}, false);\n";
	msg += "	source.addEventListener('open', function(e) {\n";
	msg += "		console.log(\"connected\");\n";
	msg += "	}, false);\n";
	msg += "	source.addEventListener('error', function(e) {\n";
	msg += "		console.error(e);\n";
	msg += "		if (e.readyState == EventSource.CLOSED) {\n";
	msg += "		}\n";
	msg += "	}, false);\n";
	msg += "	</script>\n";
	msg += "</body>\n";
	msg += "</html>\n";

	server.send(200,"text/html",msg);
}


void setup() {
  // Open serial communications and wait for port to open:
	IPAddress myIP;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:

  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid,password);

  myIP = WiFi.softAPIP();

  //Ethernet.begin(mac, ip);


  server.on("/",indexPage);
  server.begin();
  Serial.print("server is at ");
  Serial.println(myIP);


}




void skipRequest(WiFiClient client) {
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      // if you've gotten to the end of the line (received a newline
      // character) and the line is blank, the http request has ended,
      // so you can send a reply
      if (c == '\n' && currentLineIsBlank) {
        return;
      }
      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      }
      else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
  }
}

void serverSentEventHeader(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/event-stream;charset=UTF-8");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Access-Control-Allow-Origin: *");  // allow any connection. We don't want Arduino to host all of the website ;-)
  client.println("Cache-Control: no-cache");  // refresh the page automatically every 5 sec
  client.println();
  client.flush();
}

void serverSentEvent(WiFiClient client) {
  client.println("event: arduino"); // this name could be anything, really.
  client.print("data: {");
  client.print("\"A0\": ");
  client.print(1.0 * analogRead(0) / 1024.0);
  client.print(", \"in5\": ");
  client.print(digitalRead(5));
  client.print(", \"in6\": ");
  client.print(digitalRead(6));
  client.println("}");
  client.println();
  client.flush();
}


void loop() {
  // listen for incoming clients
  //WiFiClient client2 = server.client(); sdfsdfsdfsdf

	server.handleClient();

  if (server.client()) {
    Serial.println("new client");
    skipRequest(server.client());
    serverSentEventHeader(server.client());
    while (server.client().connected()) {
      serverSentEvent(server.client());
      delay(160); // round about 60 messages per second
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    server.client().stop();
    Serial.println("client disconnected");
  }
}
