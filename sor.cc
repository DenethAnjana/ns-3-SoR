/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 West Laboratory, Keio University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Janaka Wijekoon <janaka@west.sd.keio.ac.jp>
 */
/*
 * module is added to the network because the dependability issues.
 * if this module is added as a separate module in the network layer
 * dependency cycles may occur between the node and the internet modules.
 * Therefore, SoR module has been implemented under the network module to 
 * keep and maintain the simplicity.
 */

#include "sor.h"
#include <iostream>
#include <stdio.h>

#include "node.h"
#include "node-list.h"
#include "net-device.h"
#include "application.h"
#include "channel.h"
#include "ns3/core-module.h"
//#include "ns3/sor-header.h" //This needs only SoR for SoR Client and Server
#include "ns3/ipv4.h"

#include "ns3/packet-metadata.h"
#include "ns3/ethernet-trailer.h"
#include "ns3/ethernet-header.h"
#include "ns3/llc-snap-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"



using namespace std;
NS_LOG_COMPONENT_DEFINE ("SoR");


namespace ns3 {
//NS_OBJECT_ENSURE_REGISTERED (SoR);

/*TypeId 
SoR::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SoR")
    .SetParent<Object> ()
    .AddConstructor<SoR> ()
    .AddAttribute ("Id", "The id (unique integer) of this SoR.",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0),
                   MakeUintegerAccessor (&SoR::sr_id),
                   MakeUintegerChecker<uint32_t> ())
     ;
  return tid;
}*/
SoR::SoR():sr_id(0)
{
	//do nothing;
}
SoR::~SoR()
{
	
}

void
SoR::DoDispose ()
{
  pktQ.empty ();
}
void 
SoR::addToQueue(Ptr<Packet> packet)
{
	pktQ.push(packet);
}

uint64_t 
SoR::IsQueueEmpty ()
{
  return pktQ.empty() ? true : false;
}	
	
bool
SoR::AnalyzePacketInformation( Ptr<Node> Node)
{
	Ptr<Packet> testPkt;
//	SoRHeader srHeader; // Needed only for SoR server and Client
	if(!pktQ.empty ())
	{
		testPkt = pktQ.front()->Copy();
		pktQ.pop();
	}
	
	testPkt->RemoveHeader (ipv4hdr);
	sourceAddress = ipv4hdr.GetSource (); // source address
  destAddress = ipv4hdr.GetDestination (); // destination address
  protocol = ipv4hdr.GetProtocol (); // protocol number 
  
  
	cout<< "This is from the Node:" << Node->GetId() << "  ";
	if(protocol == 17)//UDP protocol
	{
		cout << "UDP pacekt ";
		testPkt->RemoveHeader (udpHeader);
		destPort = udpHeader.GetDestinationPort();
	  sourcePort = udpHeader.GetSourcePort();	
	  cout << " " << sourceAddress << ":"
		  << sourcePort
		  << " > "
		  << destAddress << ":"
		  << destPort << endl;
		
		
//		// Assuming that packet is not fragmented. 
//		//	testPkt->RemoveHeader(srHeader); // Needed only for SoR server and Client
//    	uint8_t *buf = new uint8_t (testPkt->GetSize ());
//    	uint32_t bytes = testPkt->CopyData (buf, testPkt->GetSize ());
//	  // retrveive (only) String data
//    	cout << Node->GetId() << " Node received " << bytes << " of data" << endl;
//    	std::string s = std::string ((char*)buf); 
// 	    cout << s << endl;   		  	  
	}
	else if (protocol == 6) // TCP protocol
	{
		testPkt->RemoveHeader (tcpHeader);
		destPort = tcpHeader.GetDestinationPort();
		sourcePort = tcpHeader.GetSourcePort();
		cout << "TCP packet ";
	  cout << " " << sourceAddress << ":"
		  << sourcePort
		  << " > "
		  << destAddress << ":"
		  << destPort << endl;	
		
		PacketMetadata::ItemIterator i = testPkt -> BeginItem ();
    uint32_t totSize = 0;		
    while (i.HasNext ())
    {
      PacketMetadata::Item item = i.Next ();
      if (item.isFragment)
      {
        if (item.type == PacketMetadata::Item::PAYLOAD ||
            item.type == PacketMetadata::Item::HEADER ||
            item.type == PacketMetadata::Item::TRAILER)
        {
          std::cout << "Fragment [" << item.currentTrimedFromStart << ":"
                    << (item.currentTrimedFromStart + item.currentSize) << "]" << endl;
          totSize += item.currentSize;
        }
      }
      else
      {
        totSize = item.currentSize;
      }
    }
    std::cout << "Total size of the packet is:" << totSize << std::endl;
    totSize = 0;    
		// Layer 7 analysis part is not implemented.	
	}
	else if (protocol == 1) //ICMP
	{
		cout << "ICMP packet ";
	}
	else
	{
		cout << "protocol neither UDP nor TCP" << endl;
	}
	return false; // by default, packets are routed via IPv4 routing  
}

void 
SoR::CallForNodeList(Ptr<Node> Node)
{
	Ptr<Ipv4> ipv4_src = Node->GetObject<Ipv4>();	
	Ptr<Ipv4> ipv4_dst;
	
	for(uint32_t j=1;j<=(Node->GetNDevices() -1);j++)
  {
    // Get the channel information
    Ptr<Channel> ch = Node->GetDevice(j)->GetChannel();
    
    if(!ch)// Checking for the nodes are connected
      continue;
    // Get the remote edge of the channel
    uint32_t nDevices = ch->GetNDevices ();
    NS_ASSERT (nDevices);
    
    // Get the remote NetDevice
    Ptr<NetDevice> remoteNd = ch->GetDevice(nDevices-2);
    // if the NetDevice attached to me, ignore it
    if(remoteNd == Node->GetDevice(j))
      remoteNd = ch->GetDevice(nDevices-1);
    
    // Get the node attached to the found NetDevice    
    remoteNode = remoteNd->GetNode();
      
    ipv4_dst=remoteNode->GetObject<Ipv4>();

    cout << ipv4_src->GetAddress(1,0).GetLocal() 
         << " " << ipv4_dst->GetAddress(1,0).GetLocal() << endl;
  }	
}
bool 
SoR::CheckForReroute(Ptr<Packet>,Ipv4Address address)
{
  return false;
  // Yet to implement
}

void 
SoR::getModifiedPacket(Ptr<Packet> packet, Ptr<Node> node)
{
	// Yet to implement 
}
}//SoR
