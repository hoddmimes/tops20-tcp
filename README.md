# tops20-tcp
A hack illustrating how to create a tcp/ip connection and declare a tcp/ip srv on PDP-10 TOPS and KCC-6.

This is a quite odd project. Unless you do not have an incurable passion for PDP-10 / TOPS 20 system you should continue and go on.

I for fun run a simulated PDP-10 /TOPS-20 system on a Raspberry PI using the KLH-10 simulator i.e. the Panda distribution. 
It has network connectivity. It has a few Internet services running such as a Telnet, FTP and Mail daemon.  
So the basic for Internet communication is there.

So pleasure and fun I wondered if it was possible to establish Tcp/Ip P-2-P connectivity between a Linux host and the TOPS-20 system.
I took an offset in the C compiler bundled with the Panda distribution, which is KCC-6. KCC-6 does not have a fully implemented the socket interface, especially not for the server side.

The two sample programs in this project outlines the essential code required for creating a Tcp/ip client connection and declare a Tcp/ip server. The programs are by no mean correct in all aspects.
The need for this functionality on the TOPS-20 platform is NIL. :-) But it would be possible with not too  much effort to implement bind, listening and accept (at least for tcp/ip streamed sessions).
To understand the essential TCP/IP interface on TOPS-20 you should read the  JSYS_REFERENCE.MEM section 5-28.

Possibly the Utha PCC-20 compiler has a more complete implementation of the socket interface. It's however not part of the Panda distribution and must be installer possibly built on the TOPS-20 installation.