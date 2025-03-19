/* Name: Darion
   Lab: 7
   ID: 3122890
*/

#include <string.h>
#include <stdio.h>
#include "sysio.h"
#include "serf.h"
#include "phys_cc1350.h"
#include "plug_null.h"
#include "tcvphys.h"
#include "ser.h"

#define CC1350_BUF_SZ	250

int sfd;
/* in class node */
int nodeID = 9;
/* counter for seq*/
int c = 0;
/* char for menu*/
char ch;
int rNode;
/* str for message*/
char str1[27];

/* structure of message*/
struct msg {
  int senderID;
  int receiverID;
  int sequence_number;
  char message[27];
};

fsm receiver {
    address packet;
    struct msg *payload;

    state Receiving:
        packet = tcv_rnp(Receiving, sfd);
        payload = (struct msg*)(packet + 1);
	/* if reciever ID is 0, then broadcast*/
        if (payload->receiverID == 0) {
            ser_outf(Receiving, "Broadcast from node %d (Seq: %d): %s\r\n", 
                payload->senderID, payload->sequence_number, payload->message);
	/* if reciever ID is == to nodeID, then direct message*/
        } else if (payload->receiverID == nodeID) {
            ser_outf(Receiving, "Message from node %d (Seq: %d): %s\r\n", 
                payload->senderID, payload->sequence_number, payload->message);
        }

        tcv_endp(packet);
        proceed Receiving;
}


fsm root {
    byte count = 0;
    struct msg * payload;
    address packet;

    state INIT:
        phys_cc1350 (0, CC1350_BUF_SZ);
        tcv_plug(0, &plug_null);
        sfd = tcv_open(NONE, 0, 0);
        if (sfd < 0) {
            diag("unable to open TCV session");
            syserror(EASSERT, "no session");
        }
        tcv_control(sfd, PHYSOPT_ON, NULL);
	proceed Menu;
    state Menu:
	/* display menu options*/
	ser_outf(Menu,"P2P Chat (Node#%d)\n\r(C)hange node ID\n\r(D)irect transmission\n\r(B)roadcast transmission\n\rSelection: ", nodeID);
    state MenuInput:
	/* get menu input from user and act accordingly*/
	ser_inf(MenuInput, "%c", &ch);
	/* if C, change node ID*/
	if (ch == 'C'){
		ser_outf(MenuInput, "New node ID(1-25): ");
		proceed ChangeNodeID;
	}
	/* if D, send direct message*/
	else if (ch == 'D'){
		ser_outf(MenuInput, "Reciever node ID(1-25)");
		proceed GetRecieverNode;
	}
    state GetRecieverNode:
	ser_inf(GetRecieverNode, "%d", &rNode);
	proceed GetMessage;

    state GetMessage:
	/* use ser in to allow spaces in message*/
    	ser_outf(GetMessage, "Message: ");
    	ser_in(GetMessage, str1, 27); 
   	proceed Sending;

    state ChangeNodeID: 
	ser_inf(ChangeNodeID, "%d", &nodeID);
	proceed Menu;

    state Sending:
        packet = tcv_wnp(Sending, sfd, 34);
	packet[0] = 0;
	char * p = (char *) (packet+1);
	/* increment p until message is sent*/
	*p=nodeID; p++;
	*p=rNode; p++;
	*p=c+=1; p++;
	strcpy(p,str1);
	ser_outf(Sending, "Message Sent\r\n");
    
	tcv_endp(packet);
	/* proceed back to menu*/
	proceed Menu;

}
