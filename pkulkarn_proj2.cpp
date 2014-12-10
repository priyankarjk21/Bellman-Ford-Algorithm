/*MNC Project 2
Routing Protocol Algorithm by Priyanka Kulkarni,UBIT : 50098043, ubit name: pkulkarn
*/
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<stdlib.h>
#include<fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>
# define STDIN 0

using namespace std;



struct server_list					// server_list structs
{
	int connection_id;
	int port;
	char ipAddress[16];
	
	

};
struct server_list serv_list[5];

struct cost_table					//cost_list structs
{
	int from_id;
	int to_id;
	int cost;
	int enable;
	int count;
	
	

};
struct cost_table cost_list[5];			// the list of cost table




/*Main class*/
class MNCProject
{

	private:

	int no_of_servers;
	int no_of_connections;
	char myIPAddress[16];
	int myPort;
	int dv_table[5][5];
	int disable_nodes[5];
	int self_connectionId;			//store self connection ID
	int count;				//to track connections
	int routing_table[5][3];		//routing Table
	int packets_received;
	int send_toServer;
	int disable_or_update;
	
	public:
	
	int start(string filename);
	void getMyIP();			//get my Ip address
	int serverCall(int timeout);
	void retrieveConnectionList();		//get the list of servers connected to
	void retrieveCostList();		//Cost list display
	int clientCall(char ipAddr[],int port,char msg[]);
	void packetSend();                      // sending packets
	int splitMsg(char buf[]);		//split the message after receiving
	void display_dvTable();		//distance vector display 
	void dv_algorithm();		//Algorithm
	void disableNode(int command);
	void checkNodeDisable();		//check if node is disabled after 3 intervals
	void display_routingTable();		//to display Routing Table
        void updateCost(int node,int cost,int disable_or_update);
        int crashServer(int sockfd);		//to check if server was crashed
        
	MNCProject()
	{
		no_of_servers=0;
		no_of_connections=0;
		send_toServer=0;
		disable_or_update=1;
		memset(myIPAddress,0,sizeof myIPAddress);
		myPort=0;
		for(int i=0;i<5;i++)
		{
			for(int j=0;j<5;j++)
			{
				if(i!=j)
				{
					dv_table[i][j]=99;			//initialise the cost to 99 initially

				}
				else
				{

					dv_table[i][j]=0;			//initialise self cost to 0
				}
			}
		}

		
	   for(int i=0;i<5;i++)
	   {
			disable_nodes[i]=0;
	   }

	    for(int i=0;i<5;i++)
	    {
	    
	    	//initialising the routing Table values
		routing_table[i][0]=i+1;
		routing_table[i][1]=0;
		routing_table[i][2]=99;
	    }
		
	}
	

} project;




int main(int argc, char* argv[])			//programs starts to execute
{

	int ch;
	string filename; 
	string interval;
	if(argc<3)
        {
		cout<<"Invalid Entry"<<endl;
		exit(1);
        }
        while ((ch = getopt(argc, argv, "t:i:")) != -1)
        switch(ch)
        {
            case 't':						//read input following -t
                 filename=optarg;
                 break;
           case 'i':
                   interval=optarg;				//read input following -i
                   break;
               default:
              exit(1);
         } 	
	if(strcmp(filename.c_str()," ")==0)
	{
		cout<<"Enter a valid Filename"<<endl;
	}
	else if(strcmp(interval.c_str()," ")==0)
	{
		cout<<"Enter a valid time interval"<<endl;
		
	}
	else
	{
		int timeout=atoi(interval.c_str());

		int return_val;
		return_val=project.start(filename);			//start to read the file and store values
		if(return_val==1)
		{
			project.getMyIP();			//retrieve IP of running machine from list
			int server_return=project.serverCall(timeout);			//call to connect
		}
		return return_val;
	}

}

/*The program initiates 
open file and retrieves the details from file and puts in the structs
*/

