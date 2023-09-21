#include "Swarm.h"

namespace particle {
Swarm::Swarm(): lastTimeUpdated(0) {
	mParticles = new Particle[NPARTICLES];
};

Swarm::~Swarm() {
	delete[]mParticles;
};

void Swarm::update(int timePassed) {

	int interval = timePassed - lastTimeUpdated;

	for (int i = 0; i < Swarm::NPARTICLES; i++) {
		// pegando part�culas individuais do ponteiro para o array de part�culas
		mParticles[i].update(interval);

		lastTimeUpdated = timePassed;
	}
}
}