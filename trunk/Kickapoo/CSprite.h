#pragma  once
#include "Common.h"

class CSpriteFrame
{
public:
	Texture* getTexture() { return m_frameTexture; }
	void setTexture(Texture* tex) {m_frameTexture = tex;}
private:
	Texture* m_frameTexture;
};

class CSprite : public resource<CSprite>
{
public:
	// Default constructor
	CSprite();

	// Initialize constructor
	CSprite(const std::vector<CSpriteFrame*> frames);

	~CSprite();

	// Load
	static CSprite* load(const std::string& name);

	// Add frame
	bool addFrame(CSpriteFrame* frame);

	// Delete frame
	bool deleteFrame(const std::string& name);
	bool deleteFrame(unsigned int index);

	void update();
	void draw();

	void setDelta(float delta) {m_maxDelta = delta;}
	float getDelta() const {return m_maxDelta;}
	float getActualDelta() const {return m_actualDelta;}
	int getActualFrame() const {return m_actualFrame;}
	bool isReverse() {return m_isReverse;}
	bool isStarted() {return m_isStarted;} 

	void setPosition(float x, float y)
	{
		m_position.x = x;
		m_position.y = y;
	}

private:
	std::vector<CSpriteFrame*> m_frames;
	bool m_isReverse;
	bool m_isStarted;
	unsigned int m_actualFrame;
	float m_actualDelta;
	float m_maxDelta;
	D3DXVECTOR2 m_position;
	D3DXVECTOR2 m_dimension;
};