int MNCProject :: start(string filename)
{
	cout<<"Hello,Welcome to MNC Project2- Routing Protocols"<<endl;
	char *token;
	char str[2]=" ";
	char buf[500];
	string line;
	int length;
	ifstream myfile;
	myfile.open(filename.c_str());
	int count=0;

	 while (getline(myfile,line))
   	 {

		count++;

		if(count==1)
		{
			
			no_of_servers = atoi(line.c_str());					//number of servers
			
		}	
		if(count==2)
		{
			no_of_connections=atoi(line.c_str());						//number of connections
			
		}
		if(count>2 && (count<=(no_of_servers+2)))//reference:http://stackoverflow.com/questions/289347/using-strtok-with-a-stdstring
		{
			char *pch;
			char *dup = strdup(line.c_str());
			pch = strtok(dup, " ");
			int values_count=0;

			while(pch!=NULL)					//server list
			{

				   values_count++;
				  if(values_count==1)
				  {					 
					 serv_list[count-3].connection_id=atoi(pch);		//get connection ID
\
				  }
				  if(values_count==2)
				  {

					 strcpy(serv_list[count-3].ipAddress,pch);		//get the IP Address
				  }
				  if(values_count==3)
				  {	
				          serv_list[count-3].port=atoi(pch);			//get Port number

				  }
				
				 pch = strtok (NULL, " ");
			}
			free(dup);
			

		}
		if(count>(no_of_servers+2) &&(count<=(no_of_connections+no_of_servers+2)))		//get the list 
		{
			
				char *pch;
				char *dup = strdup(line.c_str());
				pch = strtok(dup, " ");
				int values_count=0;
				while(pch!=NULL)					//server list
				{

					  values_count++;
					  int cost_var=count-(no_of_servers+3);
					  if(values_count==1)
					  {					 
						 cost_list[cost_var].from_id=atoi(pch);

					  }
						 
					  if(values_count==2)
					  {
					
					     cost_list[cost_var].to_id=atoi(pch);
						 
				
					  }
					  if(values_count==3)
					  {	
					       cost_list[cost_var].cost=atoi(pch);
					    dv_table[cost_list[cost_var].from_id-1][cost_list[cost_var].to_id-1]=cost_list[cost_var].cost;
					       cost_list[cost_var].enable=1;

						//assigning values in routing table on reading file
					      routing_table[cost_list[cost_var].from_id-1][0]=cost_list[cost_var].from_id;
					      routing_table[cost_list[cost_var].to_id-1][0]=cost_list[cost_var].to_id;
					      routing_table[cost_list[cost_var].from_id-1][1]=cost_list[cost_var].from_id;
					      routing_table[cost_list[cost_var].to_id-1][1]=cost_list[cost_var].to_id;
					      routing_table[cost_list[cost_var].from_id-1][2]=0;
					      routing_table[cost_list[cost_var].to_id-1][2]=cost_list[cost_var].cost;
					      self_connectionId=cost_list[cost_var].from_id;
					      cost_list[cost_var].enable=1;
					      cost_list[cost_var].count=1;

					 }

					 pch = strtok (NULL, " ");
				}
				free(dup);
			
		}
	 }
	

	project.getMyIP();		//retrieve self IP address and Port Number
	//project.display_dvTable();
	display_routingTable();		//To display the routing Table on setting new connection
	myfile.close();
	return 1;


}

/*Initially Assigning values to Routing Table*/
void MNCProject :: display_routingTable()
{
	cout<<endl;
	cout<<"The Routing Protocol Table"<<endl;
	cout<<endl<<"Destination"<<"\t"<<"Next Hop"<<"\t"<<"Cost"<<endl;

	if(no_of_servers>0)
	{
		for(int i=0;i<no_of_servers;i++)
		{
			for(int j=0;j<3;j++)
			{
			
					cout<<routing_table[i][j]<<"\t"<<"\t";
			}
			cout<<endl;
		}
	}
	else
	{
		cout<<"No servers connected to display"<<endl;
	}

}


