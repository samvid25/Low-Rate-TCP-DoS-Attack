/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // The Topology contains 5 nodes as follows:
  // 0 -> alice (sender)
  // 1 -> eve (attacker)
  // 2 -> switch (common switch between alice and eve)
  // 3 -> switch (switch conneced to bob)
  // 4 -> bob (receiver)
  /*
     n1
        \ pp1 (100 Mbps, 2ms RTT)
         \
          \             -> pp1 (100 Mbps, 2ms RTT)
           \            |
            n2 ---- n3 ---- n4
           /    |
          /     -> pp2 (1.5 Mbps, 40ms RTT) 
         /
        / pp1 (100 Mbps, 2ms RTT)
     n0
  */
  NodeContainer nodes;
  nodes.Create (5);

  // Define the Point-To-Point Links (Helpers) and their Paramters
  PointToPointHelper pp1, pp2;
  pp1.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pp1.SetChannelAttribute ("Delay", StringValue ("1ms"));

  pp2.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  pp2.SetChannelAttribute ("Delay", StringValue ("20ms"));

  // Install the Point-To-Point Connections between Nodes
  NetDeviceContainer d02, d12, d23, d34;
  d02 = pp1.Install (nodes.Get(0),nodes.Get(2));
  d12 = pp1.Install (nodes.Get(1),nodes.Get(2));
  d23 = pp2.Install (nodes.Get(2),nodes.Get(3));
  d34 = pp1.Install (nodes.Get(3),nodes.Get(4));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper a02, a12, a23, a34;
  a02.SetBase ("10.1.1.0", "255.255.255.0");
  a12.SetBase ("10.1.2.0", "255.255.255.0");
  a23.SetBase ("10.1.3.0", "255.255.255.0");
  a34.SetBase ("10.1.4.0", "255.255.255.0");


  Ipv4InterfaceContainer i02, i12, i23, i34; 
  i02 = a02.Assign(d02);
  i12 = a12.Assign(d12);
  i23 = a23.Assign(d23);
  i34 = a34.Assign(d34);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (i02.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (10240));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  pp1.EnablePcapAll("PCAPs/tcplow");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
