/*
 Copyright (c) 2010-2015, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 This file is part of Cinder-Warping.

 Cinder-Warping is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Cinder-Warping is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "CiSpoutIn.h"
#include "Warp.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;

class SpoutWarpApp : public App {
public:
	static void prepare( Settings *settings );

	void setup() override;
	void cleanup() override;
	void update() override;
	void draw() override;

	void resize() override;

	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;

	void keyDown( KeyEvent event ) override;
	void keyUp( KeyEvent event ) override;
	SpoutIn	mSpoutIn;
	
private:
	fs::path		mSettings;

	//gl::TextureRef	mImage;
	WarpList		mWarps;

	Area			mSrcArea;
	int							mDisplayCount;
	int							getWindowsResolution();

	int							mMainWindowX, mMainWindowY;
	bool						mAutoLayout;
	int							mMainWindowWidth = 1280;
	int							mMainWindowHeight = 720;
	int							mRenderWidth = 1280;
	int							mRenderHeight = 720;
	int							mRenderX = 1024;
	int							mRenderY = 0;

};



void SpoutWarpApp::setup()
{
	//disableFrameRate();

	// initialize warps
	mSettings = getAssetPath( "" ) / "warps.xml";
	if( fs::exists( mSettings ) ) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings( loadFile( mSettings ) );
	}
	else {
		// otherwise create a warp from scratch		
		mWarps.push_back( WarpPerspectiveBilinear::create() );	
	}

	// load test image
	/*try {
		mImage = gl::Texture::create( loadImage( loadAsset( "help.png" ) ), 
									  gl::Texture2d::Format().loadTopDown().mipmap( true ).minFilter( GL_LINEAR_MIPMAP_LINEAR ) );

		mSrcArea = mImage->getBounds();

		// adjust the content size of the warps
		Warp::setSize( mWarps, mImage->getSize() );
	}
	catch( const std::exception &e ) {
		console() << e.what() << std::endl;
	}*/
	getWindowsResolution();
}

void SpoutWarpApp::cleanup()
{
	// save warp settings
	Warp::writeSettings( mWarps, writeFile( mSettings ) );
}

void SpoutWarpApp::update()
{
	/*if (mSpoutIn.getSize() != app::getWindowSize()) {
		app::setWindowSize(mSpoutIn.getSize());
	}*/
}

void SpoutWarpApp::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize( mWarps );
}

void SpoutWarpApp::mouseMove( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( !Warp::handleMouseMove( mWarps, event ) ) {
		// let your application perform its mouseMove handling here
	}
}

void SpoutWarpApp::mouseDown( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( !Warp::handleMouseDown( mWarps, event ) ) {
		// let your application perform its mouseDown handling here
		if (event.isRightDown()) { // Select a sender
			// SpoutPanel.exe must be in the executable path
			mSpoutIn.getSpoutReceiver().SelectSenderPanel(); // DirectX 11 by default
		}
	}
}

void SpoutWarpApp::mouseDrag( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( !Warp::handleMouseDrag( mWarps, event ) ) {
		// let your application perform its mouseDrag handling here
	}
}

void SpoutWarpApp::mouseUp( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( !Warp::handleMouseUp( mWarps, event ) ) {
		// let your application perform its mouseUp handling here
	}
}

void SpoutWarpApp::keyDown( KeyEvent event )
{
	// pass this key event to the warp editor first
	if( !Warp::handleKeyDown( mWarps, event ) ) {
		// warp editor did not handle the key, so handle it here
		switch( event.getCode() ) {
			case KeyEvent::KEY_ESCAPE:
				// quit the application
				quit();
				break;
			case KeyEvent::KEY_f:
				// toggle full screen
				setFullScreen( !isFullScreen() );
				break;
			case KeyEvent::KEY_v:
				// toggle vertical sync
				gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
				break;
			case KeyEvent::KEY_w:
				// toggle warp edit mode
				Warp::enableEditMode( !Warp::isEditModeEnabled() );
				break;
		}
	}
}
// utils
/*
mVDSession->getWindowsResolution();
	mRenderWindow->setBorderless();
	mRenderWindow->getSignalDraw().connect(std::bind(&videodrommLiveCodingApp::drawRender, this));
	mVDSettings->mRenderPosXY = ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY);
	mRenderWindow->setPos(50, 50);
	mRenderWindowTimer = 0.0f;
	timeline().apply(&mRenderWindowTimer, 1.0f, 2.0f).finishFn([&] { positionRenderWindow(); });
*/
int SpoutWarpApp::getWindowsResolution() {
	mDisplayCount = 0;
	for (auto display : Display::getDisplays())
	{
		//CI_LOG_V("VDUtils Window #" + toString(mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
		mDisplayCount++;
	}
	int w = Display::getMainDisplay()->getWidth();
	int h = Display::getMainDisplay()->getHeight();
	// Display sizes
	if (mAutoLayout)
	{
		mRenderX = 0;
		mRenderY = 0;
		mMainWindowWidth = w;
		mMainWindowHeight = h;
		// in case only one screen, render from x = 0
		if (mDisplayCount == 1) {
			mRenderX = 0;
			mRenderWidth = mMainWindowWidth;
			mRenderHeight = mMainWindowHeight;
		}
		else {
			mRenderX = mMainWindowWidth;
			for (auto display : Display::getDisplays())
			{
				mRenderWidth += display->getWidth();
				mRenderHeight = display->getHeight();
			}
			mRenderWidth -= mMainWindowWidth;
		}
	}
	//setBorderless();
	return w;
};
void SpoutWarpApp::keyUp( KeyEvent event )
{
	// pass this key event to the warp editor first
	if( !Warp::handleKeyUp( mWarps, event ) ) {
		// let your application perform its keyUp handling here
	}
}
void SpoutWarpApp::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());
	auto tex = mSpoutIn.receiveTexture();
	if (tex) {
		// Otherwise draw the texture and fill the screen
		//gl::draw(tex, getWindowBounds());

		// Show the user what it is receiving
		gl::ScopedBlendAlpha alpha;
		gl::enableAlphaBlending();

		//gl::drawString("Receiving from: " + mSpoutIn.getSenderName(), vec2(toPixels(20), toPixels(20)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
		//gl::drawString("fps: " + std::to_string((int)getAverageFps()), vec2(getWindowWidth() - toPixels(100), toPixels(20)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
		//gl::drawString("RH click to select a sender", vec2(toPixels(20), getWindowHeight() - toPixels(40)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
		// iterate over the warps and draw their content
		for (auto &warp : mWarps) {
			warp->draw(tex);
		}
	}
	else {
		gl::ScopedBlendAlpha alpha;
		gl::enableAlphaBlending();
		gl::drawString("No sender/texture detected", vec2(toPixels(20), toPixels(20)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	}
	getWindow()->setTitle(toString((int)getAverageFps()) + " fps");

}
void SpoutWarpApp::prepare(Settings *settings)
{
	settings->setWindowSize(1440, 900);
}

CINDER_APP( SpoutWarpApp, RendererGl( RendererGl::Options().msaa( 8 ) ), &SpoutWarpApp::prepare )