/*Retrive the IP address of self*/
void MNCProject :: getMyIP()
{
	int i=0;
	int j=0;
	int connectionId;
	
	if(cost_list!=NULL)
	{
		connectionId=cost_list[0].from_id;
	}
	for(i=0;i<no_of_servers;i++)
	{
		if(serv_list!=NULL)
		{
			if(serv_list[i].connection_id==connectionId)
			{
				
				myPort=serv_list[i].port;			//retrieving the running port number
				strcpy(myIPAddress,serv_list[i].ipAddress);	//retrieving the IP address
				
			}
		}
		
	}
	
	

}

void MNCProject ::retrieveConnectionList()			//retrieve server_list
{
	int i=0;
	
	for(i=0;i<no_of_servers;i++)
	{
		cout<<"  "<<serv_list[i].connection_id<<" "<<serv_list[i].ipAddress<<" "<<serv_list[i].port<<endl;
	}

}

void MNCProject :: retrieveCostList()					//retrieve cost_list
{
	int i=0;
	
	
	for(i=0;i<no_of_connections;i++)
	{
		cout<<"  "<<cost_list[i].from_id<<" "<<cost_list[i].to_id<<" "<<cost_list[i].cost<<endl;
	}

}

/*Packet sending format*/
void MNCProject :: packetSend()
{

	int k=0;
	int j=0;
	char myportno[5];
	int msg_length;
	if(no_of_servers>0)
	{
		msg_length=23+(25*no_of_servers)+1;
	}
	else
	{
		msg_length=500; 
	
	}
	char msg_to_send[msg_length];
	memset(myportno,0,sizeof(myportno));
	sprintf(myportno,"%d",myPort);	
        myportno[strlen(myportno)]='\0';
	char update[2];
	char my_ip[16];
	char my_port[5];
	char server_ip[16];
	char server_port[5];
	char cost[2];
	char connections[2];
	int connection_id;
	char connection_id_str[1];
	memset(connections,0,sizeof connections);

	//formatting a message to send
	memset(msg_to_send,0,sizeof(msg_to_send));
	sprintf(connections,"%-2d",no_of_connections);
	strcpy(msg_to_send,connections);
	
	memset(my_ip,0,sizeof(my_ip));
        memset(my_port,0,sizeof(my_port));

	sprintf(my_port,"%-5s",myportno);
	strcat(msg_to_send,my_port);
	sprintf(my_ip,"%-16s",myIPAddress);
	strcat(msg_to_send,my_ip);
	
	
	

		int dv_tableCost=0;
		for(j=0;j<no_of_servers;j++)
		{ 
			
				if(j+1!=cost_list[0].from_id) // break;			//don send self details
				{
					sprintf(server_ip,"%-16s",serv_list[j].ipAddress);
					strcat(msg_to_send,server_ip);
					sprintf(server_port,"%-5d",serv_list[j].port);
					strcat(msg_to_send,server_port);
					sprintf(connection_id_str,"%-2d",j+1);
					strcat(msg_to_send,connection_id_str);

					dv_tableCost=routing_table[j][2];		//retrieve cost from routing algorithm
					sprintf(cost,"%-2d",dv_tableCost);
					strcat(msg_to_send,cost);
					
					
				}
		}
		
		
	
	msg_to_send[strlen(msg_to_send)]='\0';	
	
	if(send_toServer==0)
	{
		for(k=0;k<no_of_connections;k++)
		{

		
			for(j=0;j<no_of_servers;j++)
			{ 
				if(cost_list[k].to_id==serv_list[j].connection_id)
				{
				  if(cost_list[k].enable==1)		//check if flag is enabled to send or not
				   {
				   	 int sent=clientCall(serv_list[j].ipAddress,serv_list[j].port,msg_to_send);//call to send packet
				   	 cout<<endl<<"Packet sent to"<<" "<<cost_list[k].to_id<<endl;
				   	 
				   }
				}
			}
		
		}
	}

	

}

