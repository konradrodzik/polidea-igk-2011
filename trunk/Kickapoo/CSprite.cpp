#include "Common.h"
#include "CSprite.h"

CSprite::CSprite()
{
	addResource();
	m_actualDelta = 0.0f;
	m_actualFrame = 0;
	m_isReverse = false;
	m_position = D3DXVECTOR2(0.0f, 0.0f);
}

CSprite::CSprite( const std::vector<CSpriteFrame*> frames )
{
	addResource();
	m_actualDelta = 0.0f;
	m_actualFrame = 0;
	m_isReverse = false;
	m_position = D3DXVECTOR2(0.0f, 0.0f);
}

CSprite::~CSprite()
{
	removeResource();
}

CSprite* CSprite::load( const std::string& name )
{
	FILE* file = fopen(name.c_str(), "r");
	if(!file)
		return NULL;
	
	CSprite* sprite = new CSprite;

	int delta;
	int count;
	int width;
	int height;
	// delta(ms), count, width, height
	fscanf(file, "%i %i %i %i\n", &delta, &count, &width, &height );
	sprite->setDelta(delta/1000.0f);
	sprite->m_dimension = D3DXVECTOR2(width, height);

	CSpriteFrame* frame = NULL;
	Texture* tex = NULL;
	for(int i = 0; i < count; ++i)
	{
		char buff[255];
		ZeroMemory(&buff, 255);
		fscanf(file, "%s", buff);

		frame = new CSpriteFrame;
		tex = new Texture;

		tex->load(buff);
		frame->setTexture(tex);
		sprite->addFrame(frame);
	}

	fclose(file);
	return sprite;
}

bool CSprite::addFrame( CSpriteFrame* frame )
{
	if(!frame)
		return false;
	m_frames.push_back(frame);
	return true;
}

bool CSprite::deleteFrame( const std::string& name )
{
	int size = m_frames.size();
	for(int i = 0; i < size; ++i)
	{
		CSpriteFrame* tmp = m_frames[i];
		if(strcmp(tmp->getTexture()->getTextureName().c_str(), name.c_str()) == 0)
		{
			//m_frames.remove(tmp);
			return true;
		}
	}

	return false;
}

bool CSprite::deleteFrame( unsigned int index )
{
	/*if(index < 0 || index >= m_frames.size())
		return false;

	CSpriteFrame* frame = m_frames[index];
	if(!frame)
		return false;

	std::list<CSpriteFrame*>::iterator itor = m_frames

	m_frames.erase(itor);*/
	return true;
}

void CSprite::update()
{
	m_actualDelta += g_Timer()->getFrameTime(); 

	if(m_actualDelta >= m_maxDelta)
	{
		m_actualDelta = 0.0f;

		if(m_isReverse)
		{
			--m_actualFrame;
		}
		else
		{
			++m_actualFrame;
		}

		if(m_actualFrame < 0)
			m_actualFrame = m_frames.size()-1;
		if(m_actualFrame >= m_frames.size())
			m_actualFrame = 0;
		
	}
}

void CSprite::draw()
{
	CSpriteFrame* renderFrame = m_frames[m_actualFrame];
	renderFrame->getTexture()->set();
	g_Renderer()->drawRect(m_position.x, m_position.y, m_dimension.x, m_dimension.y);
}