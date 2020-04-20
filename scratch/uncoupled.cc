
#include "ns3/flow-monitor-module.h"

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcpBulkSendExample");


int
main(int argc, char *argv[])
{
//  LogComponentEnable("MpTcpSocketBase", LOG_INFO);
LogComponentEnable("MpTcpSubflow", LOG_INFO);
  //Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(65));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(2)); // Sink
//  Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
//Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("Linked_Increases"));
Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("UNCOUPLED"));
//Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("Fully_Coupled"));
//Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("Fully_Coupled"));
Config::SetDefault("ns3::MpTcpSocketBase::ShortPlotting", BooleanValue(true));
  Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));
   uint16_t port = 999;


		
	InternetStackHelper internet;
	Ipv4AddressHelper ipv4;

  NodeContainer router;				
		router.Create (4);
		internet.Install (router);	
  NodeContainer host;				
		host.Create (2);
		internet.Install (host);
  //NodeContainer h0h3 = NodeContainer (host.Get (0), host.Get (2));
  NodeContainer h0r0 = NodeContainer (host.Get (0), router.Get (0));
  NodeContainer h0r1 = NodeContainer (host.Get (0), router.Get (1));
  
  NodeContainer r0r2 = NodeContainer (router.Get (0), router.Get (2));
  // NodeContainer r0r3 = NodeContainer (router.Get (0), router.Get (3));
  // NodeContainer r1r2 = NodeContainer (router.Get (1), router.Get (2));
  NodeContainer r1r3 = NodeContainer (router.Get (1), router.Get (3));

  NodeContainer h1r2 = NodeContainer (host.Get (1), router.Get (2));
  NodeContainer h1r3 = NodeContainer (host.Get (1), router.Get (3));

  NS_LOG_INFO ("Create channels.");
  
  PointToPointHelper p2p;

//  p2p.SetDeviceAttribute ("DataRate", StringValue ("500kbps"));
  p2p.SetDeviceAttribute ("DataRate", StringValue ("200kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
  NetDeviceContainer l0h0r0 = p2p.Install (h0r0);
  NetDeviceContainer l2r0r2 = p2p.Install (r0r2);
  NetDeviceContainer l6h1r2 = p2p.Install (h1r2);
//  NetDeviceContainer l8h0h3 = p2p.Install (h0h3);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("200kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
  NetDeviceContainer l1h0r1 = p2p.Install (h0r1);
//  NetDeviceContainer l6h1r2 = p2p.Install (h1r2);

  // NetDeviceContainer l3r0r3 = p2p.Install (r0r3);
  // NetDeviceContainer l4r1r2 = p2p.Install (r1r2);
  NetDeviceContainer l5r1r3 = p2p.Install (r1r3);
 

  NetDeviceContainer l7h1r3 = p2p.Install (h1r3);

  NS_LOG_INFO ("Assign IP Addresses.");

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0h0r0 = ipv4.Assign (l0h0r0);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1h0r1 = ipv4.Assign (l1h0r1);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2r0r2 = ipv4.Assign (l2r0r2);

 ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i5r1r3 = ipv4.Assign (l5r1r3);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i6h1r2 = ipv4.Assign (l6h1r2);
  
  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i7h1r3 = ipv4.Assign (l7h1r3);

// ipv4.SetBase ("10.1.9.0", "255.255.255.0");
//  Ipv4InterfaceContainer i8h0h3 = ipv4.Assign (l8h0h3);


  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.0001));
  l6h1r2.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  l7h1r3.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install(host.Get(1));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(10.0));

  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(i6h1r2.GetAddress(0)), port));
  source.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceApps = source.Install(host.Get(0));
  sourceApps.Start(Seconds(0.0));
  sourceApps.Stop(Seconds(10.0));

//  Address sinkAddress (InetSocketAddress (i6h1r2.GetAddress(0), port));

//  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpSocketBase::GetTypeId()));

//Ptr<Socket> localSocket =Socket::CreateSocket (h0h3.Get (1), TcpSocketFactory::GetTypeId ());

//  Ptr<MyApp> app = CreateObject<MyApp> ();
//  app->Setup (localSocket, sinkAddress, 1040, 1000, DataRate ("1Mbps"));
//  host.Get (2)->AddApplication (app);
//  app->SetStartTime (Seconds (0.));
//  app->SetStopTime (Seconds (10.));


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(10.0));
  AnimationInterface anim ("mptcp-example.xml");
 
  	
	 struct rgb {
  uint8_t r; 
  uint8_t g; 
  uint8_t b; 
 };

struct rgb red = { 255, 0, 0 }; // Red
 
struct rgb green= { 0, 255, 0 }; // Blue
struct rgb blue = { 0, 0, 255 } ; // Green
 	//MobilityHelper mobility;
//mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0), "MinY", DoubleValue (0.0),"DeltaX", DoubleValue (40.0), "DeltaY", DoubleValue (20.0), "GridWidth", UintegerValue (5), "LayoutType", StringValue ("RowFirst"));

//mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//	mobility.Install (host);
//	mobility.Install (router);
                anim.UpdateNodeColor (host.Get(0), red.r, red.g, red.b); 
		anim.UpdateNodeColor (host.Get(1), blue.r, blue.g, blue.b); 
		anim.UpdateNodeColor (router.Get(0), green.r, green.g, green.b); 
		anim.UpdateNodeColor (router.Get(1), green.r, green.g, green.b); 
		anim.UpdateNodeColor (router.Get(2), green.r, green.g, green.b); 
		anim.UpdateNodeColor (router.Get(3), green.r, green.g, green.b); 
//		anim.SetConstantPosition(router.Get(0), 33.0,33.0);
  //      	anim.SetConstantPosition(router.Get(2), 66.0,33.0);
//		anim.SetConstantPosition(router.Get(1), 33.0, 66.0);
//		anim.SetConstantPosition(router.Get(3), 66.0, 66.0);
//		anim.SetConstantPosition(host.Get(0), 0, 50.0);
//		anim.SetConstantPosition(host.Get(1), 100.0 , 50.0);  
	      anim.EnablePacketMetadata (true); 
//	AsciiTraceHelper ascii;
  //p2p.EnableAsciiAll (ascii.CreateFileStream ("myfirst.tr"));
// p2p.EnablePcapAll ("myfirst");
  	Simulator::Run ();
    //    NS_LOG_INFO ("Done.");
      

//  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO ("Done.");

}
