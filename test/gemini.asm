MOV #0 39
MOV #30 40
MOV @6 @7           // T = S        MOV @S @T
ADD #1 5            // S++          ADD #1 S
ADD #1 5            // T++          ADD #1 T
CMP 3 #8            // if(S == 8)   CMP S #8
JMP 26              // GOTO next    JMP @T
JMP -5              // GOTO line 3
DAT #0              // S = 0
DAT #30             // T = 100


// 2 + 5*6 + 4
