# NES Emulator for macOS

Thanks to https://www.nesdev.org/ and all the people who contributed to the information contained there.  All internal NES hardware info for this implementation taken from that source.  Again many thanks.  This implementation is my own take on that information.  The goal was to make it fairly accurate.  There are however some edge cases not accounted for. And there will be bugs!

PUBLIC DOMAIN CRT STYLED SCAN-LINE SHADER, Original by Timothy Lottes published on ShaderToy - ported to Metal and only very slightly modified here. (To remove the shader CRT effect change "fragCopy_Monitor" to "fragCopy" in the Metal library function creation).

This is a project I just wanted to do for fun - lots of people have already done this and probably better.  It is completely unofficial.  You are free to use this code for personal use only.  You may not redistribute, publish, sublicense, sell, rent or transfer.  Not for commerical use.  Use of this code is at your own risk.

### Controls

(This may change - so I really need to keep this section up to date!)

Supports the game controllers supported by macOS.  Controllers are assigned the same order as they are given in GCGameController.

NES Pad keyboard controls:<br>
W,S,A,D = D-Pad<br>
6,7     = Select | Start<br>
O,P     = B | A<br>

Emulation controls:<br>
Return  = Swap keyboard controls above between Player1 <=> Player2<br>
&#8595; = save snap shot<br>
&#8593; = load snap shot<br>
&#8592; = rewind time (currently 5 seconds - saved snap shots do not save rewind history)<br>
ESC     = Reset console<br>
N       = Open file load dialogue (opens automatically on start if no file load from the command line)

### Goal

Decently accurate emulation, try to have most "Top 50" games working well.  But ignore stuff or games I don't care about. Please note: At a minimum - you must physically own any games used and make that backup you intend to use yourself.

### Random Thoughts

1) Rather than virtual functions for my bus implementation it would be nice to emulate the bus and cartridge pins as is directly and send clock pulses to each system to update what goes on the pins.  Better for mappers/open bus/external audio/etc.
2) Looking like NMI (and maybe IRQ?) signals should be held back some amount of ticks.  Must be as the H/W take time to pull the lines low.  Look at moving any wait logic into the System::SignalXXX() functiions before signaling the CPU
3) FME-7 (Mapper 69) is missing 5B audio - use the other version of the game.
4) Looking for MMC5 support? Doesn't exist - Mapper 24 (VRC6) is available though for that game and it supports the extra audio too!
5) EmulationController needs refactoring but I probably won't bother - it's not the interesting part.
6) Wrapper App is very bare bones.  It was the emulation core that interested me.

### Implementation Notes for 6502

1) Instruction cycle Tn states taken from original 6502 data sheet.
2) Registers, memory, address and data buses values try to follow the update rules for these states.
3) This means some Tn operations look superfluous but this is trying to be cpu register state cycle "accurate".
4) However some operations on the data bus that would be discared are not always done - may cause some incompatabilities - see bus emulation note above.
5) No instruction implements the T0 state as that is handled by the generic opCode fetch.
6) Decimal mode is not implemented - but it isn't on the NES CPU either.  Set and clear decimal mode flag still work but do nothing.
7) Most illegal opcodes are not implemented - if one is caught in Debug then its more likely a bug on my side - release will (one byte 2 cycle) NO-OP.