/*To display the distance Vector table*/
void MNCProject :: display_dvTable()
{
	
	for(int i=0;i<no_of_servers;i++)	
	{
		cout<<i+1;
		for(int j=0;j<no_of_servers;j++)
		{
			cout<<" "<<dv_table[i][j]<<" ";		//display DV table
		}
		cout<<endl;
	}
			



}


/*To retrieve the message packet received*/
int MNCProject :: splitMsg(char buf[])
{		
	buf[strlen(buf)]='\0';
	int i=0;
	int j=0;
	char recvServer_ip[16];
	int recvServer_port=0;
	int pkt_connections=0;
	string pkt_connections_str;
	string sender_ip_buff;
	string sender_ip;
	string sender_port_str;
	int sender_port=0;


	char *pch;
	char *pch1;
	char *pch2;
	char *pch3;
	string str(buf,strlen(buf));
	memset(buf,0,strlen(buf));

	//retrieve the number of connections the sender has
	pkt_connections_str=str.substr(0,2);
	char *dup=strdup(pkt_connections_str.c_str());
	pch = strtok(dup, " ");
	while(pch!=NULL)
	{
		pkt_connections=atoi(pch);
		pch = strtok (NULL, " ");
	}


	//get sender's port number
	sender_port_str=str.substr(2,5);
	char *dup2=strdup(sender_port_str.c_str());
	pch2 = strtok(dup2, "\0, ");
	while(pch2!=NULL)
	{
		sender_port=atoi(pch2);
		pch2 = strtok (NULL, " ");
		
	}

	//get the sender IP address from msg
	sender_ip=str.substr(7,16);
	char *dup1=strdup(sender_ip_buff.c_str());
	pch1 = strtok(dup1, " ");
	while(pch1!=NULL)
	{
		sender_ip=pch1;
		pch1 = strtok (NULL, " ");
		
	}
	

	char *dup3=strdup(sender_ip.c_str());
	pch3 = strtok(dup3, " ");
	while(pch3!=NULL)
	{
		sender_ip=pch3;
		pch3 = strtok (NULL, " ");
		
	}
	int pointer=23;			//the bits scanned till now
	int connectId=0;
	int cost=0;
	int receiver_id=0;

	//get the connection Id by matching server IP and Port number

	for(i=0;i<no_of_servers;i++)
	{
		
		string serverIP(serv_list[i].ipAddress);
		int compare_result=serverIP.compare(sender_ip);
		if((serv_list[i].port==sender_port)&&(compare_result==0))
		{
			receiver_id=serv_list[i].connection_id;		
			
			
		}
		
	}
	int scan_flag=0;			//check if flag for receiver is enabled or not
	for(int k=0;k<no_of_connections;k++)
	{
		if(cost_list[k].to_id==receiver_id)
		{
			if(cost_list[k].enable==1)
			{
		
				scan_flag=1;
		
			}
		}
	}
	
	//scan cost of each node that the sender is connected to and call method to calculate minimum
	if(scan_flag==1)
	{
		int j=0;
		for(j=0;j<no_of_servers-1;j++)
		{			
				
			string value=str.substr(pointer+16+5,2);
			connectId=atoi(value.c_str());
			value=str.substr(pointer+16+5+2,2);
	
			cost=atoi(value.c_str());
			pointer=pointer+16+5+2+2;
			dv_table[receiver_id-1][connectId-1]=cost;  //update neighbour's cost in distance-vector table
			cost=0;
			connectId=0;
			value='\0';
					
	
				
		}// end of for to scan all servers cost
		dv_algorithm();	//to calculate DV Algorithm
		packets_received=packets_received+1;
	        cout<<"Packet received from"<<" "<<receiver_id<<endl;
		free(dup);
		free(dup1);
		free(dup2);
		free(dup3);
		return receiver_id;			//return the Id of receiver to increment its count of packet receipt

	}
	free(dup);
free(dup1);                                                                                                                                           
	free(dup2);
	free(dup3);	
	return 0;
	
}

