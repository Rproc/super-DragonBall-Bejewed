void apply_surface( int x, int y, SDL_Surface* message, SDL_Surface* destination ) 
{ 
	SDL_Rect* rect_message = NULL; 

	//Holds offsets 
	SDL_Rect offset;
 
	//Get offsets 
	offset.x = x; 
	offset.y = y; 

	//Blit 
	SDL_BlitSurface( message, rect_message, destination, &offset ); 
}

void BlitaTempo ()
{
	int time,start;
	char segundos[4];

	start=100;
	time =-1*(SDL_GetTicks()/1000-start);
  	itoa(time,segundos);
	TTF_Init();

	//BlitaPontos();

	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);

	//SDL_Rect textLocation = { 100, 50, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* message = NULL;
	message = TTF_RenderText_Solid(font, "Tempo:", foregroundColor);
	apply_surface(100, 50, message, screen);

	SDL_Rect PontLocation = { 200, 50, 0, 0 };
	SDL_Color PontColor = { 0, 0, 0 };
	message = TTF_RenderText_Solid(font, segundos, PontColor);
	apply_surface(200,50, message, screen);

}
