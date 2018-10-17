ChronoCape Rev. A

PCB DECRIPTION: 3.4in x 2.15in  (BBB cape shape, see outline gerber)
                4 LAYER PCB (.062 inches thickness) FR4

                Smallest line width and spacing 6/6 mils
                Smallest standard drill hole 13 mils
                Components/Silkscreen on top side
                Double side soldermask
                
PCB STACKUP:

    Stack Layer     Gerber Polarity Comment	
    -----------     --------------- -------
    1 (Top)         Positive        Component side (signal)
    2 (Inner 1)     Positive        Power (GND)
    3 (Inner 2)     Positive        Power
    4 (Bottom)      Positive        Solder side (signal)



FILES:

ChronoCape_RevA.topsilkscreen.ger - Top Silk Screen (component side)
ChronoCape_RevA.topsoldermask.ger - Top Solder stop mask (component side)
ChronoCape_RevA.toplayer.ger  - Top Copper (component side), Positive Polarity
ChronoCape_RevA.internalplane1.ger  - Inner Layer 1, Positive Polarity
ChronoCape_RevA.internalplane2.ger  - Inner Layer 2, Positive Polarity
ChronoCape_RevA.bottomlayer.ger  - Bottom Copper (solder side), Positive Polarity
ChronoCape_RevA.bottomsoldermask.ger - Bottom Solder stop mask (solder side)
ChronoCape_RevA.boardoutline.ger - Board outline

ChronoCape_RevA.drills.xln - NC Drill : 2.4, Abs, inches, leading zero supress, quad
ChronoCape_RevA.drills.dri - NC Drill info

ChronoCape_RevA.tcream.ger  - Top Side solder paste

ChronoCape_RevA_centroid.csv - Centroid / Pick & Place / XY data

QUESTIONS?
  Please contact Peter Volgyesi <peter.volgyesi@vanderbilt.edu>
  at 615.294.6520 with any problems or questions
