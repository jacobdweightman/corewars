// @author Jacob Weightman
// Based off of code by Dewdney.
// takes 42 cycles to duplicate and jump forward.
// If it jumps forward more than that, it might beat imp.

MOV #0 59           // duplicate S
MOV #50 60          // duplicate T
MOV @6 @7           // T = S        MOV @S @T
ADD #1 5            // S++          ADD #1 S
ADD #1 5            // T++          ADD #1 T
CMP 3 #8            // if(S == 8)   CMP S #8
JMP 46              // GOTO next    JMP @T
JMP -5              // GOTO line 3
DAT #0              // S = 0
DAT #50             // T = 50


// 2 + 5*8 + 4
