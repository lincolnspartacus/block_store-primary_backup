enum ServerStates {
    STATE_START = 0,
    STATE_PRIMARY = 1,
    STATE_BACKUP = 2
};

namespace StateMachine {
    const enum ServerStates getState();
    void loop();
};