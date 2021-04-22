# Multicasting
###### tags: `assigment` `junior` `net`

## Intro
This is a practice and learning note of socket programming, the goal is to write a UDP multicasting server and client. Server can send file to multiple client at a same time

Command should be the following form
```shell=
./server <send text name>
./client

ex:
./server send.txt
./client

```
:::warning
note: recv file name is recv.txt in default. When testing, one should puts several cilent program under different folder path, to avoid writing the same recv.txt
:::

:::info
Multicasting can be implemented with TCP but need multi-thread support, since TCP protocol link two specific hosts.  
concept:  
server.thread1 <-> client1  
server.thread2 <-> client2
:::

## Server
create new socket
```c=
sd = socket(PF_INET, SOCK_DGRAM, 0);
```

set destination group ip and port
```c=
struct sockaddr_in groupSock

groupSock.sin_family = AF_INET;
//destination group ip
groupSock.sin_addr.s_addr = inet_addr("{Group IP in string}");
//destination group port num
groupSock.sin_port = htons({portnum});
```

set local interface
```c=
//server local ip
localInterface.s_addr = inet_addr("10.0.2.15");
//if ip send from specific ip, turn it into multicast 
setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface))
```

send data to socket
```c=
sendto(sd, buffer, file_block_length, 0, (struct sockaddr*)&groupSock, sizeof(groupSock))
```

## client
create new socket
```c=
sd = socket(PF_INET, SOCK_DGRAM, 0);
```

enable reuse of socket
```c=
int reuse = 1;
setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0
```
:::warning
Because we are implementing a conceptual nuticasting on single PC. Multiple process(host) link to one socket(router), usually link two process to exactly same interface(ip,port) is invalid, so we need to modify the restriction setting
:::

set and bind socket
```c=
struct sockaddr_in localSock;
localSock.sin_family = AF_INET;
localSock.sin_port = htons(5566);
localSock.sin_addr.s_addr = INADDR_ANY;
//bind this process to any ip(which usually is this PC's ip) with port 5566
bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))
```

add to a group 
```c=
// group ip to join
group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
//my local ip
group.imr_interface.s_addr = inet_addr("10.0.2.15"); 
//set the socket
setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)
```
:::danger
Q: my assumption is that any process with different port can join same group and recv group data, but when client1 use 5566 port, client2 use 6677 port and server use 4321 port, only client1 recv data?   
I expect successful trasprotation when all port to be the same and all port not the same but know client1 client2 need to use same port and server doesn't.
:::

recv from socket
```c=
recvfrom(sd, buffer, BUFFER_SIZE, 0, NULL, NULL)
```