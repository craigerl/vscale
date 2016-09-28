# vscale

Alsa client which scales up midi note velocity events


   This program creates a new alsa midi client called "VScale"
   which scales midi note velocities to the full midi spectrum from
   zero to 127.  Many midi controllers max out at 110 or so,
   unless you use a bullet or sledge hammer on the keys.  If
   your midi controler seems quiet, or fortisimo strikes sound
   dark, use this program to upscale key velocities.

Usage:
   vscale value

   where value is the typical maximum output your controler
   provides from 0 to 127.

Example:

   vscale 110

   aconnect -x ; aconnect 24 129; aconnect 129 20

   This will multiply your input velocity by 1.154545,
   connect alsa input (24) to vscale (129), then connect
   vscale (129) to alsa output (20).   Use "aconnect -i"
   to see which midi devices map to what numbers.

License:
   Craig Lamparter 2016  General Public License

   code and inspiration used in part from organ_filter,
                    Hans Fugal <hans@fugal.net>  2004

