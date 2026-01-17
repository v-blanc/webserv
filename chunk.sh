printf 'POST /cgi-bin/body.py HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n7\r\nname=Ya\r\n5\r\nzid&ci\r\na\r\nity=Lyon!\r\n0\r\n\r\n' | nc localhost 8080
