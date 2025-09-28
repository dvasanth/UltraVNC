#include "KaduguWrapper.hpp"

// ---------------- KaduguLibrary Implementation ----------------
bool KaduguLibrary::Init(const std::string& dllName) {
    if (hModule) return true; // already initialized

    hModule = LoadLibraryA(dllName.c_str());
    if (!hModule) {
        return false;
    }

    if (!loadFunction(startClient, "start_port_forwarding_client")) return false;
    if (!loadFunction(stopClient, "stop_port_forwarding_client")) return false;
    if (!loadFunction(startServer, "start_port_forwarding_server")) return false;
    if (!loadFunction(stopServer, "stop_port_forwarding_server")) return false;
    if (!loadFunction(getPeerId, "get_server_peer_id")) return false;
    if (!loadFunction(freePeerId, "free_peer_id")) return false;

    return true;
}

KaduguLibrary::~KaduguLibrary() {
    if (hModule) {
        FreeLibrary(hModule);
        hModule = nullptr;
    }
}

template <typename T>
bool KaduguLibrary::loadFunction(T& fn, const char* name) {
    fn = reinterpret_cast<T>(GetProcAddress(hModule, name));
    return fn != nullptr;
}

// ---------------- PortForwardingClient Implementation ----------------
PortForwardingClient::PortForwardingClient(bool autostop)
    : autoStop(autostop) {}

bool PortForwardingClient::Init(const std::string& dllPath) {
    return lib.Init(dllPath);
}

bool PortForwardingClient::Start(const std::string& serverPeer, const std::string& localForwardAddr) {
    if (!lib.startClient) return false;
    handle = lib.startClient(serverPeer.c_str(), localForwardAddr.c_str());
    return handle != nullptr;
}

PortForwardingClient::~PortForwardingClient() {
    if (autoStop) Stop();
}

void PortForwardingClient::Stop() {
    if (handle && lib.stopClient) {
        lib.stopClient(handle);
        handle = nullptr;
    }
}

// ---------------- PortForwardingServer Implementation ----------------
PortForwardingServer::PortForwardingServer(bool autostop)
    : autoStop(autostop) {}

bool PortForwardingServer::Init(const std::string& dllPath) {
    return lib.Init(dllPath);
}

bool PortForwardingServer::Start(const std::string& localServerAddr, const std::string& acceptedPeer) {
    if (!lib.startServer) {
        return false;
    }
    const char* peer = acceptedPeer.empty() ? nullptr : acceptedPeer.c_str();
    handle = lib.startServer(localServerAddr.c_str(), peer);
    return handle != nullptr;
}

PortForwardingServer::~PortForwardingServer() {
    if (autoStop) Stop();
}

void PortForwardingServer::Stop() {
    if (handle && lib.stopServer) {
        lib.stopServer(handle);
        handle = nullptr;
    }
}

std::string PortForwardingServer::GetPeerId() {
    if (!handle || !lib.getPeerId) return {};
    const char* cstr = lib.getPeerId(handle);
    if (!cstr) return {};
    std::string peerId(cstr);
    if (lib.freePeerId) {
        lib.freePeerId(const_cast<char*>(cstr));
    }
    return peerId;
}
