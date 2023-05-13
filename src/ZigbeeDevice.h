#ifndef ZIGBEEDEVICE_H
#define ZIGBEEDEVICE_H

#include "PersistedValue.h"
#include "PdmIds.h"
#include "PollTask.h"
#include "Queue.h"

extern "C"
{
    #include "zps_gen.h"
    #include "zps_apl_af.h"
    #include "bdb_api.h"
}

class ZigbeeDevice
{
    typedef enum
    {
        NOT_JOINED,
        JOINING,
        JOINED

    } JoinStateEnum;

    PersistedValue<JoinStateEnum, PDM_ID_NODE_STATE> connectionState;
    Queue<BDB_tsZpsAfEvent, 3> bdbEventQueue;
    PollTask pollTask;

    bool polling;
    int rejoinFailures;
    int cyclesTillNextRejoin;

    ZigbeeDevice();

public:
    static ZigbeeDevice * getInstance();

    void joinNetwork();
    void rejoinNetwork();
    void leaveNetwork();
    void joinOrLeaveNetwork();
    bool isJoined();

    void pollParent();
    bool canSleep() const;
    bool needsRejoin() const;
    void handleWakeUp();

protected:
    void handleNetworkJoinAndRejoin();
    void handleLeaveNetwork();
    void handleRejoinFailure();
    void handlePollResponse(ZPS_tsAfPollConfEvent* pEvent);
    void handleZdoBindUnbindEvent(ZPS_tsAfZdoBindEvent * pEvent, bool bind);
    void handleZdoDataIndication(ZPS_tsAfEvent * pEvent);
    void handleZdoEvents(ZPS_tsAfEvent* psStackEvent);
    void handleZclEvents(ZPS_tsAfEvent* psStackEvent);
    void handleAfEvent(BDB_tsZpsAfEvent *psZpsAfEvent);

public:
    void handleBdbEvent(BDB_tsBdbEvent *psBdbEvent);
};

#endif // ZIGBEEDEVICE_H
