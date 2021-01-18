/*
 * AUTHOR	: RUCHIRA REVDEKAR
 * DATE		: 18-JAN-2021
 * FILE NAME	: kclient.c
 * AIM		: Establishing connection between nodes using kernel sockets
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/net.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/inet_common.h>
#include <linux/syscalls.h>
#include <linux/inet.h>
#include <asm/uaccess.h>
#define size 1024
#define SERVER_PORT 1234
MODULE_LICENSE("GPL");

struct socket *sock;
/*Module init function*/
int init_module(void)
{	
	//struct socket *sock;
	struct sockaddr_in *server_addr;
	struct kvec *vec;
	struct msghdr *sock_msg;
	int ret, ret1,ret2,sum=0;
	int *buff,*buff1,i,j=0;
	buff = (int *)kzalloc((size),GFP_KERNEL);
	
	/*Create kernel socket*/
	ret = sock_create_kern(AF_INET, SOCK_STREAM, 0, &sock);
	if (ret<0)
	{
		printk(KERN_INFO "socket creation in client failed\n");
	}
	
	server_addr = (struct sockaddr_in*) kzalloc(sizeof(struct sockaddr_in),GFP_KERNEL);

	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	//server_addr->sin_addr.s_addr = in_aton("10.208.23.240");		/*Server IP Address*/
	server_addr->sin_port = htons(SERVER_PORT);
	
	ret1 = kernel_connect(sock, (struct sockaddr*) server_addr,sizeof(struct sockaddr_in),0);

	if (ret1<0)
	{
		printk(KERN_INFO "connect failed\n");
	}

	/*Msg receiption in client*/
	vec=(struct kvec*)kzalloc(sizeof(struct kvec),GFP_KERNEL);
	sock_msg=(struct msghdr*)kzalloc(sizeof(struct msghdr),GFP_KERNEL);

	vec->iov_base = (void *)buff;		/*Received msg buffer*/
	vec->iov_len = size;			/*Msg buffer size*/
	sock_msg->msg_name=NULL;
	sock_msg->msg_namelen=0;
	sock_msg->msg_control=NULL;
	sock_msg->msg_controllen=0;

	while (sum != size)
	{
		ret2 = kernel_recvmsg(sock, sock_msg,vec,1, (size), 0);
		if(ret2 < 0)
		{
			printk(KERN_INFO "Reception in client failed\n");
		}
		else
		{
			buff1 =	buff + sum;
			for (i=j; i<((sum+ret2) / sizeof(int)); i++)
			{	
				printk(KERN_INFO "%d \t", *((int *)buff1));
				buff1++;
				j = i;
			}
			
			sum = sum + ret2;
			vec->iov_base = (void *)(buff+sum);
		}	
        }
	
	printk(KERN_INFO "msg received= %d \n",sum);
	
	/*sock_release(sock);*/		
	return 0;
}
/*Module Exit Function*/
void cleanup_module(void)
{
	sock_release(sock);
	printk(KERN_INFO "Client Exit\n");
}


