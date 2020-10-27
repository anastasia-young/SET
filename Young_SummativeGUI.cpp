/*****************************************************************************

 *	Name:     Mr. Creelman                                                   *

 *	Date:     August 2017                                                    *

 *                                                                           *

 *	Purpose: Example 3 - Show how to clip a bitmap (image)                   *

 *	                                                                         *

 *	Usage:   Shows two clipped bitmaps then exits					         *

 *	                                                                         *

 *	Revision History:                                                        *

 *	                                                                         *

 *	Known Issues:                                                            *

 *	                                                                         *

 *****************************************************************************/



#include <allegro5/allegro.h>

#include <allegro5/allegro_image.h>

#include <allegro5/allegro_native_dialog.h>

#include <string.h>

#include <stdio.h>

#include <stdlib.h>


const int SCREEN_W = 640;       // screen width

const int SCREEN_H = 480;       // screen height



int main(int argc, char *argv[]) {

	ALLEGRO_DISPLAY *display = nullptr;



	// Initialize Allegro

	al_init();



	// Initialize display

	display = al_create_display(SCREEN_W, SCREEN_H);

	if (!display) {

    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",

                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);

       	return -1;

	}

	al_set_window_title(display, "Allegro Example 3 - Clipping Bitmaps");



   	// need to add image processor

 	if (!al_init_image_addon()) {

    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize image addon!",

                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);

    	return 0;

	}

    // Create a bitmap for the rainbow and load appropriate image
	char filename[20] = "card";
	ALLEGRO_BITMAP *card[81];

	for(int i = 0; i<81; i++){
        sprintf(filename, "card%d.bmp",i);
        printf("%s\n", filename);
        card[i] = al_load_bitmap(filename); // Load our picture
	}

	// draw the new sub bitmap to a couple of locations on the display

	al_draw_bitmap(card[0] , 0, 0, 0);

	al_draw_bitmap(card[1] ,60, 0, 0);

	al_draw_bitmap(card[2] ,120, 0, 0);

	al_draw_bitmap(card[3] ,180, 0, 0);

	al_draw_bitmap(card[4] , 240, 0, 0);

	al_flip_display();	 // show what is on the display



	// Wait for 5 seconds. . .

	al_rest(5);



	//Release the bitmap data and exit with no erros
    for (int i = 0; i<4; i++){
        al_destroy_bitmap(card[i]);
    }


	 // al_destroy_bitmap(clippedRainbow); not needed since clipped image share memory

	al_destroy_display(display);

	return 0;

}
