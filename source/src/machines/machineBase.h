#ifndef MACHINEBASE_H
#define MACHINEBASE_H

#include "Arduino.h"
#include "../cpus/z80/Z80.h"
#include "../cpus/i8048/i8048.h"
#include "../emulation/input.h"

#ifdef LED_PIN
#include <FastLED.h>
#define NUM_LEDS  7

#define LED_BLACK    CRGB::Black
#define LED_RED      CRGB::Red
#define LED_GREEN    CRGB::Green
#define LED_BLUE     CRGB::Blue
#define LED_YELLOW   CRGB::Yellow
#define LED_MAGENTA  CRGB::Magenta
#define LED_CYAN     CRGB::Cyan
#define LED_WHITE    CRGB::White
#endif

#if defined(ENABLE_XEVIOUS) || defined(ENABLE_MAPPY) || defined(ENABLE_PBACTION) || defined(ENABLE_STARFORCE) || defined(ENABLE_TODRUAGA) || defined(ENABLE_ROCNROPE)
  #define RAMSIZE   (16384)
#elif (defined(ENABLE_1942) || defined(ENABLE_MRDO) || defined(ENABLE_LADYBUG) || defined(ENABLE_GYRUSS) || defined(ENABLE_TIMEPLT) || defined(ENABLE_PENGO))
  #define RAMSIZE   (8192 + 1024 + 128)
#else
  #define RAMSIZE   (8192)
#endif

#ifdef ENABLE_STARFORCE
#define SN_CHIPS 3
#else
#define SN_CHIPS 2
#endif

struct sprite_S {
  unsigned char code, color, flags;
  short x, y;

  //bombjack
  unsigned char color_block; // Già shiftato di 3 per essere usato come offset
  char is_32x32;
  char flip_x;
  char flip_y;
};

enum {
  MCH_MENU = 0,
  #ifdef ENABLE_1942  
    MCH_1942,
  #endif 	
  #ifdef ENABLE_ALIBABA
    MCH_ALIBABA,
  #endif
  #ifdef ENABLE_AMIDAR
    MCH_AMIDAR,
  #endif
  #ifdef ENABLE_ANTEATER  
    MCH_ANTEATER,
  #endif 	
  #ifdef ENABLE_BAGMAN  
    MCH_BAGMAN,
  #endif 
  #ifdef ENABLE_BOMBJACK  
    MCH_BOMBJACK,
  #endif 	
  #ifdef ENABLE_BNJ
    MCH_BNJ,
  #endif
  #ifdef ENABLE_BTIME
    MCH_BTIME,
  #endif
  #ifdef ENABLE_CIRCUSC
    MCH_CIRCUSC,
  #endif
  #ifdef ENABLE_CRUSH  
    MCH_CRUSH,
  #endif 	
  #ifdef ENABLE_DKONG  
    MCH_DKONG,
  #endif 	
  #ifdef ENABLE_DKONG3
    MCH_DKONG3,
  #endif
  #ifdef ENABLE_DKONGJR
    MCH_DKONGJR,
  #endif
  #ifdef ENABLE_DIGDUG  
    MCH_DIGDUG,
  #endif 	
  #ifdef ENABLE_EYES  
    MCH_EYES,
  #endif
  #ifdef ENABLE_FANTASY
    MCH_FANTASY,
  #endif
  #ifdef ENABLE_FROGGER  
    MCH_FROGGER,
  #endif 	
  #ifdef ENABLE_GALAGA  
    MCH_GALAGA,
  #endif 	
  #ifdef ENABLE_GALAXIAN  
    MCH_GALAXIAN,
  #endif 	
  #ifdef ENABLE_GAPLUS
    MCH_GAPLUS,
  #endif
  #ifdef ENABLE_GYRUSS  
    MCH_GYRUSS,
  #endif 	
  #ifdef ENABLE_SPACE  
    MCH_SPACE,
  #endif
  #ifdef ENABLE_LADYBUG  
    MCH_LADYBUG,
  #endif 	
  #ifdef ENABLE_LIZARD
    MCH_LIZARD,
  #endif
  #ifdef ENABLE_MAPPY
    MCH_MAPPY,
  #endif
  #ifdef ENABLE_MOONCRESTA
    MCH_MOONCRESTA,
  #endif
  #ifdef ENABLE_MRDO  
    MCH_MRDO,
  #endif 	
  #ifdef ENABLE_MRTNT
    MCH_MRTNT,
  #endif
  #ifdef ENABLE_MSPACMAN  
    MCH_MSPACMAN,
  #endif 	
  #ifdef ENABLE_NIBBLER
    MCH_NIBBLER,
  #endif
  #ifdef ENABLE_PACMAN  
    MCH_PACMAN,
  #endif 	
  #ifdef ENABLE_PENGO  
    MCH_PENGO,
  #endif 	
  #ifdef ENABLE_PHOENIX
    MCH_PHOENIX,
  #endif
  #ifdef ENABLE_PBACTION
    MCH_PBACTION,
  #endif
  #ifdef ENABLE_POOYAN
    MCH_POOYAN,
  #endif
  #ifdef ENABLE_ROCNROPE
    MCH_ROCNROPE,
  #endif
  #ifdef ENABLE_SCRAMBLE
    MCH_SCRAMBLE,
  #endif
  #ifdef ENABLE_SCREGG
    MCH_SCREGG,
  #endif
  #ifdef ENABLE_STARFORCE
    MCH_STARFORCE,
  #endif
  #ifdef ENABLE_SUPERCOBRA
    MCH_SUPERCOBRA,
  #endif
  #ifdef ENABLE_THEGLOB
    MCH_THEGLOB,
  #endif
  #ifdef ENABLE_TODRUAGA
    MCH_TODRUAGA,
  #endif
  #ifdef ENABLE_TIMEPLT  
    MCH_TIMEPLT,
  #endif 	
  #ifdef ENABLE_TURTLES
    MCH_TURTLES,
  #endif
  #ifdef ENABLE_TUTANKHM
    MCH_TUTANKHM,
  #endif 
  #ifdef ENABLE_VANVAN
    MCH_VANVAN,
  #endif
  #ifdef ENABLE_VANGUARD
    MCH_VANGUARD,
  #endif
  #ifdef ENABLE_XEVIOUS
    MCH_XEVIOUS,
  #endif
};


