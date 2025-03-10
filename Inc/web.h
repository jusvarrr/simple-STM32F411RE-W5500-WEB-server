#define IMAGE_BYTES 	{\
						0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x3f, 0xff, 0xff, 0xff, \
						0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, \
						0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, \
						0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, \
						0xff, 0xfe, 0x7f, 0xff, 0xe9, 0xb7, 0xff, 0xfe, 0x7f, 0xff, 0xc9, 0xb7, 0xff, 0xfe, 0x7f, 0xff, \
						0x80, 0x01, 0xff, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe, \
						0x7f, 0xfc, 0xff, 0xff, 0x3f, 0xfe, 0x7f, 0xfd, 0xff, 0xff, 0x9f, 0xfe, 0x7f, 0xf1, 0xff, 0xff, \
						0x9f, 0xfe, 0x7f, 0xe1, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xfd, 0xff, 0xff, 0x9f, 0xfe, 0x7f, 0xe1, \
						0xff, 0xff, 0x87, 0xfe, 0x7f, 0xe1, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xf9, 0xff, 0xff, 0x9f, 0xfe, \
						0x7f, 0xe1, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xf9, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xf9, 0xff, 0xff, \
						0x9f, 0xfe, 0x7f, 0xe1, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xf9, 0xff, 0xff, 0x87, 0xfe, 0x7f, 0xfd, \
						0xff, 0xff, 0x9f, 0xfe, 0x7f, 0xfc, 0xff, 0xff, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe, \
						0x7f, 0xff, 0x00, 0x00, 0xff, 0xfe, 0x7f, 0xff, 0xed, 0x93, 0xff, 0xfe, 0x7f, 0xff, 0xed, 0xb7, \
						0xff, 0xfe, 0x7f, 0xff, 0xed, 0xb7, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, \
						0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, \
						0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, \
						0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, \
						0xff, 0xff, 0xff, 0xfc, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03 \
					}

#define index	  "HTTP/1.1 200 OK\r\n" \
                  "Content-Type: text/html\r\n" \
				  "Content-Length: <LENGTH>\r\n" \
				  "\r\n" \
				  "<!DOCTYPE html>" \
				  "<html lang=\"en\">" \
				  "<head>" \
				  "<meta charset=\"UTF-8\">" \
				  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" \
				  "<title>FetchParameterExample</title>" \
				  "<link rel=\"icon\" type=\"image/x-icon\" href=\"https://cdn-icons-png.flaticon.com/512/977/977597.png\">" \
				  "</head>" \
				  "<body>" \
				  "<h1>ParameterDisplay</h1>" \
				  "<p id=\"parameter1\"></p>" \
				  "<p id=\"parameter2\"></p>" \
				  "<script>" \
				  "function myFunction(){" \
				  "fetch('http://192.168.55.10:80/',{method:'POST',body:JSON.stringify({param1: 'button', param2: 'ADC'})})" \
				  ".then(response=>response.json())" \
				  ".then(data=>{" \
				  "const parameterValue1=data.parameter1;" \
				  "const parameterValue2=(data.parameter2*3.254/1024).toFixed(3);" \
				  "document.getElementById('parameter1').innerText=parameterValue1;" \
				  "document.getElementById('parameter2').innerText=parameterValue2 + ' V';" \
				  "})" \
				  "}" \
				  "myFunction();" \
				  "setInterval(myFunction, 50);" \
				  "</script>" \
				  "</body>" \
				  "</html>"



