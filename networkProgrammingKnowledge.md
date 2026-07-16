# SOCKET

basically a way to communicate with other program using unix standard file descripto

## how to get that file descriptor

use socket() from sys/socket.h library
and communite throught it with send() and recv(), but you can also use standard linux read()
write() syscall but it won't be compatible with windows ofcourse but who cares about windows
anyway. also it is beter to use send() and recv() to not get confused by standard read/write
operation and network read/write operation

# Two types of internet sockets

there is actually more but only 2 matters stream socket which sends data sequentially or
FIFO and will make sure every packets is sent without any packets loss,
this is usually used in something like ssh for example. Datagram socket(connectionless socket)
which sends whatever the fuck is ready without caring about what request comes first, this
usually used in something where order doesn't matter aslong as it was sent, oh and also
this one can have packets loss like streaming services for example, you wouldn't notice
if 3 pixels is gone in 1 singular frame isn't it?

## TCP

TCP(Transmission Control Protocol) make sure data arrive error free with no packets loss
and make sure the data is also in order of FIFO, and then ip(internet protocol) deals with
internet routing so that you can connect to those specific device ip

## UDP

UDP is used by Datagram socket (connectionless socket) and it's called connectionless because
you only need ip header and connection information and no connection needed to be open between
the sender and receiver, but you can still use connect() for Datagram socket for some reason

## IP address

there are something called loopback address which basically says, im on this machine im
running now, and the loopback address for ipv4 is 127.0.0.1 and for ipv6 is simply ::1.

There is also IPv4 compatibility mode to translate IPv4 address to IPv6 so for example from
192.0.2.23 to ::ffff:192.0.2.33

### netmask

Netmask is basically in IPv4 it looks like this for example you have Class C address
"192.23.12.0" and the netmask is "255.255.255.0" so you use bitwiese AND and then
those mask will turn into the ip address back.

[still stupid]Netmask in IPv6 like 2001:db8::/32, to be honest i'm fucking confuse
with this IPv6 netmask i have no idea what the hell is this i will figure it out later
