AT+CWJAP="MySSID"," Logon"

AT+RST\r\n
AT+CIPSTART="TCP","https://swharden.com",80\r\n

AT+CIPSEND=69\r\n
GET /tmp/test.txt HTTP/1.1\r\nHost: 68.101.76.23\r\nUser-Agent: test\r\n\r\n
GET /tmp/test.txt HTTP/1.1\r\nHost: swharden.com\r\nUser-Agent: test\r\n\r\n

AT+CIPSEND=29\r\n
GET /tmp/test.txt HTTP/1.1\r\n\r\n

AT+CIPSEND=72\r\n
GET /tmp/test.txt HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n

