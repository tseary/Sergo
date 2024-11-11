# Sergo
Sergo is a custom circuit board I made to interface hobby servos (or Geek servos) with Lego Power Functions. It has an ATTINY84 microcontroller and a 5 V, 2.5 A buck converter. It supports up to two servos, and can be controlled either from a PF receiver or directly by IR remote. It is programmed in Arduino.

![54124418031_3b0a96d11d_c](https://github.com/user-attachments/assets/e3827d7d-40d6-448e-872e-add0d20d352b)

# Make Your Own
Disclaimer: This is not a how-to guide. I have shared this project for general interest and in case the source code is useful to anyone. However, if you want to make your own Sergo, I am assuming you already have some knowledge of surface-mount soldering. You will need reflow soldering materials and equipment. **This project _cannot_ be assembled with only a soldering iron!**

## Gather Parts
The bare PCB without components can be ordered from [OSH Park](https://oshpark.com/shared_projects/rSyxp330).

<img src="https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/15dd6a34eb8136264769d7c58d72d672.png" width="200" />

Here is a [Digikey cart](https://www.digikey.ca/short/tn3525p4) with all of the components. The cost of parts for one board is about $15.61 CAD (or $13.92 CAD if the IR receiver is omitted).

It is recommended to apply solder paste to the board with a stencil. I used a polyimide film stencil of 0.005" thickness from [OSH Stencils](https://www.oshstencils.com). The stencil gerber file is "KiCad/gerbers/Sergo-F_Paste.gbr". Alternatively, solder paste may be applied freehand with a syringe. Particular care should be taken to apply the paste neatly under the buck converter PS1.

## Assembly, Programming, and Testing
More details to come...

# Show Me!
If you do attempt to build this project, I'd like to see it! Find me on [Eurobricks](https://www.eurobricks.com/forum/index.php?/profile/207750-tseary/).
