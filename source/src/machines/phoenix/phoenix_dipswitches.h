#ifndef PHOENIX_DIPSWITCHES_H
#define PHOENIX_DIPSWITCHES_H
// DSW0 (MAME phoenix, verificato su driver):
//   bit 0-1 = lives   (0=3, 1=4, 2=5, 3=6)          -> 0x00 (3 vite)
//   bit 2-3 = bonus   (0=3K/30K, 1=4K/40K, ...)     -> 0x00
//   bit 4   = coinage (0=1C/1C, 1=2C/1C)            -> 0x00 (1 coin 1 credito)
//   bit 5   = unknown (default Off = 1)             -> 0x20
//   bit 6   = unknown (default Off = 1)             -> 0x40
//   bit 7   = VBLANK live (gestito dinamicamente nel rdZ80)
// Totale bit 0-6 = 0x60 (default cabinet MAME).
#define PHOENIX_DSW0 0x60
#endif
