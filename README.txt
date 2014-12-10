Project Details

1.Compile using g++ filename -o filename
2.Execute by using ./server -t <topology_file> -i <timeout_interval>

Or

make
Execute by using ./server -t <topology_file> -i <timeout_interval>


3.Message Format Used
Bytes
2  - Number of conenctions
16 -sender IP address
5  - port number
16 - connection IPs
5  - connections port
2  -connection Id
2  - cost

4.Bellaman Ford Algorithm

a.Initially read the file and store the neighbour and cost in distance vector matrix
b.On receiving a packet, open the packet and retrieve the details of cost of sender's connected nodes
c.Run the Algorithm
d.The algorithm computes the distance of self to other servers by reading its table.
e.It then computes the distnace of sender who sent the packet to all servers.
f.Compares the distance of self to sender and sender to other neighbour.
g.The lesser of the two distances is assigned as the distance from self to other server.
This continues for all the server.

5.Data structurs Used
a.Routing Table. Uses integer 2-D array. 
Destination  next hop    cost

b.Use a distance Vector 2D integer array.
Its used for computation ,the result of which is reflected in routing Table.



6.Limitations

1.The cost lies between 1 to 99(0 for self).
2.No new connections will be accepted in toplolgy.
3.The inf(infinite) is read as 99 on update.
4.On updatation of cost, update on both the servers.
5.The cost should be given in integers only for update.
6.On update, the server can only update self cost with its neighbours.
7.On crash, the server hangs and will not execute any command,accept any packets or send any packets.
8.The time interval is set in seconds.
9.The topology file can be read only in a specific format(enclosed in project report).







