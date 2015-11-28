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
#ifndef SOR_H
#define SOR_H

#include <iostream>
#include <string>
#include <map>
#include "node-list.h"
#include <queue>

#include "ns3/llc-snap-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ptr.h"
#include "ns3/net-device.h"

namespace ns3 {
//class Node;
/* ... */

class SoR
{
public:
	static TypeId GetTypeId (void);
	SoR();
	~SoR();
	
  /**
   * \brief This function analyzes the packets up to L7 information 
   * \param Node the node that analyzes the packet
   * \returns true if the packet analysis is success
   */			
	bool AnalyzePacketInformation( Ptr<Node> Node);
	
  /**
   * \brief This function is used to change the IP header of UDP packets.
   * Once a UDP packet needs to be redirected to new destination,
   * this function can change the IP header for the new destination address
   * before pass the packet to the routing moduel.
   *
   * \param packet the packet needed to be modified
   * \param node the node (SoR)
   * \returns returns the new IP Address
   */				
	Ipv4Address ReturnDestAddress(Ptr<Packet> packet, Ptr<Node> Node);
	
  /**
   * \brief retrieve the node list 
   * \param Node the SoR
   */			   	
	void CallForNodeList(Ptr<Node> Node);
	
	 /**
   * \brief get the modified packet 
   * \param packet the packet needed to be modified
   * \param node the node (SoR)
   */			
	void getModifiedPacket(Ptr<Packet> packet, Ptr<Node> node);
	
	 /**
   * \brief check the routing table (FIB) 
   * \param packet the packet needed to be modified
   * \param the destination
   */				
	bool CheckForReroute(Ptr<Packet>,Ipv4Address address);
	
	 /**
   * \brief This function is called by the node to pass the packet to the SoR module
   * \param packet the packet needed to be modified
   * \param the destination
   */			
	void addToQueue(Ptr<Packet> packet);

	void DoDispose ();
	
	/**
	* /brief Get the size of the SoR packet buffer.
	* /return true if the queueu is empty
	*/
	uint64_t IsQueueEmpty ();	
	private:	
	/*to peel the packet information */
	Ipv4Header ipv4hdr; //!< ip header information
	Ipv4Address sourceAddress; //!< the source address 
  Ipv4Address destAddress; //!< the destination address
	uint8_t protocol; //!< the protocol number 	// this is to identify the packet type
	                                            // 17 UDP
	                                            // 6 TCP
	                                            // 1 ICMP
	UdpHeader udpHeader; //!< UDP header
	TcpHeader tcpHeader; //!< TCP header
	uint16_t sourcePort; //!< Source port 
  uint16_t destPort;  //!< Destination port
	Ptr<Node> remoteNode; //!< the next hop node
	std::queue < Ptr<Packet> > pktQ; //!< the packet buffer
	Ptr<Node> m_node; //!< genaral node information
	uint64_t sr_id; //!< SoR ID
};
}// namespace ns3

#endif /* SOR_H */

