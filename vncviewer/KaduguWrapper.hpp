#pragma once
#include <windows.h>
#include <string>

// ------------------- Types -------------------
typedef void* PortForwardingClientHandle;
typedef void* PortForwardingServerHandle;

// ------------------- Dynamic Loader -------------------
class KaduguLibrary {
public:
    KaduguLibrary() = default;
    ~KaduguLibrary();

    // No copy
    KaduguLibrary(const KaduguLibrary&) = delete;
    KaduguLibrary& operator=(const KaduguLibrary&) = delete;

    // Initialize library (returns true if success, false if failure)
    bool Init(const std::string& dllName = "kadugu_net.dll");

    // Function pointers
    typedef PortForwardingClientHandle(*StartClientFn)(const char* server_peer, const char* local_forward_addr);
    typedef void (*StopClientFn)(PortForwardingClientHandle handle);

    typedef PortForwardingServerHandle(*StartServerFn)(const char* local_server_addr, const char* accepted_peer);
    typedef void (*StopServerFn)(PortForwardingServerHandle handle);

    typedef const char* (*GetPeerIdFn)(PortForwardingServerHandle handle);
    typedef void (*FreePeerIdFn)(char* ptr);

    StartClientFn startClient = nullptr;
    StopClientFn stopClient = nullptr;
    StartServerFn startServer = nullptr;
    StopServerFn stopServer = nullptr;
    GetPeerIdFn getPeerId = nullptr;
    FreePeerIdFn freePeerId = nullptr;

private:
    HMODULE hModule = nullptr;

    template <typename T>
    bool loadFunction(T& fn, const char* name);
};

// ------------------- Client Wrapper -------------------
class PortForwardingClient {
public:
    PortForwardingClient(bool autostop = true);
    ~PortForwardingClient();

    bool Init(const std::string& dllPath = "kadugu_net.dll");
    bool Start(const std::string& serverPeer, const std::string& localForwardAddr);
    void Stop();

    // No copy
    PortForwardingClient(const PortForwardingClient&) = delete;
    PortForwardingClient& operator=(const PortForwardingClient&) = delete;

private:
    KaduguLibrary lib;
    PortForwardingClientHandle handle = nullptr;
    bool autoStop;
};

// ------------------- Server Wrapper -------------------
class PortForwardingServer {
public:
    PortForwardingServer(bool autostop = true);
    ~PortForwardingServer();

    bool Init(const std::string& dllPath = "kadugu_net.dll");
    bool Start(const std::string& localServerAddr, const std::string& acceptedPeer = "");
    void Stop();
    std::string GetPeerId();

    // No copy
    PortForwardingServer(const PortForwardingServer&) = delete;
    PortForwardingServer& operator=(const PortForwardingServer&) = delete;

private:
    KaduguLibrary lib;
    PortForwardingServerHandle handle = nullptr;
    bool autoStop;
};
