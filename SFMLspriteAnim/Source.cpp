#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class TextureCodex
{
public:
	static std::shared_ptr<sf::Texture> Acquire( const std::string& name )
	{
		const auto i = texturePtrs.find( name );
		if( i != texturePtrs.end() )
		{
			return i->second;
		}
		else
		{
			auto pTex = std::make_shared<sf::Texture>();
			pTex->loadFromFile( name );
			texturePtrs.insert( { name,pTex } );
			return pTex;
		}
	}
	static void MurderOrphans()
	{
		for( auto i = texturePtrs.begin(); i != texturePtrs.end(); )
		{
			if( i->second.unique() )
			{
				i = texturePtrs.erase( i );
			}
			else
			{
				++i;
			}
		}
	}
private:
	static std::unordered_map<std::string,std::shared_ptr<sf::Texture>> texturePtrs;
};

std::unordered_map<std::string,std::shared_ptr<sf::Texture>> TextureCodex::texturePtrs;

class Animation
{
public:
	Animation() = default;
	Animation( int x,int y,int width,int height,int nFrames,float holdTime )
		:
		holdTime( holdTime )
	{
		frames.reserve( nFrames );
		pTex = TextureCodex::Acquire( "professor_walk_cycle_no_hat.png" );
		for( int i = 0; i < nFrames; i++ )
		{
			frames.emplace_back( sf::Vector2i{ x,y },sf::Vector2i{ width,height } );
			x += width;
		}
	}
	void ApplyToSprite( sf::Sprite& s ) const
	{
		s.setTexture( *pTex );
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
		if( ++iFrame >= int( frames.size() ) )
		{
			iFrame = 0;
		}
	}
private:
	float holdTime;
	std::shared_ptr<sf::Texture> pTex;
	std::vector<sf::IntRect> frames;
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
		StandingUp,
		StandingDown,
		StandingLeft,
		StandingRight,
		Count
	};
public:
	Character( const sf::Vector2f& pos )
		:
		pos( pos )
	{
		animations[(int)AnimationIndex::WalkingUp] = Animation( 64,0,64,64,8,0.1f );
		animations[(int)AnimationIndex::WalkingLeft] = Animation( 64,64,64,64,8,0.1f );
		animations[(int)AnimationIndex::WalkingDown] = Animation( 64,128,64,64,8,0.1f );
		animations[(int)AnimationIndex::WalkingRight] = Animation( 64,192,64,64,8,0.1f );
		animations[(int)AnimationIndex::StandingUp] = Animation( 0,0,64,64,1,10.1f );
		animations[(int)AnimationIndex::StandingLeft] = Animation( 0,64,64,64,1,10.1f );
		animations[(int)AnimationIndex::StandingDown] = Animation( 0,128,64,64,1,10.1f );
		animations[(int)AnimationIndex::StandingRight] = Animation( 0,192,64,64,1,10.1f );
	}
	void Draw( sf::RenderTarget& rt ) const
	{
		rt.draw( sprite );
	}
	void SetDirection( const sf::Vector2f& dir )
	{
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
		else
		{
			if( vel.x > 0.0f )
			{
				curAnimation = AnimationIndex::StandingRight;
			}
			else if( vel.x < 0.0f )
			{
				curAnimation = AnimationIndex::StandingLeft;
			}
			else if( vel.y < 0.0f )
			{
				curAnimation = AnimationIndex::StandingUp;
			}
			else if( vel.y > 0.0f )
			{
				curAnimation = AnimationIndex::StandingDown;
			}
		}
		vel = dir * speed;
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
	AnimationIndex curAnimation = AnimationIndex::StandingDown;
};

int main()
{
	// Create the main window
	sf::RenderWindow window( sf::VideoMode( 800,600 ),"SFML window" );

	{
		Character fucker( { 100.0f,100.0f } );
		Character fucker2( { 100.0f,200.0f } );
	}

	TextureCodex::MurderOrphans();

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