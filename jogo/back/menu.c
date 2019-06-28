while(running)
	{	
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			 running=false;
			if( event.type == SDL_MOUSEMOTION )
			{
				mousex = event.motion.x;
				mousey = event.motion.y;
				if( (mousex > textLocation.x) && (mousex < (textLocation.x + textLocation.w)) && (mousey > textLocation.y) && (mousey < (textLocation.y + textLocation.h)) )
				{
					SDL_Color foregroundColor = { 0, 255, 255 };
					textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 255, 0, 0 };
					textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);
				}
				if( (mousex > textLocation2.x) && (mousex < (textLocation2.x + textLocation2.w)) && (mousey > textLocation2.y) && (mousey < (textLocation2.y + textLocation2.h)) )
				{
					SDL_Color foregroundColor = { 0, 255, 255 };
					textSurface2 = TTF_RenderText_Solid(font, "Options", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 255, 0, 0 };
					textSurface2 = TTF_RenderText_Solid(font, "Options", foregroundColor);
				}
			}

			if( event.type == SDL_MOUSEBUTTONDOWN )
		  {
				mousex = event.button.x;
				mousey = event.button.y;

			if( (mousex > textLocation.x) && (mousex < (textLocation.x + textLocation.w)) && (mousey > textLocation.y) && (mousey < (textLocation.y + textLocation.h)) )
				{
					running=false;
				}

			}
		}
