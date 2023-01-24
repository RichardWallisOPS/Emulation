# NES Emulator for macOS

Thanks to https://www.nesdev.org/ and all the people who contributed to the information contained there.  All internal NES hardware info for this implementation taken from that source.  Again many thanks.  This implementation is my own take on that information.  The goal was to make it fairly accurate.  There are however some edge cases not accounted for. And there will be bugs!

This is a project I just wanted to do for fun - lots of people have already done this and probably better in many/most cases!  Feel free to use this code for personal use.  Not for commerical use.  People who did the reverse engineering hard work at nesdev deserve that more.

### Controls

(This may change so I really need to keep this section up to date!)

Supports the game controllers supported by macOS.  Controllers are assigned the same order as they are given in GCGameController.

W,S,A,D = [D-Pad]<br>
6,7     = [Select|Start]<br>
O,P     = [B|A]<br>
Return  = Swap keyboard controls Player1 <=> Player2<br>
&#8595; = save snap shot<br>
&#8593; = load snap shot<br>
&#8592; = rewind time (currently 5 seconds - saved snap shots do not save history)<br>
ESC     = Reset<br>
N       = Open file load dialogue (opens automatically on start if no file load from the command line)

### Goal

Decently accurate emulation, try to have most "Top 50" games working well.  But ignore stuff or games I don't care about.

### Random Thoughts

1) Rather than virtual functions for my bus implementation it would be nice to emulate the bus and cartridge pins as is directly and send clock pulses to each system to update what goes on the pins.  Better for mappers/open bus/external audio/etc.
2) Looking like NMI [and IRQ?] signals should be held back some amount of ticks.  Must be as the H/W take time to pull the lines low.  Look at moving any wait logic into the System::SignalXXX() functiions before signaling the CPU
3) FME7 (Mapper 69) is missing 5B audio - use the other version of the game.
4) Looking for MMC5 support? Doesn't exist - Mapper 24 (VRC6) is available though for that game and it supports the extra audio too!
5) EmulationController needs refactoring but I probably won't bother.

### Implementation Notes for 6502

1) Instruction cycle Tn states taken from original 6502 data sheet.
2) Registers, memory, address and data buses values try to follow the update rules for these states.
3) This means some Tn operations look superfluous but this is trying to be cpu register state cycle "accurate".
4) However some operations on the data bus that would be discared are not always done - may cause some incompatabilities - see bus emulation note above.
5) No instruction implements the T0 state as that is handled by the generic opCode fetch.
6) Decimal mode is not implemented - it and any illegal op-codes will error in Debug and 1 byte NOP in Release.
7) Most illegal opcodes are not implemented - if one is caught in Debug then its more likely a bug on my side.
