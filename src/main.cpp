/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <ctime>

//Screen dimension constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
const int GIRLS_COUNT = 50;
const int SEC_IN_YEAR = 500000;
//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot(int, int, int, int);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the dot on the screen
		void render();

		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		float mVelX, mVelY;

        LTexture gDotTexture;

        bool loadMedia(std::string path);

        void close();

        int age;

        bool married = 0;
};

class girl : public Dot{
public:
    girl(int x, int y, int VelX, int VelY):Dot(x, y, VelX, VelY){
    };

    void velCalcX(){
        int a = rand() % 200 - 100;
        float b = a/100.0;
        mVelX += b;
    }
    void velCalcY(){
        int a = rand() % 200 - 100;
        float b = a/100.0;
        mVelY += b;
    }
    void move();
    int avergeMarriageAge = 24;
    int birthRate = 10;
    bool hasChild = 0;
    void ChangeIfMarried(){
        int diff = age - avergeMarriageAge;
        int probability = rand() % 20;
        if((diff == 1)||(diff == -1)){
            if (probability < 3){
                if(hasChild == true)
                    gDotTexture.loadFromFile("img/family.png");
                else
                    gDotTexture.loadFromFile("img/married.png");
                married = true;
                }
            }
        else if(diff == 0){
            if(probability < 5){
                if(hasChild == true)
                    gDotTexture.loadFromFile("img/family.png");
                else
                    gDotTexture.loadFromFile("img/married.png");
                married = true;
                }
            }
        else {
            if (probability == 1){
                if(hasChild == true)
                    gDotTexture.loadFromFile("img/family.png");
                else
                    gDotTexture.loadFromFile("img/married.png");
                married = true;
                }
        }
    }

    void ChangeIfHasChild(){
        int diff = age - avergeMarriageAge;
        int probability = rand() % 100;
        if (probability < 10){
            if(married == true)
                gDotTexture.loadFromFile("img/family.png");
            else
                gDotTexture.loadFromFile("img/withbaby.png");
            hasChild = true;
        }
    }
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
//LTexture gDotTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = 0;//clip->w;
		renderQuad.h = 0;//clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


Dot::Dot(int x, int y, int VelX, int VelY)
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Initialize the velocity
    mVelX = VelX;
    mVelY = VelY;
    age = 18;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move()
{
    //Move the dot left or right
    mPosX += mVelX;

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the dot up or down
    mPosY += mVelY;

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
}

void girl::move()
{
    //Move the dot left or right
    mPosX += mVelX;

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        mVelX = -0.8 * mVelX;
    }

    //Move the dot up or down
    mPosY += mVelY;

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        mVelY = -0.8 * mVelY;
    }
}

void Dot::render()
{
    //Show the dot
	gDotTexture.render( mPosX, mPosY );
}

bool init()
{
	//Initialization flag
	bool success = true;
    SDL_DisplayMode current;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
        int should_be_zero = SDL_GetCurrentDisplayMode(0, &current);
        if(should_be_zero != 0){
            printf("Could not get display mode for video display #%d: %s", 0, SDL_GetError());
        }
        else{
            SCREEN_HEIGHT = current.h - 50;
            SCREEN_WIDTH = current.w;
            //Create window
            gWindow = SDL_CreateWindow( "Get a woman or die trying", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED - 50, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
            if( gWindow == NULL )
            {
                printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Create vsynced renderer for window
                gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
                if( gRenderer == NULL )
                {
                    printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                    success = false;
                }
                else
                {
                    //Initialize renderer color
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                    //Initialize PNG loading
                    int imgFlags = IMG_INIT_PNG;
                    if( !( IMG_Init( imgFlags ) & imgFlags ) )
                    {
                        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                        success = false;
                    }
                }
            }
        }
    }
    int num_joysticks = SDL_NumJoysticks();
    int i;
    for(i = 0; i < num_joysticks; ++i)
    {
      SDL_Joystick* js = SDL_JoystickOpen(i);
      printf( "Joystick defined! %i\n", i );
    }
	return success;
}

bool Dot::loadMedia(std::string path)
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile( path.c_str() ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void Dot::close()
{
	//Free loaded images
	gDotTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
    std::clock_t start = std::clock();
    std::clock_t startYear = std::clock();
    Dot dot(10, 10, 0, 0);
    girl* women[GIRLS_COUNT];
    for(int i = 0; i < GIRLS_COUNT; i++){
        women[i] = new girl(i*10, i*10, 0, 0);
    }
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		std::string pathToGirls;
		if( !dot.loadMedia("img/man.png") )
		{
			printf( "Failed to load media!\n" );
		}
		else
		for(int i = 0; i < GIRLS_COUNT; i++){
            if(i < 16) pathToGirls = "img/girl1.png";
            else if(i < 32) pathToGirls = "img/girl2.png";
            else pathToGirls = "img/girl3.png";
            if( !women[i]->loadMedia(pathToGirls) )
            {
                printf( "Failed to load media!\n" );
            }
        }
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
				}

				//Move the dot

				dot.move();
				for(int i = 0; i < GIRLS_COUNT; i++){
                   women[i]->move();
                }
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render objects
				dot.render();
				for(int i = 0; i < GIRLS_COUNT; i++){
                   women[i]->render();
                }
				if(std::clock() - start > 20000){
                    for(int i = 0; i < GIRLS_COUNT; i++){
                        women[i]->velCalcX();
                        }
                    }
				if(std::clock() - start > 80000){
                    start = std::clock();
                    for(int i = 0; i < GIRLS_COUNT; i++){
                        women[i]->velCalcY();
                        }
                    }
                if(std::clock() - startYear > SEC_IN_YEAR){
                    for(int i = 0; i < GIRLS_COUNT; i++){
                        if(!women[i]->married)
                            women[i]->ChangeIfMarried();
                        if(!women[i]->hasChild)
                            women[i]->ChangeIfHasChild();
                        women[i]->age += 1;
                        }
                    startYear = clock();
                    printf("year passed!,%i \n", women[1]->age);
                    }

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}
	//Free resources and close SDL
	dot.close();
	for(int i = 0; i < GIRLS_COUNT; i++){
        women[i]->close();
    }
	return 0;
}
