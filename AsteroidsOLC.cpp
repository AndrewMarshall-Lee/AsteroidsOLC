// AsteroidsOLC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <string>
#include "olcConsoleGameEngine.h"
using namespace std;


class OLC_Asteroids : public olcConsoleGameEngine
{
public:
    OLC_Asteroids()
    {
        m_sAppName = L"Asteroids";
    }

private:
    struct sSpaceObject
    {
        float x;
        float y;
        float dx;
        float dy;
        int nSize;
        float angle;
    };
    vector<sSpaceObject> vecAsteroid;
    sSpaceObject player;
    vector<sSpaceObject> bullets;

    bool bDead;
    int nScore = 0;
    vector<pair<float, float>> ModelShip;
    vector<pair<float, float>> ModelAsteroid;

protected:
    //overriding initialising functions
    virtual bool OnUserCreate()
    {
        ModelShip =
        {
        {0.0f,-5.0f},
        {-2.5f,+2.5f},
        {+2.5f,+2.5f}
        };

        //making model asteroid
        int verts = 20;
        for (int i = 0; i < verts; i++)
        {
            float radius = 1.0f;
            float a = ((float)i / (float)verts) * 6.28f;
            ModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cos(a)));
        }
        resetGame();
        return true;
    }


    virtual bool OnUserUpdate(float fElapsedTime)
    {

        //clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

        //Steering

        if (m_keys[VK_LEFT].bHeld)
            player.angle -= 5.0f * fElapsedTime;

        if (m_keys[VK_RIGHT].bHeld)
            player.angle += 5.0f * fElapsedTime;

        //Thrust

        if (m_keys[VK_UP].bHeld)
        {
            player.dx += sin(player.angle) * 20.f * fElapsedTime;
            player.dy -= cos(player.angle) * 20.f * fElapsedTime;
        }

        player.x += player.dx * fElapsedTime;
        player.y += player.dy * fElapsedTime;

        //Wrap Ship
        WrapCoords(player.x, player.y, player.x, player.y);

        //check if fired

        if (m_keys[VK_SPACE].bReleased)
            bullets.push_back({player.x, player.y, 50.0f * sinf(player.angle), -50.0f*cosf(player.angle), 0, 0});

        //check if dead
        if (bDead) resetGame();

   

        vector<sSpaceObject> newAsteroids;

        //update and draw bullets
        for (auto& b : bullets)
        {
            b.x += b.dx * fElapsedTime;
            b.y += b.dy * fElapsedTime;
            WrapCoords(b.x, b.y, b.x, b.y);
            Draw(b.x,b.y);
            //check for asteroid collision
            for (auto& a : vecAsteroid)
            {
                if (IsPointInRadius(a.x, a.y, a.nSize, b.x, b.y))
                {
                    //Asteroid hit
                    b.x = -100; //force bullet off screen to be clean up later
                    nScore += 200;
                    if (a.nSize > 4)
                    {
                        //split asteroid in half
                        float angle1 = ((float)rand() / (float)RAND_MAX) * 6.2831f;
                        float angle2 = ((float)rand() / (float)RAND_MAX) * 6.2831f;
                        newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)a.nSize >> 1, 0.0f });
                        newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), (int)a.nSize >> 1, 0.0f });
                        
                    }
                    a.x = -100;
                }
            }
        }
        //append all new asteroids to existing vector
        for (auto p : newAsteroids)
            vecAsteroid.push_back(p);

        //remove off-screen bullets
        if (bullets.size() > 0)
        {
            //loop through bullet list and remove if off screen
            auto i = remove_if(bullets.begin(), bullets.end(), 
                [&](sSpaceObject o) {return o.x < 1 || o.y < 1 || o.x > ScreenWidth() || o.y > ScreenHeight(); });
            //remove only if there are bullets to remove
            if (i != bullets.end())
                bullets.erase(i);
        }

        //remove hit asteroids
        if (vecAsteroid.size() > 0)
        {
            //loop through bullet list and remove if off screen
            auto i = remove_if(vecAsteroid.begin(), vecAsteroid.end(),
                [&](sSpaceObject a) {return a.x < -1; });
            //remove only if there are bullets to remove
            if (i != vecAsteroid.end())
                vecAsteroid.erase(i);
        }
        

        //update and draw asteroids
        for (auto& a : vecAsteroid)
        {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;
            a.angle += 0.5f * fElapsedTime;
            WrapCoords(a.x, a.y, a.x, a.y);
            DrawWireFrameModel(ModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, FG_WHITE);
        }

        //Ship collision detection
        for (auto &a : vecAsteroid)
        {
            if (IsPointInRadius(a.x, a.y, a.nSize, player.x, player.y))
                bDead = true;
        }

       
        
        //drawShip

        DrawWireFrameModel(ModelShip, player.x, player.y, player.angle, 1.0);

        // Draw Score
        DrawString(2, 2, L"SCORE: " + to_wstring(nScore));

        //check if level clear
        if (vecAsteroid.size() == 0)
        {
            nScore += 1000;
            float new_x1 = ((float)rand() / (float)RAND_MAX) * ScreenWidth() + player.x;
            float new_y1 = ((float)rand() / (float)RAND_MAX) * ScreenHeight() + player.y;

            float new_x2 = ((float)rand() / (float)RAND_MAX) * ScreenWidth() + player.x;
            float new_y2 = ((float)rand() / (float)RAND_MAX) * ScreenHeight() + player.y;
            // Put in two asteroids
            vecAsteroid.push_back({ new_x1, new_y1, 8.0f, -6.0f, (int)16, 0.0f });
            vecAsteroid.push_back({ new_x2, new_y2, -5.0f, 3.0f, (int)16,0.0f });

        }

        return true;
    }
    bool IsPointInRadius(float cx, float cy, float radius, float x, float y)
    {
        return (x - cx) * (x - cx) + (y - cy) * (y - cy) <= (radius*radius);
    }
    void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
    {
        // pair.first = x coord
        // pair.second = y coord
        
        //translated model vector ofcoord pairs
        vector<pair<float, float>> TransformedCoords;
        
        int verts = vecModelCoordinates.size();
        TransformedCoords.resize(verts);
        //Rotate


        for (int i = 0; i < verts; i++)
        {
            TransformedCoords[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
            TransformedCoords[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);

        }
        // Scale
        for (int i = 0; i < verts; i++)
        {
            TransformedCoords[i].first = TransformedCoords[i].first * s;
            TransformedCoords[i].second = TransformedCoords[i].second * s;
        }

        // Translate
        for (int i = 0; i < verts; i++)
        {
            TransformedCoords[i].first = TransformedCoords[i].first + x;
            TransformedCoords[i].second = TransformedCoords[i].second + y;
        }

        // Draw Closed Polygon
        for (int i = 0; i < verts + 1; i++)
        {
            int j = (i + 1);
            DrawLine(TransformedCoords[i % verts].first, TransformedCoords[i % verts].second,
                TransformedCoords[j % verts].first, TransformedCoords[j % verts].second, PIXEL_SOLID, col);
        }
    }


    void WrapCoords(float ix, float iy, float& ox, float& oy)
    {
        //output = input
        ox = ix;
        oy = iy;

        if (ix < 0.0f) ox = ix + (float)ScreenWidth();
        if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();

        if (iy < 0.0f) oy = iy + (float)ScreenHeight();
        if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
    }

    virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
    {
        //I overwrite the Draw function and supply wrapped coordinates, then draw each pixel with the wrapped coordinates with the original void
        float fx, fy;
        WrapCoords(x, y, fx, fy);
        olcConsoleGameEngine:: Draw(fx, fy, c, col);
    }

    void resetGame()
    {
        vecAsteroid.clear();
        bullets.clear();
        //    //make first asteroid
        //    vecAsteroid.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });

        //    //Initialise player
        player.x = ScreenWidth() / 2.0f;
        player.y = ScreenHeight() / 2.0f;
        player.dx = 0.0f;
        player.dy = 0.0f;
        player.angle = 0.0f;



        //    // Put in two asteroids
        vecAsteroid.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
        vecAsteroid.push_back({ 200.0f, 20.0f, -5.0f, 3.0f, (int)16,0.0f });

        bDead = -false;
        nScore = 0;
    }
  
};

int main()
{
    //create instance of the "game" class.
    OLC_Asteroids game;
    game.ConstructConsole(200, 80, 8, 8);
    game.Start();

    return 0;
}
