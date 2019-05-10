/*  Example applying ADSR envelopes to 2 audio oscillators
     with Mozzi sonification library.

    This shows how to use an ADSR which updates at AUDIO_RATE,
    in updateAudio(), and output using next() at AUDIO_RATE in updateAudio().

    Another example in this folder shows an ADSR updating at CONTROL_RATE,
    which is more efficient, but AUDIO_RATE updates shown in this example
    enable faster envelope transitions.

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2013, CC by-nc-sa.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
//#include <ReverbTank.h>
#include <ADSR.h>
#include <tables/triangle2048_int8.h>
#include <tables/sin2048_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <ReverbTank.h>
Oscil <2048, AUDIO_RATE> aOscil0(TRIANGLE2048_DATA);
Oscil <2048, AUDIO_RATE> aOscil1(SIN2048_DATA);

// for triggering the envelope
EventDelay noteDelay;
EventDelay noteDelay2;

byte note_array[17] = {60-24, 62-24, 64-24, 67-24 , 69-24 , 72-24, 74-24, 76-24, 79-24, 81-24, 84-24, 86-24, 88-24, 91-24 , 93-24, 96-24} ;
const int nb_notes = 17 - 1;
// ADSR update_rate, interpolation_rte
ADSR <CONTROL_RATE,AUDIO_RATE> envelope0;
ADSR <CONTROL_RATE,AUDIO_RATE> envelope1;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  //Serial.begin(115200);
  randSeed(); // fresh random
  startMozzi();
  envelope0.setADLevels(200,128);
  envelope0.setTimes(400,600,400,3000);
  envelope1.setADLevels(200,128);
  envelope1.setTimes(400,600,400,3000);
  noteDelay.start(0);
  noteDelay2.start(1000);
}

void updateControl(){
  if(noteDelay.ready()){
    envelope0.noteOn();
    byte midi_note = note_array[rand(0,nb_notes)];
    int freq = (int)mtof(midi_note);
    aOscil0.setFreq(freq);
    //Serial.print(freq);
    noteDelay.start(3000);
  }
  if(noteDelay2.ready()){

    envelope1.noteOn();
    byte midi_note = note_array[rand(0,nb_notes)];
    int freq = (int)mtof(midi_note);
    aOscil1.setFreq(freq);
    //Serial.print(freq);
    noteDelay2.start(3000);
  }

  envelope0.update();
  envelope1.update();
}


int updateAudio(){
  long synth = ((long)envelope0.next() * aOscil0.next()) + ((int)envelope1.next() * aOscil1.next()) >> 8;
  return (synth);
}


void loop(){
  audioHook(); // required here
}
