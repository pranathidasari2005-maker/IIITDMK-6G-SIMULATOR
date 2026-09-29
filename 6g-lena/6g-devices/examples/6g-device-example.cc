#include "ns3/6g-device-helper.h"
#include "ns3/6g-net-device.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

using namespace ns3;

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Ptr<Node> node = CreateObject<Node>();

    Ptr<SixGNetDevice> device = CreateObject<SixGNetDevice>();

    device->SetNode(node);
    node->AddDevice(device);

    std::cout << "====================================" << std::endl;
    std::cout << "       6G-LENA DEVICE TEST" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Node created : YES" << std::endl;
    std::cout << "Device created : YES" << std::endl;
    std::cout << "Device index : " << device->GetIfIndex() << std::endl;
    std::cout << "MTU : " << device->GetMtu() << std::endl;
    std::cout << "Link up : " << (device->IsLinkUp() ? "YES" : "NO") << std::endl;
    std::cout << "====================================" << std::endl;

    return 0;
}