/*To calculate the minimum value*/
void MNCProject :: dv_algorithm()
{
	

	int connect_id;
	int cost_recv;
	int cost_connect_id;
	int dvTable_cost;
	for(int k=0;k<no_of_servers;k++)
	{
		if(k!=self_connectionId-1)		//do not check or update for self cost
		{
			connect_id=routing_table[k][1]-1;

			if(connect_id >= 0)
			{		
				cost_connect_id=routing_table[k][2];
				dvTable_cost=dv_table[self_connectionId-1][connect_id]+dv_table[connect_id][routing_table[k][0]-1];
							
				if(dvTable_cost!=cost_connect_id)
				{
						routing_table[k][2]=99;		//set cost to infinity
				}
				
			}
			
		}

	}
	

	//Bellaman Ford Algorithm
	int connect_cost;
	for(int j=0;j<no_of_servers;j++)
	{
		for(int l=0;l<no_of_servers;l++)
		{
			cost_recv=routing_table[j][2];
			connect_cost=dv_table[self_connectionId-1][l]+dv_table[l][j];
			if((connect_cost)<cost_recv)
			{
				routing_table[j][1]=l+1;			//assigning next hop
				routing_table[j][2]=connect_cost;		//setting values in routing table

			}
	
			
			
		}


	}
	
}
 
 /*Crash the server*/
int MNCProject :: crashServer(int sockfd)
{
	for(int d=0;d<no_of_connections;d++)
	{
		cost_list[d].enable=0;
			
	}
	send_toServer=1;
	close(sockfd);
	return 1;



}

/*Check if node is disabled*/
void MNCProject :: checkNodeDisable()
{
	for(int rp=0;rp<no_of_connections;rp++)
	{
	
		if(cost_list[rp].enable==1)			//check if the node for the connection is enabled.
		{
			if(cost_list[rp].count==0)
			{
					disable_or_update=1;
					updateCost(cost_list[rp].to_id,99,disable_or_update);
					dv_algorithm();						//run the algorithm
					cout<<"Server"<<" "<<cost_list[rp].to_id<<"  has been disabled"<<endl;
			}
			else
			{
				cost_list[rp].count=0;
			}
		}
		
		
		
	}
}

/*Disable the node*/
void MNCProject :: disableNode(int to_node)
{
	int node_exists=0;
	for(int i=0;i<no_of_connections;i++)
	{
		if(to_node==cost_list[i].to_id)
		{

			disable_or_update=0;		//stop sending the packet
			updateCost(cost_list[i].to_id,99,disable_or_update);
			node_exists=1;
		}
			
	}
	if(node_exists!=1)
	{
		cout<<"Does not belong to connection list,Cannot disable"<<endl;
		
	}
	

}
 

/*Update the cost of Routing Table*/
void MNCProject :: updateCost(int to_node,int new_cost, int disable_or_update)
{


	int node_to_update=to_node-1;
	routing_table[node_to_update][2]=new_cost;			//update the new cost
	routing_table[node_to_update][1]=to_node;
	dv_table[self_connectionId-1][node_to_update]=new_cost;
	
	//display the table on updation
	display_routingTable();

        
																					
}


/* to make serverCall*/

