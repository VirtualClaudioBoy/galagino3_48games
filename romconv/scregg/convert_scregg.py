#!/usr/bin/env python3
"""Convert Scrambled Egg ROMs to the headers used by the local emulator."""
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def read_rom(folder, name, size):
    data = (folder / name).read_bytes()
    if len(data) != size:
        raise ValueError(f"Unexpected size for {name}: {len(data)}, expected {size}")
    return data

def write_array(path, name, data, dimensions):
    def initializer(values, dims):
        if len(dims) == 1:
            return '{' + ','.join(str(v) for v in values) + '}'
        stride = len(values) // dims[0]
        return '{' + ','.join(initializer(values[i:i + stride], dims[1:])
                              for i in range(0, len(values), stride)) + '}'
    suffix = ''.join(f'[{n}]' for n in dimensions[1:])
    stride = len(data) // dimensions[0]
    with path.open('w', encoding='utf-8', newline='\n') as output:
        output.write('// Generated from Scrambled Egg ROMs.\n')
        output.write(f'const unsigned char {name}[]{suffix} = {{\n')
        for i in range(0, len(data), stride):
            if len(dimensions) == 1:
                output.write('  ' + str(data[i]) + ',\n')
            else:
                output.write('  ' + initializer(data[i:i + stride], dimensions[1:]) + ',\n')
        output.write('};\n')

def decode(planes, count, width, xoffsets, yoffsets, increment):
    pixels = []
    for code in range(count):
        for y in range(width):
            for x in range(width):
                # Rotate decoded tiles/sprites to the existing portrait layout.
                sx, sy = y, width - 1 - x
                bit = code * increment + xoffsets[sx] + yoffsets[sy]
                pixels.append(sum(((plane[bit // 8] >> (7 - bit % 8)) & 1) << p
                                  for p, plane in enumerate(planes)))
    return pixels

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom-dir', type=Path, default=ROOT / 'romszip' / 'scregg_unpack')
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'source' / 'src' / 'machines' / 'scregg')
    args = parser.parse_args()
    program = b''.join(read_rom(args.rom_dir, f'scregg.{c}14', 4096) for c in 'edcba')
    planes = [read_rom(args.rom_dir, f'scregg.{c}12', 4096) +
              read_rom(args.rom_dir, f'scregg.{c}10', 4096) for c in 'jhg']
    colors = read_rom(args.rom_dir, 'screggco.c6', 32)
    chars = decode(planes, 1024, 8, list(range(8)), [y * 8 for y in range(8)], 64)
    sprites = decode(planes, 256, 16, list(range(128, 136)) + list(range(8)),
                     [y * 8 for y in range(16)], 256)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    write_array(args.output_dir / 'scregg_rom.h', 'scregg_rom', program, [len(program)])
    write_array(args.output_dir / 'scregg_chartiles.h', 'scregg_chartiles', chars, [1024, 8, 8])
    write_array(args.output_dir / 'scregg_spritetiles.h', 'scregg_spritetiles', sprites, [256, 16, 16])
    write_array(args.output_dir / 'scregg_colorprom.h', 'scregg_colorprom', colors, [32])
    print('Generated Scrambled Egg ROM, tiles, sprites and colour PROM headers.')

if __name__ == '__main__':
    main()