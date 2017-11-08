#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

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

  NodeContainer nodes;
  nodes.Create (5);

  PointToPointHelper nonBottleNeck;
  nonBottleNeck.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  nonBottleNeck.SetChannelAttribute ("Delay", StringValue ("1ms"));

  PointToPointHelper bottleNeck;
  bottleNeck.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  bottleNeck.SetChannelAttribute ("Delay", StringValue ("20ms"));

  NetDeviceContainer devices1,devices2,devices3,devices4;
  devices1 = nonBottleNeck.Install (nodes.Get(0),nodes.Get(2));
  devices2 = nonBottleNeck.Install(nodes.Get(1),nodes.Get(2));
  devices3 = bottleNeck.Install(nodes.Get(2),nodes.Get(3));
  devices4 = nonBottleNeck.Install(nodes.Get(3),nodes.Get(4));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address1,address2,address3,address4;
  address1.SetBase ("10.1.1.0", "255.255.255.0");
  address2.SetBase ("10.2.1.0", "255.255.255.0");
  address3.SetBase ("10.3.1.0", "255.255.255.0");
  address4.SetBase ("10.4.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
  Ipv4InterfaceContainer interfaces3 = address3.Assign (devices3);
  Ipv4InterfaceContainer interfaces4 = address4.Assign (devices4);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
