#include "state_machine.h"

static enum ServerStates state = STATE_START; // Default state is START

const enum ServerStates StateMachine::getState()
{
    return state;
}

void StateMachine::setState(enum ServerStates target_state)
{
    std::string state_strings[] = {"STATE_START", "STATE_PRIMARY", "STATE_BACKUP"};
    std::cout << "[setState] Setting state to " << state_strings[target_state] << "\n";
    state = target_state;
}

void StateMachine::initState(PrimaryBackupRPCClient *g_RPCCLient)
{
    int state_other, ret;
    switch(state) {
        case STATE_START:
        state_other = g_RPCCLient->GetState(5);

        if (state_other == -1) {
            setState(STATE_PRIMARY); // If no response from other server, become the PRIMARY
        } else if (state_other == STATE_START) {
            setState(DEFAULT_ROLE); // Fed in from CMakeLists.txt as a -D compiler constant
        } else if (state_other == STATE_PRIMARY) {
            // We become the backup after RESYNCING
            ret = g_RPCCLient->ReSync();
            assert(ret == 0);
            setState(STATE_BACKUP);
        } else if (state_other == STATE_BACKUP) {
            // Become the primary
            // TODO : Ensure we don't have 2 PRIMARIES
            setState(STATE_PRIMARY);
        }
        break;

        case STATE_PRIMARY:
        // Just process requests here, keep log of requests in case BACKUP is dead
        break;

        case STATE_BACKUP:
        // Client requests will be asynchronous
        // Try reaching Primary. If unreachable, become primary
        break;
    }
}