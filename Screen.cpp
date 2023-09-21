#include "Screen.h"
namespace particle {
Screen::Screen() {
	m_window = NULL;
	m_gRenderer = NULL;
	m_gTexture = NULL;
	m_surface = NULL;
	m_pixelBuffer = NULL;
	m_pixelBufferBlurred = NULL;
};

bool Screen::initialize(Uint32 flags) {
	if (SDL_Init(flags) < 0 /* ou seja, igual a -1 */) {
		cout << "SDL failed to initialize: SDL error: " << flush;
		SDL_GetError();
		return false;
	}

	cout << "Initializaing application" << endl;
	// SDL_CreateWindow() retorna um ponteiro do tipo SDL_Window
	m_window = SDL_CreateWindow("Particle Explosion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	// checa se foi poss�vel criar a janela
	if (m_window == NULL) {
		cout << "Could not create window. SDL_error: " << SDL_GetError() << endl;
		return false;
	}

	// renderizador de pixeis na tela
	m_gRenderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (m_gRenderer == NULL) {
		cout << "Failed to create renderer. SDL_error: " << SDL_GetError() << endl;
		return false;
	}
	//SDL_RENDERER_PRESENTVSYNC: taxa de atualiza��o sincronizada com a do dispositivo

	// As texturas que ser�o renderizadas pelo renderer.
	// SDL_PIXELFORMAT_RGBA8888: cada pixel usar� 4 bytes de mem�ria = 32 bits.
	// --> 8 bits para red, 8 bits para green, 8 bits para blue, e 8 bits para alpha
	// ou seja, 255 combina��es diferentes de cores para para R, para G, para B e para A.
	// RGB: red, green e blue. Cada um deles usa 4 bytes, o que significa que podem guardar um valor at� 255
	m_gTexture = SDL_CreateTexture(m_gRenderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);


	// aloca 4bytes/32 bits para cada p�xel da janela
	m_pixelBuffer = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
	m_pixelBufferBlurred = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
	cout << "Buffer de pixel definido" << endl;
	// memset: escreve um valor em uma determinada regi�o da mem�ria
	// dado um valor, e dado um tamanho total de mem�ria
	//memset(m_pixelBuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
	memset(m_pixelBuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
	memset(m_pixelBufferBlurred, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

	return true;
};

void Screen::close() {
	cout << "Closing application" << endl;
	delete[]m_pixelBuffer;
	delete[]m_pixelBufferBlurred;
	SDL_DestroyTexture(m_gTexture);
	SDL_DestroyRenderer(m_gRenderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
};

bool Screen::manageEvents() {
	SDL_Event event;
	// gerenciador de eventos do SDL: SDL_PollEvent()
	// eventos: movimento de mouse, clicks, bot�es
	while (SDL_PollEvent(&event) == 1) {
		/*
		SDL_PollEvents: l� eventos da fila de eventos.
		Caso haja eventos na fila, a fun��o remove o pr�ximo evento da fila,
		encaixa esse evento na structure respectiva, e retorna 1 como retorno.
		Enquanto o retorno for 1, � poss�vel acessar os membros da structure
		*/
		switch (event.type)
		case SDL_QUIT: {
			return false;
		}
	}
	return true;
};

void Screen::update() {
	// atualiza o ret�ngulo de textura com as informa��es de pixeis passadas
	SDL_UpdateTexture(m_gTexture, NULL, m_pixelBuffer, SCREEN_WIDTH * sizeof(Uint32));
	SDL_RenderClear(m_gRenderer);
	// copia uma por��o de textura para o renderizador
	SDL_RenderCopy(m_gRenderer, m_gTexture, NULL, NULL);
	SDL_RenderPresent(m_gRenderer);
};

void Screen::setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue) {

	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
		return;
	}
	Uint32 color = 0;
	color += red;
	color <<= 8;
	color += green;
	color <<= 8;
	color += blue;
	color <<= 8;
	color += 0xFF;

	m_pixelBuffer[(y * SCREEN_WIDTH) + x] = color;
};

void Screen::unsetPixel() {
	memset(m_pixelBuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
	memset(m_pixelBufferBlurred, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
};

void Screen::applyBlur() {
	// pointer swap
	Uint32* tmpPointer = m_pixelBuffer;
	m_pixelBuffer = m_pixelBufferBlurred;
	m_pixelBufferBlurred = tmpPointer;

	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			
			/* 9 pixels. The 8 around are going to have a average color of the center pixel
			0 0 0
			0 1 0
			0 0 0
			*/
			int redTotal = 0;
			int greenTotal = 0;
			int blueTotal = 0;
			for (int row = -1; row <= 1; row++) {
				for (int col = -1; col <= 1; col++) {
					int currentX = x + col;
					int currentY = y + row;

					if ((currentX >= 0 && currentX < SCREEN_WIDTH)
					 && (currentY >= 0 && currentY < SCREEN_HEIGHT)) {
						Uint32 color = m_pixelBufferBlurred[currentY * SCREEN_WIDTH + currentX];

						// particular color of the center pixel
						Uint8 red = color >> 24;
						Uint8 green = color >> 16;
						Uint8 blue = color >> 8;

						redTotal += red;
						greenTotal += green;
						blueTotal += blue;
					}
				}
			}
			Uint8 red = redTotal / 9;
			Uint8 green = greenTotal / 9;
			Uint8 blue = blueTotal / 9;

			setPixel(x, y, red, green, blue);
		}
	}
};

}