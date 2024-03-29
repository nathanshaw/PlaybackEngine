#ifndef __PLAYBACK_ENGINE_H__
#define __PLAYBACK_ENGINE_H__

#include "Rhythm.h"
#include <Mechanisms.h>
#include "../NeopixelManager/NeopixelManager.h"

class PlaybackEngine {
    public:
      PlaybackEngine();
      void playRhythm(Rhythm * r);
      void update();
      void linkMechanism(BellMechanism * m);
      void linkMechanism(WoodpeckerMechanism * m);
      void linkNeoGroup(NeoGroup * n);
      bool isActive(){return playback_active;};
      // void addSolenoidMechanism(uint8_t act, uint8_t damp);
      void setFeedbackColor(uint8_t r, uint8_t g, uint8_t b);
      
    private:
      void updateBells();
      void updateWoodpeckers();

      BellMechanism * bell_mechanisms[3];
      WoodpeckerMechanism * woodpecker_mechanisms[3];
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

void PlaybackEngine::setFeedbackColor(uint8_t r, uint8_t g, uint8_t b) {
    red = r;
    green = g;
    blue = b;
}

void PlaybackEngine::linkNeoGroup(NeoGroup *n) {
    neos[num_neos] = n;
    num_neos++;
}

void PlaybackEngine::linkMechanism(BellMechanism * m) {
    bell_mechanisms[num_bell_mechanisms] = m;
    num_bell_mechanisms++;
    bells_active = true;
}

void PlaybackEngine::linkMechanism(WoodpeckerMechanism * m) {
    woodpecker_mechanisms[num_woodpecker_mechanisms] = m;
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
    // TODO , need logic to determine which pecking_mechanism to choose
    for (int i = 0; i < num_woodpecker_mechanisms; i++) {
        if (rhythm->getNoteType() == NOTE_TYPE_MOTOR) {
            Serial.println("CALLING rotate() on the woodpecker class");
            woodpecker_mechanisms[i]->rotate(rhythm->getLength());
            rhythm->nextNote();
            last_onset = 0;
        }
        else {
            woodpecker_mechanisms[i]->queueStrike(rhythm->getVelocity());
            if (woodpecker_mechanisms[i]->strike()) {
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
        woodpecker_mechanisms[i]->update();
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
