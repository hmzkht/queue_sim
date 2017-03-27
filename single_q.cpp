#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
using namespace std;

class Event {
public:
	bool type; //0 for arrival, 1 for departure
	double time;
	Event(bool etype, double etime){
		type = etype;
		time = etime;
	};
};

double getTime(double rate);
bool compareTime(Event e1, Event e2);

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
	int length = 0, MAXBUFFER, numDropped = 0; //number of packets in queue, max packets in queue. set to 10001 to denote infinite
	double lambda, mu, currentTime = 0, busyTime = 0;
	int acount, dcount, totalQueueLen;
	acount = dcount = 0;
	vector<Event> GEL;
	cout << "MAXBUFFER = ";
	cin >> MAXBUFFER;
	cout << "Arrival rate lambda: ";
	cin >> lambda;
	cout << "Transmission rate mu: ";
	cin >> mu;
	GEL.push_back(Event(0, getTime(lambda)));
	//Event currEvent = GEL[0];
	
	for (int i = 0; i < 10000; i++) {
		Event currEvent = GEL[i];
		//arrival
		if (currEvent.type == 0) {
			currentTime = currEvent.time;
			//schedule next arrival
			GEL.push_back(Event(0, currentTime + getTime(lambda)));
			//if server free
			if (length == 0 && acount == dcount) {
				//schedule departure of this packet
				GEL.push_back(Event(1, currentTime + getTime(mu)));
			}
			//server busy
			else {
				//queue not full
				if (length < MAXBUFFER) {
					length++;	//place in queue
				}
				//queue full
				else {
					numDropped++;	//drop the packet
					acount--;
				}
				busyTime = busyTime + currentTime - GEL[i-1].time;	//update total time spent busy
				totalQueueLen += length;	//update total q length
				
			}
			acount++;
		}
		
		//departure
		else {
			currentTime = currEvent.time;
			//queue is not empty
			if (length > 0) {
				length--;
				GEL.push_back(Event(1, currentTime + getTime(mu)));
			}
			busyTime = busyTime + currentTime - GEL[i-1].time;
			totalQueueLen += length;
			dcount++;
		}

		sort(GEL.begin(), GEL.end());

	}
	//debug
	// for(int j = 0; j < GEL.size(); j++) {
		// cout << j << " Type: " << GEL[j].type << "	Time: " << GEL[j].time << endl;
	// }
	//output stats
	cout << "Dropped packets: " << numDropped << endl;
	cout << "Fraction of time busy: " << busyTime/currentTime << endl;
	cout << "Mean queue length: " << totalQueueLen/currentTime << endl;
}

double getTime(double rate) {
	double u;
	u = drand48();
	return ((-1 / rate)*log(1 - u));
}

bool compareTime(Event e1, Event e2) {
	return (e1.time < e2.time);
}