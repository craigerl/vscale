/* vscale.c

   This program creates a new alsa midi client called "VScale"
   which scales midi note velocities to the full midi spectrum from
   zero to 127.  Many midi controllers max out at 110 or so, 
   unless you use a bullet or sledge hammer on the keys.  If    
   your midi controler seems quiet, or fortisimo strikes sound
   dark, use this program to upscale key velocities.

Usage:
   vscale <value>

   where <value> is the typical maximum output your controler 
   provides from 0 to 127.   

Example:

   vscale 110
   aconnect -x ; aconnect 24 129; aconnect 129 20

   This will multiply your input velocity by 1.154545,
   connect alsa input (24) to vscale (129), then connect
   vscale (129) to alsa output (20).   Use "aconnect -i"
   to see which midi devices map to what numbers.

License:
   Craig Lamparter 2016 

   code and inspiration used in part from organ_filter, 
                    Hans Fugal <hans@fugal.net>  2004
*/

#include <alsa/asoundlib.h>

void vscale(snd_seq_t *seq, int port, snd_seq_event_t *ev, int vel_offset)
{
float slope;
int new_vel;

slope = (float)127 / (127 - vel_offset);
printf("velocity multiplier: %f\n",slope);

    while (snd_seq_event_input(seq, &ev) >= 0)
    {
	if (ev->type == SND_SEQ_EVENT_NOTEON && ev->data.note.velocity != 0)
        {
            if (ev->data.note.velocity > 0 ) printf("%3d -> ", ev->data.note.velocity );
            new_vel = slope * ev->data.note.velocity;
            if ( new_vel > 127) ev->data.note.velocity =  127; 
            else ev->data.note.velocity = new_vel;
            if (ev->data.note.velocity > 0 ) printf("%3d\n", ev->data.note.velocity );
        }

	snd_seq_ev_set_source(ev, port);
	snd_seq_ev_set_subs(ev);
	snd_seq_ev_set_direct(ev);
	snd_seq_event_output(seq, ev);
	snd_seq_drain_output(seq);
    }
}

snd_seq_t *open_client()
{
    snd_seq_t *handle;
    int err;
    err = snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0);
    if (err <0)
    {
	fprintf(stderr, "Error opening sequencer\n");
	exit(1);
    }
    snd_seq_set_client_name(handle, "VScale");
    return handle;
}

int create_port(snd_seq_t *seq,char *name)
{
    int portid;
    if ((portid = snd_seq_create_simple_port(seq, name, 
		    SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE|
		    SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
		    SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
    {
	fprintf(stderr, "Error creating port %s\n", name);
	exit(1);
    }
    return portid;
}

int main(int argc, char *argv[])
{
    snd_seq_t *seq;
    snd_seq_event_t ev;
    int port;
    int vel_input_max;
    int vel_offset;
    
    if ( argc > 1 ) 
    { 
        vel_input_max = atoi(argv[1]);
        if (( vel_input_max  >= 0) && (vel_input_max <= 127 ))
        {
            vel_offset = 127 - vel_input_max;
            printf("vel_offset: %d\n", vel_offset);
        }
        else
        {
            printf("vscale <value>\n");
            printf("<value> is the typical maximum output your controler provides from 0 to 127\n");
            exit(1);
        }
    }
    else  
    {
         printf("vscale <value>\n");
         printf("<value> is the typical maximum output your controler provides from 0 to 127\n");
         exit(1);
    }
 
    seq = open_client();
    port = create_port(seq,"VScale");
    vscale(seq, port, &ev, vel_offset);
}
