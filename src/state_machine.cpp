#include "state_machine.h"

static enum ServerStates state = STATE_START; // Default state is START

const enum ServerStates StateMachine::getState()
{
    return state;
}

void StateMachine::loop()
{
    enum ServerStates state_other;
    while(1) {
        switch(state) {
            case STATE_START:
            // TODO: RPC call to retreive state of other server
            // TODO : If no response from other server, become the PRIMARY

            state_other = STATE_START;
            if (state_other == STATE_START) {
                state = DEFAULT_ROLE; // Fed in from CMakeLists.txt as a -D compiler constant
            } else if (state_other == STATE_PRIMARY) {
                // We become the backup
                // TODO: Get log of operations from PRIMARY
                state = STATE_BACKUP;
            } else if (state_other == STATE_BACKUP) {
                // Become the primary
                // TODO : Ensure we don't have 2 PRIMARIES
                state = STATE_PRIMARY;
            }
            break;

            case STATE_PRIMARY:
            // Just process requests here
            // TODO: Keep log of requests in case BACKUP is dead
            break;

            case STATE_BACKUP:
            // Client requests will be asynchronous
            // Try reaching Primary. If unreachable, become primary
            break;
        }
    }
}