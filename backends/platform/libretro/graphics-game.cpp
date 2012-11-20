#include <stdio.h>

#include "graphics.h"
#include "os.h"

Graphics::Surface RetroGraphicsManager::_gameScreen;
RetroPalette RetroGraphicsManager::_gamePalette;
int RetroGraphicsManager::_screenChangeCount; 		
		
// GAME VIDEO
void RetroGraphicsManager::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
    _gameScreen.create(width, height, format ? *format : Graphics::PixelFormat::createFormatCLUT8());
}

int16 RetroGraphicsManager::getWidth()
{
    return _gameScreen.w;
}

int16 RetroGraphicsManager::getHeight()
{
    return _gameScreen.h;
}

void RetroGraphicsManager::beginGFXTransaction()
{
    //?
}

OSystem::TransactionError RetroGraphicsManager::endGFXTransaction()
{
    //?
    _screenChangeCount ++;
    return OSystem::kTransactionSuccess;
}

int RetroGraphicsManager::getScreenChangeID() const
{
    _screenChangeCount;
}

void RetroGraphicsManager::setPalette(const byte *colors, uint start, uint num)
{
    _gamePalette.set(colors, start, num);
}

void RetroGraphicsManager::grabPalette(byte *colors, uint start, uint num)
{
    _gamePalette.get(colors, start, num);
}

void RetroGraphicsManager::copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h)
{
	const byte *src = (const byte *)buf;
	byte *dst = (byte *)_gameScreen.pixels + y * _gameScreen.pitch + x * _gameScreen.format.bytesPerPixel;
	
	for (int i = 0; i < h; i++)
	{
		memcpy(dst, src, w * _gameScreen.format.bytesPerPixel);
		src += pitch;
		dst += _gameScreen.pitch;
	}
}

Graphics::Surface *RetroGraphicsManager::lockScreen()
{
    return &_gameScreen;
}

void RetroGraphicsManager::unlockScreen()
{
}

void RetroGraphicsManager::fillScreen(uint32 col)
{
    _gameScreen.fillRect(Common::Rect(_gameScreen.w, _gameScreen.h), col);
}

void RetroGraphicsManager::updateScreen()
{
    extern void retro_leave_thread();
    retro_leave_thread();
}