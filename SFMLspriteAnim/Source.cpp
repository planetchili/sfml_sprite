#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

class Animation
{
public:
	Animation() = default;
	Animation( int x,int y,int width,int height )
	{
		texture.loadFromFile( "professor_walk_cycle_no_hat.png" );
		for( int i = 0; i < nFrames; i++ )
		{
			frames[i] = { x + i * width,y,width,height };
		}
	}
	void ApplyToSprite( sf::Sprite& s ) const
	{
		s.setTexture( texture );
		s.setTextureRect( frames[iFrame] );
	}
	void Update( float dt )
	{
		time += dt;
		while( time >= holdTime )
		{
			time -= holdTime;
			Advance();
		}
	}
private:
	void Advance()
	{
		if( ++iFrame >= nFrames )
		{
			iFrame = 0;
		}
	}
private:
	static constexpr int nFrames = 8;
	static constexpr float holdTime = 0.1f;
	sf::Texture texture;
	sf::IntRect frames[nFrames];
	int iFrame = 0;
	float time = 0.0f;
};

class Character
{
private:
	enum class AnimationIndex
	{
		WalkingUp,
		WalkingDown,
		WalkingLeft,
		WalkingRight,
		Count
	};
public:
	Character( const sf::Vector2f& pos )
		:
		pos( pos )
	{
		sprite.setTextureRect( { 0,0,64,64 } );
		animations[int( AnimationIndex::WalkingUp )] = Animation( 64,0,64,64 );
		animations[int( AnimationIndex::WalkingDown )] = Animation( 64,128,64,64 );
		animations[int( AnimationIndex::WalkingLeft )] = Animation( 64,64,64,64 );
		animations[int( AnimationIndex::WalkingRight )] = Animation( 64,192,64,64 );
	}
	void Draw( sf::RenderTarget& rt ) const
	{
		rt.draw( sprite );
	}
	void SetDirection( const sf::Vector2f& dir )
	{
		vel = dir * speed;
		if( dir.x > 0.0f )
		{
			curAnimation = AnimationIndex::WalkingRight;
		}
		else if( dir.x < 0.0f )
		{
			curAnimation = AnimationIndex::WalkingLeft;
		}
		else if( dir.y < 0.0f )
		{
			curAnimation = AnimationIndex::WalkingUp;
		}
		else if( dir.y > 0.0f )
		{
			curAnimation = AnimationIndex::WalkingDown;
		}
	}
	void Update( float dt )
	{
		pos += vel * dt;
		animations[int( curAnimation )].Update( dt );
		animations[int( curAnimation )].ApplyToSprite( sprite );
		sprite.setPosition( pos );
	}
private:
	static constexpr float speed = 100.0f;
	sf::Vector2f pos;
	sf::Vector2f vel = {0.0f,0.0f};
	sf::Sprite sprite;
	Animation animations[int( AnimationIndex::Count )];
	AnimationIndex curAnimation = AnimationIndex::WalkingDown;
};

int main()
{
	// Create the main window
	sf::RenderWindow window( sf::VideoMode( 800,600 ),"SFML window" );

	Character fucker( { 100.0f,100.0f } );

	// timepoint for delta time measurement
	auto tp = std::chrono::steady_clock::now();

	// Start the game loop
	while( window.isOpen() )
	{
		// Process events
		sf::Event event;
		while( window.pollEvent( event ) )
		{
			// Close window: exit
			if( event.type == sf::Event::Closed )
				window.close();
		}

		// get dt
		float dt;
		{
			const auto new_tp = std::chrono::steady_clock::now();
			dt = std::chrono::duration<float>( new_tp - tp ).count();
			tp = new_tp;
		}

		// handle input
		sf::Vector2f dir = { 0.0f,0.0f };
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) )
		{
			dir.y -= 1.0f;
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) )
		{
			dir.y += 1.0f;
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
		{
			dir.x -= 1.0f;
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) )
		{
			dir.x += 1.0f;
		}
		fucker.SetDirection( dir );
		

		// update model
		fucker.Update( dt );

		// Clear screen
		window.clear();
		// Draw the sprite
		fucker.Draw( window );
		// Update the window
		window.display();
	}
	return EXIT_SUCCESS;
}