// one inst at 3Mhz ~ 500k inst/sec = 500000/60 inst per frame
#define INST_PER_FRAME 300000/60/4 //=1250

#ifdef LED_PIN
  typedef const CRGB (*MenuLedType)[12][NUM_LEDS];
#endif

class machineBase
{
public:
    machineBase() { }
    virtual ~machineBase() { }

    virtual void init(Input *input, unsigned short *framebuffer, sprite_S *spritebuffer, unsigned char *memorybuffer) {
      this->input = input;
      this->frame_buffer = framebuffer; 
      this->sprite = spritebuffer;
      this->memory = memorybuffer;
      memset(soundregs, 0, sizeof(soundregs)); 
     }

    virtual void reset() {
      for(current_cpu = 0; current_cpu < sizeof(cpu) / sizeof(Z80); current_cpu++)
        ResetZ80(&cpu[current_cpu]);

      memset(memory, 0, RAMSIZE);
      memset(soundregs, 0, sizeof(soundregs)); 

      for (int chip = 0; chip < SN_CHIPS; chip++) {
        for (int c = 0; c < 4; c++) {
          sn_period[chip][c] = 0;
          sn_volume[chip][c] = 15; // Muto
          sn_min_volume[chip][c] = 15;
          sn_hold[chip][c] = 0;
        }
      }
      current_cpu = 0;
      game_started = 0;
    }

    virtual signed char machineType() { return MCH_MENU; } 
    virtual signed char videoFlipY() { return 0; }
    virtual signed char videoFlipX() { return 0; }
    virtual signed char useVideoHalfRate() { return 0; } 
    
    virtual unsigned char rdZ80(unsigned short Addr) { return 0xff; }
    virtual void wrZ80(unsigned short Addr, unsigned char Value) { };
    virtual void outZ80(unsigned short Port, unsigned char Value) { };
    virtual unsigned char opZ80(unsigned short Addr) { return 0x00; }
    virtual unsigned char inZ80(unsigned short Port) { return 0x00; }

    virtual void wrI8048_port(struct i8048_state_S *state, unsigned char port, unsigned char pos) { }
    virtual unsigned char rdI8048_port(struct i8048_state_S *state, unsigned char port) { return 0x00; };
    virtual unsigned char rdI8048_xdm(struct i8048_state_S *state, unsigned char addr)  { return 0x00; };
    virtual unsigned char rdI8048_rom(struct i8048_state_S *state, unsigned short addr) { return 0x00; };

    virtual void run_frame(void) { };
    virtual void prepare_frame(void) { };
    virtual void render_row(short row) { };
    
    virtual const signed char *waveRom(unsigned char value) { return 0; }
    virtual const unsigned short *logo(void) { return 0; };
    virtual unsigned char vanguardSoundRom(unsigned short addr) { return 0xff; }
    virtual bool vanguardMusic0Muted() { return true; }
    virtual void vanguardMusic0Ended() { }
    virtual bool vanguardMusic1Muted() { return true; }
    virtual bool vanguardMusic2Muted() { return true; }
    virtual const signed char *vanguardSample(unsigned char index) { return 0; }
    virtual unsigned long vanguardSampleLength(unsigned char index) { return 0; }
    virtual unsigned char vanguardSampleDivider(unsigned char index) { return 1; }
    virtual bool audioAttractMute() { return false; }
    virtual bool hasNamco15xxAudio() { return false; }
    virtual bool namcoSoundEnabled() { return true; }
    virtual bool hasNamcoAudio() { return false; }
#ifdef LED_PIN
    virtual void menuLeds(CRGB *leds) { memcpy(leds, menu_leds, NUM_LEDS*sizeof(CRGB)); };
    virtual void gameLeds(CRGB *leds) { memcpy(leds, menu_leds, NUM_LEDS*sizeof(CRGB)); };
#endif
    char game_started;	
    unsigned char soundregs[80];
    
    //Mr.Do!
    int sn_period[SN_CHIPS][4];    // 4 canali per chip (3 tono + 1 rumore)
    int sn_volume[SN_CHIPS][4];
    int sn_min_volume[SN_CHIPS][4]; // latched min volume per audio render cycle
    int sn_hold[SN_CHIPS][4];       // hold counter: keep sound active for N render cycles
protected:
    virtual void blit_tile(short row, char col) { }
    virtual void blit_sprite(short row, unsigned char s) { }
	
    Input *input;
    Z80 cpu[3];
    char irq_enable[3];
    char current_cpu;
    unsigned char irq_ptr;

    int active_sprites;
    sprite_S *sprite;
    unsigned short *frame_buffer;
    unsigned char *memory;

private:	
#ifdef LED_PIN
    const CRGB menu_leds[7] = { LED_BLACK, LED_BLACK, LED_BLACK, LED_BLACK, LED_BLACK, LED_BLACK, LED_BLACK };
#endif
};

#endif