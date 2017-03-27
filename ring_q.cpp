#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <deque>
#include <algorithm>
#include <time.h>
using namespace std;

class Event {
public:
	bool type; //0 for packet arrival, 1 for token
	double time;
	int dest;
	Event(bool etype, double etime, int edest){
		type = etype;
		time = etime;
		dest = edest;
	};
};

double getTime(double rate);
//for sort()
bool operator<(const Event &e1, const Event &e2) {
	if(e1.time < e2.time) {
		return true;
	}
	else {
		return false;
	}
}

int main(void) {
	int numHost = 0, pktLen, destHost, numSentPkt = 0, bytesSent = 0, frameSz = 0;
	int *numQueue;
	double lambda, totalPktDelay = 0, currentTime = 0;
	double propDelay = 10*pow(10,-6), transSpd = 100*pow(10,6);
	deque<Event> GEL;
	srand(time(NULL));
	cout << "Number of hosts N = ";
	cin >> numHost;
	cout << "Arrival rate lambda: ";
	cin >> lambda;
	numQueue =  new int [numHost];
	//schedule first arrival for each host
	//also initialize the queue lengths for each host
	for (int i = 0; i < numHost; i++) {
		GEL.push_back(Event(0, 0+getTime(lambda), i));
		numQueue[i] = 0;
	}
	//schedule first token 'arrival'
	GEL.push_back(Event(1, 1/lambda, 0));
	//sort the events
	sort(GEL.begin(), GEL.end());
	
	for (int i = 0; i < 1000000; i++) {
		// cout << i << endl;
		Event currEvent = GEL[0];
		GEL.pop_front();
		currentTime = currEvent.time;
		//packet arrival
		if (currEvent.type == 0) {
			//schedule next arrival for this host
			GEL.push_back(Event(0, currentTime + getTime(lambda),currEvent.dest));
			numQueue[currEvent.dest]++;
		}
		
		//token arrival
		else {
			//no packets to transmit at this host
			if (numQueue[currEvent.dest] == 0) {
				//schedule token to arrive at next host
				GEL.push_back(Event(1, currentTime + propDelay, (currEvent.dest+1)%(numHost-1)));
			}
			//have packets to transmit
			else {
				//simulate sending the full frame around ring
				for (int j = 0; j < numQueue[currEvent.dest]; j++) {
					//generate length of pkt
					pktLen = rand()%(1518-64) + 64;
					frameSz += pktLen;
				}
				for (int k = 0; k < numQueue[currEvent.dest]) {
					//generate destination of pkt, ensuring that it is a different host
					destHost = rand()%(numHost-1);
					//determine how many links the pkt must pass through and update total delay
					if(destHost > currEvent.dest) {
						totalPktDelay += ((frameSz*8/transSpd) + propDelay)*(destHost - currEvent.dest);
					}
					else {
						totalPktDelay += ((frameSz*8/transSpd) + propDelay)*((currEvent.dest+destHost)%(numHost-1));
					}
					numSentPkt++;
				}
				//schedule the token to arrive at next host AFTER making a full loop around ring
				GEL.push_back(Event(1, currentTime + propDelay*(numHost+1) + (frameSz*8*numHost)/transSpd, (currEvent.dest+1)%(numHost-1)));
				frameSz = 0;
				numQueue[currEvent.dest] = 0;
			}
		}
		sort(GEL.begin(), GEL.end());
		// for(int j = 0; j < GEL.size(); j++) {
			// cout << j << " Type: " << GEL[j].type << "	Time: " << GEL[j].time << " Dest: " << GEL[j].dest << endl;
		// }
	}
	//output stats
	cout << "Throughput: " << bytesSent/currentTime << endl;
	cout << "Average packet delay: " << totalPktDelay/numSentPkt << endl;
	cout << "Time: " << currentTime << endl;
}

double getTime(double rate) {
	double u;
	u = drand48();
	return ((-1 / rate)*log(1 - u));
}