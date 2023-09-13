#include <iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>

using namespace std;

class Transition {
public:
    char inputSymbol;
    int currState;
    int nextState;

    Transition(char input, int curr, int next) {
        inputSymbol = input;
        currState = curr;
        nextState = next;
    }
};

class State {
public:
    int name;
    bool isStartState = false;
    bool isAcceptState = false;
    bool isActive = false;

    vector<Transition> transitions;

    State(int n, bool start, bool accept, bool active) {
        name = n;
        isStartState = start;
        isAcceptState = accept;
        isActive = active;
    }

    void addTransition(Transition t){
        transitions.push_back(t);
    }
};

class StateManager {
public:
    vector<State> states;

    int getStateIdx(int n) {
        for (int i = 0; i < states.size(); i++) {
            if (states[i].name == n) return i;
        }
        return -1;
    }

    void addState(int name, bool start, bool accept, bool active) {
        State s(name, start, accept, active);
        states.push_back(s);
    }

    void addTransition(char input, int curr, int next) {
        Transition t(input, curr, next);
        int currentIdx = getStateIdx(curr);
        if (currentIdx == -1) {
            addState(curr, false, false, false);
            currentIdx = getStateIdx(curr);
        }

        int nextIdx = getStateIdx(next);
        if (nextIdx == -1) {
            addState(next, false, false, false);
        }

        states[currentIdx].addTransition(t);
    }

    vector<int> getActiveStates() {
        vector<int> activeStates;
        for (int i = 0; i < states.size(); i++) {
            if (states[i].isActive) activeStates.push_back(i);
        }
        return activeStates;
    }

    void applyTransition(char inputSymbol) {
        vector<int> activeStates = getActiveStates();
        for (int i = 0; i < activeStates.size(); i++) {
            int stateIdx = activeStates[i];
            states[stateIdx].isActive = false;
            for (int j = 0; j < states[stateIdx].transitions.size(); j++) {
                if (states[stateIdx].transitions[j].inputSymbol == inputSymbol) {
                    int next = states[stateIdx].transitions[j].nextState;
                    states[getStateIdx(next)].isActive = true;
                }
            }
        }
    }

    void printActiveStates() {
        vector<int> activeStates = getActiveStates();
        for (int i = 0; i < activeStates.size(); i++) {
            int stateIdx = activeStates[i];
            cout << states[stateIdx].name << " ";
        }
        cout << endl;
    }

    void printOutput() {
        //"accept x" if active state x is an accept state
        //"reject y z" if active states neither y nor z are accept states and there are no active accept states
        vector<int> activeStates = getActiveStates();
        bool activeAcceptState = false;
        for (int i = 0; i < activeStates.size(); i++) {
            int stateIdx = activeStates[i];
            if (states[stateIdx].isAcceptState) activeAcceptState = true;
        }
        if (activeAcceptState) {
            cout << "accept ";
            for (int i = 0; i < activeStates.size(); i++) {
                int stateIdx = activeStates[i];
                if (states[stateIdx].isAcceptState) cout << states[stateIdx].name << " ";
            }
        }
        else {
            cout << "reject ";
            for (int i = 0; i < activeStates.size(); i++) {
                int stateIdx = activeStates[i];
                cout << states[stateIdx].name << " ";
            }
        }
    }

};

vector<string> split(const string& s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void BuildNFA(StateManager& stateManager, string fileName)
{
    fstream sampleFile;
    sampleFile.open(fileName, ios::in);
    if (sampleFile.is_open()) {
        string line;
        while (getline(sampleFile, line)) {
            vector<string> tokens = split(line, '\t');
            if (tokens[0] == "state") {
                if (tokens.size() == 3) { // start state or accept state
                    if (tokens[2] == "start") {
                        stateManager.addState(stoi(tokens[1]), true, false, true);
                    }
                    else {
                        stateManager.addState(stoi(tokens[1]), false, true, false);
                    }
                }
                else { //start and accept state
                    stateManager.addState(stoi(tokens[1]), true, true, true);
                }
            }
            else {  //Transition
                stateManager.addTransition(tokens[2][0], stoi(tokens[1]), stoi(tokens[3]));
            }
        }
        sampleFile.close();
    }
}

int main(int argc, char* argv[])
{
    string filename = argv[1];
    string inputString = argv[2];
    StateManager nfa;
    BuildNFA(nfa, filename);
    
    for (int i = 0; i < inputString.size(); i++) {
        nfa.applyTransition(inputString[i]);
    }
    nfa.printOutput();
}

