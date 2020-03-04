#ifndef __RHYTHMS_H__
#define __RHYTHMS_H__

#ifndef RHYTHM_MAX_NOTES
#define RHYTHM_MAX_NOTES 32
#endif

#ifndef MAX_RHYTHMS
#define MAX_RHYTHMS 10
#endif

class PlaybackEngine {
    PlaybackEngine();
    public:
      void playRhythm(Rhythm & r);
      void update();
      // void addSolenoidMechanism(uint8_t act, uint8_t damp);
    private:
      bool playback_active = false;
      Rhythm * rhythm;
      elapsedMillis last_onset;
}


void PlaybackEngine::playRhythm(Rhythm & r) {
    if (playback_active == false) {
        playback_active = true;
        rhythm = r;
    } else {
        Serial.println("ERROR, PLAYBACK ENGINE IS ALREADY IN PLAYBACK MODE");
    }
}

void PlaybackEngine::update() {
    if (!playback_active) {
        return;
    }
    if (last_onset > rhythm.getCurrentOnset()) {
        // TODO - need to get the information from the rhythm and set it up
        rhythm.nextNote();
        if (rhythm.isActive() == false){
            playback_active = false;
        }
    }

}

class RhythmBank {
    RhythmBank();
    public:
       void addRhythm(Rhythm & r);
       // get the index of the rhythm which contains a medium freq closest to the given freq
       uint8_t getRhythmByPitch(double freq);
    private:
       Rhythm * rhythms[MAX_RHYTHMS];
       uint8_t num_rhythms = 0;
       uint8_t last_played;
};

RhythmBank::addRhythm(Rhythm & r) {
    rhythms[num_rhythms] = r;
    num_rhythms++;
}

uint8_t RhythmBank::getRhythmByPitch(double freq) {
    uint8_t closest_idx;
    double min_distance = 20000.0;
    for (int i = 0; i < num_rhythms; i++) {
        double distance = Math.abs(rhythms[i]->getMedianFreq() - freq);
        if (distance < min_distance) {
            min_distance = distance;
            closest_idx = i;
        }
    }
    return closest_idx;
}

class Rhythm {
    Rhythm();

    public:
        void addNote(double freq, double length, double onset, double velocity);

        void nextNote();

        double getMedianFreq(){return median_freq;};

        double getCurrentOnset() {return onset_times[current_note]};

        bool isActive() {return active;};

    private:
        // number of notes stored in the rhythm
        uint8_t num_notes = 0;
        // approx freq of notes
        double freqs[RHYTHM_MAX_NOTES];
        // how long should the notes ring for (think of the bells)
        uint32_t lengths[RHYTHM_MAX_NOTES]
        // when are the note onsets in relation to the start of the first note
        // index 0 of this array should always be 0
        uint32_t onset_times[RHYTHM_MAX_NOTES];
        // should be scaled from 0 - 1
        double velocitys[RHYTHM_MAX_NOTES];
        // is the class currently active?
        bool active = false;
        // which note is currently sounding?
        bool current_note = 0;
        double median_freq = 0.0;
};


Rhythm::Rhythm() {

}


void Rhythm::nextNote() {
    current_note++;
    active = true;
    if (current_note > num_notes) {
        current_note = 0;
        active = false;
    }
}


Rhythm::addNote(double freq, double length, double onset, double velocity) {
    freqs[num_notes] = freq;
    lengths[num_notes] = length;
    onset_times[num_notes] = onset;
    velocitys[num_notes] = velocity;
    num_notes++;
    double freq_t = 0.0;
    for (int i = 0; i < num_notes; i++) {
        freq_t += freqs[i];
    }
    median_freq = freq_t / num_notes;
}


#endif // __RHYTHMS_H__
