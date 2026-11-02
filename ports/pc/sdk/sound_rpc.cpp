// The SIF RPC behind RainbowPS2Snd's clients: the IOP sound driver
// (PS2SOUND.IRX) answers on three. Two batch clients (0x12346, 0x23456)
// take a RainbowBatchData (count, batch number, 64 commands of 0x114
// bytes: the command word and its arguments) through functions 0x20/0x21;
// the normal client (0x34567) takes one command, its number the RPC
// function, and replies in recv[0]. ports/pc/audio/driver.cpp runs them, at
// once, on the calling thread: replies are synchronous (the PS2's batch
// calls were asynchronous, mode 1, but nothing waits on their results).
#include "audio/audio.h"
#include "port.h"

#include <cstring>
#include <map>

namespace {

// sifrpc.h's client block as RainbowPS2Snd.h has it: Init polls `serve`.
struct SifClient {
    char pad0[0x24];
    void *serve;
};

std::map<const void *, unsigned> g_requests; // client -> bound request id

} // namespace

extern "C" {

int sceSifBindRpc(void *client, unsigned int request, unsigned int mode)
{
    SifClient *c = (SifClient *)client;
    g_requests[client] = request;
    c->serve = c; // any non-null server
    return 0;
}

int sceSifCallRpc(void *client, unsigned int func, unsigned int mode, void *send, int ssize, void *recv,
                  int rsize, void (*end)(void *), void *end_param)
{
    unsigned request = g_requests.count(client) ? g_requests[client] : 0;
    int reply = 0;
    if (request == 0x34567) {
        int32_t args[0x134 / 4] = {};
        if (send && ssize > 0)
            memcpy(args, send, ssize < (int)sizeof(args) ? ssize : sizeof(args));
        reply = audio::Command(func, args);
    } else if ((request == 0x12346 || request == 0x23456) && send) {
        const char *batch = (const char *)send;
        int count;
        memcpy(&count, batch, 4);
        if (count < 0 || count > 64)
            count = 0;
        for (int i = 0; i < count; i++) {
            const char *cmd = batch + 8 + i * 0x114;
            int32_t args[(0x114 - 4) / 4];
            int number;
            memcpy(&number, cmd, 4);
            memcpy(args, cmd + 4, sizeof(args));
            audio::BatchCommand(number, args);
        }
    }
    if (recv && rsize > 0) {
        memset(recv, 0, rsize);
        if (rsize >= 4)
            memcpy(recv, &reply, 4);
    }
    if (end)
        end(end_param);
    return 0;
}

int sceSifCheckStatRpc(void *client)
{
    return 0; // never busy: calls complete at once
}

} // extern "C"