int MNCProject :: serverCall(int timeout)				//source: Beej Guide
{

    int sockfd;			//socket descriptor
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;		//address length of the receving socket
    char s[INET6_ADDRSTRLEN];
    char portno[4];		//to set port number in char to connect
    struct timeval tv;		//to set time interval parameters
    fd_set readfds;		//the fd set
    fd_set master;		//master fd set		
    int fdmax=0;
    int msg_length=23+(25*no_of_servers)+1;
    char buf[msg_length+100];		//to store input received(+100 to in case receives updates simultaneously
    tv.tv_sec = timeout;			//change to argv[5]
    tv.tv_usec=0;
    sprintf(portno,"%d",myPort);		//take input in char
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 				// set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; 				// use my IP
    int numbytes=0;				//number of bytes received
    int yes=1;
    int new_sockets[4];
    string input_command;
    	


   if ((rv = getaddrinfo(NULL, portno, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
	{
            cout<<"listener: socket"<<endl;
            continue;
        }
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)			//bind the socket
        {
            close(sockfd);
            cout<<"listener: bind"<<endl;
            continue;
        }
	
	if (p == NULL) 
	{
        	fprintf(stderr, "listener: failed to bind socket\n");			//listening failed

   	}
	
        freeaddrinfo(servinfo);					//free the address info of the server
        break;
    }
    addr_len = sizeof their_addr;		//the size of address
    FD_ZERO(&master);				//making the FD master as 0
    FD_ZERO(&readfds);
    FD_SET(sockfd, &master);
    FD_SET(STDIN, &master);
    fdmax=sockfd;				//set maxfd as sockfd
    int input_command1;


    int countdown=0;
    for(;;)
    {

		cout<<"server"<<cost_list[0].from_id<<"<<";
		fflush(stdout);
		readfds=master;
		int select_set=select(fdmax+1,&readfds,NULL,NULL,&tv);

		if(select_set==-1)	//setting the select parameters
		{
			//perror("select\n");

		}
		else if(select_set==0)
		{
			tv.tv_sec = timeout;		//reset the time interval
			tv.tv_usec=0;
			project.packetSend();
			display_routingTable();
			
			//to check if the sender has disabled the node at its ends
			countdown=countdown+1;
			if(countdown==3)
			{

				checkNodeDisable();  //method call to make node disable
				countdown=0;		//reset the timer
			}
			
			
			
		}
		else
		{

			if(FD_ISSET(STDIN,&readfds))			//read input from keyboard
			{
				
				cin>>input_command;
				fflush(stdout);
				if(strcmp(input_command.c_str(),"packets")==0)
				{
					cout<<endl;
					cout<<"Number of packets received are"<<" "<<packets_received<<endl;
					std::cin.clear(); 	
					packets_received=0;
					
				}
				else if(strcmp(input_command.c_str(),"display")==0)		//display routing table
				{

					cout<<endl;				
					display_routingTable();
					std::cin.clear(); 	
					fflush(stdout);					

				}
				else if(strcmp(input_command.c_str(),"update")==0)		//update Routing Table
				{

					int updateNode;
					int self_node;
					string updatedCost;
					int cost;
					cin>>self_node>>updateNode>>updatedCost;
					std::cin.clear(); 	
					if(updateNode<=0 && updateNode>6)
					{
						cout<<"Invalid Node number"<<endl;
					}
					else if(strcmp(updatedCost.c_str()," ")==0)
					{
						cout<<"Please enter the update cost"<<endl;
					}			
					else if(strcmp(updatedCost.c_str(),"inf")==0)
					{
						cost=99;
						updateCost(updateNode,cost,disable_or_update);
						
					}
					else if(self_node!=self_connectionId)
					{

						cout<<"Cannot update except self connected Server"<<endl;
						
						fflush(stdout);	
					}
					
					else if((cost=atoi(updatedCost.c_str()))!=0)
					{
						cost=atoi(updatedCost.c_str());
						if(cost<=0 || cost>99)
						{
							cout<<"Invalid Cost to update,Enter cost between 1 to 99"<<endl;
							fflush(stdout);	
						}
						else
						{
							disable_or_update=1;
							for(int i=0;i<no_of_connections;i++)
							{
								if(cost_list[i].to_id==updateNode)
								{
									cost_list[i].enable=1;
								
								}
							
							}
							updateCost(updateNode,cost,disable_or_update);		//call update method
							cout<<"Cost of server "<<" "<<updateNode<<" has been update"<<endl;
							
						}

					}
					else
					{
					
						cout<<"Inappropriate Input for update"<<endl;
					}
				}
				else if(strcmp(input_command.c_str(),"step")==0)		
				{
					project.packetSend();
					for(int i=0;i<no_of_connections;i++)
					{
						if(cost_list[i].enable==1)
						{
							cout<<"Packet sent to"<<" "<<cost_list[i].to_id<<" "<<"successfully"<<endl;
						}
					
					}
					 tv.tv_sec = timeout;			//reset the timer
   					 tv.tv_usec=0;

										

				}
				else if(strcmp(input_command.c_str(),"disable")==0)		//disable the node
				{

					std :: cin>>input_command1;
					std::cin.clear(); 	
					int nodePresent=0;
					
					if((input_command1>0) && (input_command1<=no_of_servers) && (input_command1!=self_connectionId))
					{
					
						for(int i=0;i<no_of_connections;i++)
						{
							if(cost_list[i].to_id==input_command1)
							{
								cost_list[i].enable=0;
								disableNode(input_command1);
								cout<<"Server "<<input_command1<<" "<<"has been disabled"<<endl;
								nodePresent=1;
								
							}
						}
						if(nodePresent!=1)
						{
							cout<<"Could not disable server"<<input_command1<<endl;
						
						}
						
					
					}
					else if(input_command1==self_connectionId)
					{
						cout<<"Cannot disable self connections"<<endl;
					
					}
					else
					{
						cout<<"Invalid Input for disable"<<endl;
					}
									

				}
				else if(strcmp(input_command.c_str(),"crash")==0)		//node will exit the topology
				{
					std::cin.clear(); 	
					int crashReturn=crashServer(sockfd);
					if(crashReturn==1)
					{
						cout<<"The server"<<" "<<self_connectionId<<" "<<"has been crashed"<<endl;
					
					}
					while(1){};


				}
				else if(strcmp(input_command.c_str(),"exit")==0)
				{
					close(sockfd);
					exit(0);		
				}
				else
				{

					cout<<"Invalid Command"<<endl;			//invalid Command Entry
					std::cin.clear(); 	
				}
				
			}
			//read from the socket
			else if(FD_ISSET(sockfd,&readfds))//source:http://stackoverflow.com/questions/15731924/udp-multi-client-server-basics
			{

				int count=0;
				memset(buf,0,sizeof buf);		
				if ((numbytes = recvfrom(sockfd, buf, sizeof(buf) ,0,(struct sockaddr *)&their_addr, &addr_len)) == -1)
				{
	       				 perror("recvfrom");

	    			}

				buf[strlen(buf)] = '\0';	


				int receiverId=project.splitMsg(buf);
				cout<<endl;

					
				//to track the receiver sending message
				for(int rp=0;rp<no_of_connections;rp++)
				{
					if(cost_list[rp].to_id==receiverId)
					{
						cost_list[rp].count=cost_list[rp].count+1;
						
					}
				}


				fflush(stdout);
				memset(buf,0,strlen(buf));


			}
			
	}

    }
 
  return 1;
}


	

/*The client call to send the packets*/

int MNCProject :: clientCall(char ipAddr[],int port,char msg[])
{

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char send_buffer[1000];
    char portno[5];
   
    sprintf(portno,"%d",port);			//converting port number to char
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    memset(send_buffer,0,sizeof send_buffer);
    strcpy(send_buffer,msg);			//the message to be sent is put in buffer

    if ((rv = getaddrinfo(ipAddr,portno, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
	{
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) 
    {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

   if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,p->ai_addr, p->ai_addrlen)) == -1)  
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;


}

