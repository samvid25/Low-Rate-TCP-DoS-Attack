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
 #include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#define TCP_SINK_PORT 9000
#define UDP_SINK_PORT 9001

//parameters to change
#define BULK_SEND_MAX_BYTES 2097152
#define ATTACKER_DoS_RATE "2048kb/s"
#define MAX_SIMULATION_TIME 30.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

// static void scheduler(ApplicationContainer *app) {
//   app->Start(Seconds(Simulator::Now().GetDouble() + 1.0));
//   app->Stop(Seconds(Simulator::Now().GetDouble() + 5.0));
// }

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
  //
  // OnOffHelper onoff ("ns3::UdpSocketFactory",
  //                    Address (InetSocketAddress (i34.GetAddress(1),UDP_SINK_PORT)));
  // onoff.SetConstantRate (DataRate (ATTACKER_DoS_RATE));
  // onoff.SetAttribute ("OnTime", StringValue
  //   ("ns3::ConstantRandomVariable[Constant=1]"));
  // onoff.SetAttribute ("OffTime", StringValue
  //   ("ns3::ConstantRandomVariable[Constant=0]"));
  // ApplicationContainer onOffApp = onoff.Install (nodes.Get(1));
  // onOffApp.Start(Seconds(0.0));
  // onOffApp.Stop(Seconds(0.01));
  // Simulator::Schedule((Seconds(5.0)), &scheduler, &onOffApp);
  // onOffApp.Start (Seconds (2.0));
  // onOffApp.Stop (Seconds (10.0));

  //
  // int numberOfApps = MAX_SIMULATION_TIME/2;
  // UdpClientHelper attackerApps[numberOfApps];
  // ApplicationContainer attackerAppsContainer[numberOfApps];
  // for(int i=0;i<numberOfApps;i++){
    // attackerApps[i].SetAttribute("RemoteAddress",AddressValue(i34.GetAddress(1)));
    // attackerApps[i].SetAttribute("RemotePort",UintegerValue(UDP_SINK_PORT));
    // attackerApps[i].SetAttribute ("MaxPackets", UintegerValue (400));
    // attackerApps[i].SetAttribute ("Interval", TimeValue (Seconds(0.005)));
    // attackerApps[i].SetAttribute ("PacketSize", UintegerValue (1000));
  //   attackerAppsContainer[i] = attackerApps[i].Install(nodes.Get (1));
  // }
  // for(int i=0;i<numberOfApps;i++){
  //   attackerAppsContainer[i].Start(Seconds(2.0*(i+1)));
  //   attackerAppsContainer[i].Stop(Seconds(2.0*(i+1)+1));
  // }

  UdpClientHelper attackerApps;
  attackerApps.SetAttribute("RemoteAddress",AddressValue(i34.GetAddress(1)));
  attackerApps.SetAttribute("RemotePort",UintegerValue(UDP_SINK_PORT));
  attackerApps.SetAttribute ("MaxPackets", UintegerValue (1000000000));
  attackerApps.SetAttribute ("Interval", TimeValue (Seconds(0.0005)));
  attackerApps.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer ac = attackerApps.Install(nodes.Get(1));
  ac.Start(Seconds(0.0));
  ac.Stop(Seconds(MAX_SIMULATION_TIME));


  BulkSendHelper bulkSend ("ns3::TcpSocketFactory",
                         InetSocketAddress (i34.GetAddress(1), TCP_SINK_PORT));
  bulkSend.SetAttribute ("MaxBytes", UintegerValue (BULK_SEND_MAX_BYTES));
  ApplicationContainer bulkSendApp = bulkSend.Install (nodes.Get(0));
  bulkSendApp.Start (Seconds (0.0));
  bulkSendApp.Stop (Seconds (MAX_SIMULATION_TIME-10));

  PacketSinkHelper UDPsink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), UDP_SINK_PORT)));
  ApplicationContainer UDPSinkApp = UDPsink.Install (nodes.Get(4));
  UDPSinkApp.Start (Seconds (0.0));
  UDPSinkApp.Stop (Seconds (MAX_SIMULATION_TIME));

  PacketSinkHelper TCPsink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), TCP_SINK_PORT));
  ApplicationContainer TCPSinkApp = TCPsink.Install (nodes.Get (4));
  TCPSinkApp.Start (Seconds (0.0));
  TCPSinkApp.Stop (Seconds (MAX_SIMULATION_TIME));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pp1.EnablePcapAll("PCAPs/tcplow");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
