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

## Ports number

port is used by TCP and UDP like a local address, for example if you have 1 computer
that handles email and web services, then how will it differentiate those two network
io in a single ip address? well you use ports numbers. Here are some example for ports
numbers:
80\. Telnet
23\. SMTP
666\. Doom, yes that doom that people run on a bacteria

## Byte Order

Of course for some reason CPU architecture is always confusing and never agreed on the same
thing that's why different cpu architecture will have different bytes order. For example
F3B2 this byte is big endian notation which is also the Network Byte Notation and then
in little endian notation it would looks like this B2F3, Yes it's fricking flipped
and this stupid thing make it so that everytime we recv or send data to the endpoint
we need to first convert it to the host machine byte order.

Converting it to host machine byte order using arpa/inet.h library

```
htons() // h ost to n etwork s hort
htonl() // h ost to n etwork l ong
ntohs() // n etwork to h ost s hort
ntohl() // n etwork to h ost l ong
```

and yes there is no standard 64 bits variants

# Struct S

these are the data types used by sockets interface

int -> a socket descriptor

struct addrinfo this structure is used to prep the socket address structure for subsequent
use and it's also used for host name lookups and service name lookups.

```
struct addrinfo {
int ai_flags; // AI_PASSIVE/ AI_CANNONAME, etc you can learn more later
int ai_family; // AF_INET, AF_INET6, AF_UNSPEC
int ai_socktype; // SOCK_STREAM, SOCK_DGRAM
int ai_protocol; // use 0 for "any"
size_t ai_addrlen; // size of ai_addr in bytes
struct sockaddr *ai_addr; // struct sockaddr_in or _in6
char *ai_canonname; // full canonical host name

struct addrinfo *ai_next; // linked list, next node
};
```

you will load this struct and then call getaddrinfo() and it'll return a pointer
to a new linked list of these structures filled with all i need

ai_family is used to force IPv4 or IPv6 or use AF_UNSPEC for using whatever

You might see those pointer to scokaddr and question your life for what the hell
does that even do, well have no worries since you wouldn't be calling it often anyway
since getaddrinfo() will fill it for you. But it's best to know what it contains
so you understand everything

```
struct sockaddr {
unsigned short sa_family;
char sa_data[14];
};
```

sa_family can be a variety of things, but it'll be AF_INET (IPv4) or AF_INET6.
sa_data contains address and port number for the socket

to deal with struct sockaddr, people use a parallel structure:
struct sockaddr_in ("in stands for internet not input") to be used with IPv4
and sockaddr_in can be cast to a pointer to sockaddr and vice-versa so even if it wants
sockaddr you can also use sockaddr_in

```
struct sockaddr_in {
short int sin_family; // address family corresponds to sa_family
unsigned short int sin_port; // this must be in network byte order
struct in_addr sin_addr; // Internet address
unsigned char sin_zero[8] // same size as sockaddr / will be zeroed with memset()
};
```
