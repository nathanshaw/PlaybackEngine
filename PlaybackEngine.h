#ifndef __PLAYBACK_ENGINE_H__
#define __PLAYBACK_ENGINE_H__

#include "Rhythm.h"
#include "../Mechanisms/Mechanisms.h"
#include <NeopixelManager.h>

class PlaybackEngine {
    public:
      PlaybackEngine();
      void playRhythm(Rhythm * r);
      void update();
      void linkBellMechanism(BellMechanism * m);
      void linkPeckerMechanism(Woodpecker * m);
      void linkNeoGroup(NeoGroup * n);
      bool isActive(){return playback_active;};
      // void addSolenoidMechanism(uint8_t act, uint8_t damp);
      
    private:
      void updateBells();
      void updateWoodpeckers();

      BellMechanism * bell_mechanisms[3];
      Woodpecker * woodpeckers[3];
      NeoGroup * neos[3];

      uint8_t num_bell_mechanisms = 0;
      uint8_t num_woodpecker_mechanisms = 0;
      uint8_t num_neos = 0;

      bool bells_active = false;
      bool woodpeckers_active = false;
      bool playback_active = false;
      Rhythm * rhythm;
      elapsedMillis last_onset;

      uint8_t red = 200;
      uint8_t green = 100;
      uint8_t blue = 255;
};

PlaybackEngine::PlaybackEngine(){

}

void PlaybackEngine::linkNeoGroup(NeoGroup *n) {
    neos[num_neos] = n;
    num_neos++;
}

void PlaybackEngine::linkBellMechanism(BellMechanism * m) {
    bell_mechanisms[num_bell_mechanisms] = m;
    num_bell_mechanisms++;
    bells_active = true;
}

void PlaybackEngine::linkPeckerMechanism(Woodpecker * m) {
    woodpeckers[num_woodpecker_mechanisms] = m;
    num_woodpecker_mechanisms++;
    woodpeckers_active = true;
}

void PlaybackEngine::playRhythm(Rhythm * r) {
    if (playback_active == false) {
        playback_active = true;
        rhythm = r;
        rhythm->setCurrentNote(0);
    } else {
        Serial.println("ERROR, PLAYBACK ENGINE IS ALREADY IN PLAYBACK MODE");
    }
}

void PlaybackEngine::updateBells() {
    double target_f = rhythm->getFreq();
    double distance = 20000.0;
    uint8_t idx = 0;
    for (int i = 0; i < 3; i++) {
        if (abs(bell_mechanisms[i]->getFreq() - target_f) < distance) {
            distance = abs(bell_mechanisms[i]->getFreq() - target_f);
            idx = i;
        }
    }
    Serial.print("Chose mechanism ");Serial.print(idx);
    Serial.print(" as it had the closest freq to ");
    Serial.print(target_f);
    Serial.print(" of ");
    Serial.println(bell_mechanisms[idx]->getFreq());

    bell_mechanisms[idx]->queueStrike(rhythm->getVelocity(), rhythm->getLength());
    bell_mechanisms[idx]->strike();

    rhythm->nextNote();
    if (rhythm->isActive() == false){
        playback_active = false;
    }
    last_onset = 0;
}

void PlaybackEngine::updateWoodpeckers() {
    // TODO , need logic to determine which pecker to choose
    for (int i = 0; i < num_woodpecker_mechanisms; i++) {
        if (rhythm->getNoteType() == NOTE_TYPE_MOTOR) {
            Serial.println("CALLING rotate() on the woodpecker class");
            woodpeckers[i]->rotate(rhythm->getLength());
            rhythm->nextNote();
            last_onset = 0;
        }
        else {
            woodpeckers[i]->queueStrike(rhythm->getVelocity());
            if (woodpeckers[i]->strike()) {
                rhythm->nextNote();
                last_onset = 0;
            }
        }
    }
    if (rhythm->isActive() == false){
        playback_active = false;
    }
}

void PlaybackEngine::update() {
    for (int i = 0; i < num_bell_mechanisms; i++) {
        bell_mechanisms[i]->update();
    }
    for (int i = 0; i < num_woodpecker_mechanisms; i++) {
        woodpeckers[i]->update();
    }

    if (!playback_active) {
        return;
    }

    if (last_onset > rhythm->getCurrentOnset()) {
        for (int n = 0; n < num_neos; n++) {
            neos[n]->colorWipe(red, green, blue, 1.0);
        }
        if (bells_active) {
            updateBells();
        }
        if (woodpeckers_active) {
            updateWoodpeckers();
        }
        // TODO - need to get the information from the rhythm and set it up
    } else if (last_onset > 100) {
        for (int n = 0; n < num_neos; n++) {
            neos[n]->colorWipe(0, 0, 0, 1.0);
        }
    }
}
#endif // __RHYTHMS_H__
