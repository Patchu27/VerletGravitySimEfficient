
#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>

int main()
{//for some reason the spatial partitioning isn't really helping much, and the cellslots capacity is all outta wack. I keep needing to up it even though the allotment should only be like 4 or 5
    const unsigned int screenwidth = 1920u;
    const unsigned int screenheight = 1080u;
    const unsigned int radiusval = 16u;
    const unsigned int nobjects = 400u;
    const unsigned int seglength = 9u;
    const unsigned int nobjectsfor = nobjects * seglength;
    const float ar = 0.0002f;//0.0004;//-0
    const float pival = 3.1416f;
    const unsigned int substeps = 1;//12u;
    const unsigned int physsteps = 1u;//this still needs to be fully implemented, so just leave it at 1
    const float psmult = 1.f / (physsteps);//if you don't put .f it makes the output 0
    float* objects = new float[nobjectsfor];
    const unsigned int sepn = 27u;//linux port note: not sure why this is 27 and not something like 16 or 32
    const unsigned int rowlength = ceil(screenwidth / sepn);
    const unsigned int columnlength = ceil(screenheight / sepn);
    const unsigned int cellslots = 6u;//this problem is fixed now, look at the notes below at the array creation
    const unsigned int gridsize = rowlength * columnlength * cellslots;
    const unsigned int gridsizeadj = gridsize - cellslots;
    int* grid = new int[gridsize];//MAKE SURE THIS IS SIGNED OR IT WON'T WORK. You don't have to write signed, but it can't hurt, and yeah no kidding it was messed up. For every position that had a -1, it gave an overflowed number. It wrecked the whole system because I put unsigned at the start. It's a miracle it worked like that at all.
    const float squishval = 1.f/substeps;//0.16f;//substeps / substeps;//0.16 mild//use substeps to make it 1, as in full pushback
    //
    {
        float px, py, mass;
        for (int n = 0;n != nobjectsfor;n += seglength) {
            px = rand() % screenwidth;
            py = rand() % screenheight;
            objects[n] = px - (rand() % 10) + 5;
            objects[n + 1] = py - (rand() % 10) + 5;
            objects[n + 2] = px;
            objects[n + 3] = py;
            objects[n + 4] = px;
            objects[n + 5] = py;
            mass = 10000.f;//(5000.f + (rand() % 2)*100000.f)*0.05f;
            objects[n + 6] = mass;
            objects[n + 7] = pow((3 * mass) / (4 * pival), 0.33333f);
            //float test = objects[n + 7];
            //   objects[n + 8] = 0;
        }
    }


    sf::Color* colors = new sf::Color[nobjects];
    {
        for (int n = 0;n != nobjects;n++) {
            sf::Color color(10 + rand() % 255, 10 + rand() % 255, 10 + rand() % 255);
            colors[n] = color;
        }
    }
    /*
    const unsigned int vertexCount = 3;
    const unsigned int seglengthvert = vertexCount+1;
    sf::VertexArray vertexArray(sf::TriangleFan, seglengthvert * nobjects);
    unsigned int nadj;
    unsigned int nadjvert;
    for (int n = 0;n != nobjectsfor;n += seglength) {
        nadj = n / seglength;
        nadjvert = nadj * seglengthvert;
        const float radius = objects[n+7];
        const float pival = 3.141592654f*2.f/vertexCount;

       // vertexArray[nadj].position = sf::Vector2f(objects[n+2], objects[n+3]);
       // vertexArray[nadj].color = sf::Color::colors[nadj];
        vertexArray[nadjvert].position = sf::Vector2f(objects[n + 2], objects[n + 3]);
        vertexArray[nadjvert].color = colors[nadj];
        for (unsigned int i = 0; i != vertexCount-1; i++)
        {
            float angle = pival*i;
            vertexArray[1+nadjvert+i].position = sf::Vector2f(objects[n+2]+radius * cos(angle), objects[n+3]+radius * sin(angle));
            vertexArray[1+nadjvert+i].color = colors[nadj];
        }
    }*/
    const unsigned short npoints = nobjects;
    sf::VertexArray points(sf::Points, npoints);
    {
        for (unsigned short i = 0; i != npoints; ++i)
        {
            //sf::Color color
            points[i].color = colors[i];
            //points[i].color = sf::Color::White;
        }
    }

    // const std::string icon = sf::Image::loadFromFile("C:\\Users\\Pjm27\\OneDrive\\Desktop\\Icons\\oIcon");
  //   sf::Image::getPixelsPtr()	const
    sf::CircleShape circle(radiusval, 16);
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(50.f, 50.f);
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(screenwidth, screenheight), "EfficientVerletGravitySimulation");
    //window.setFramerateLimit(60);
    while (window.isOpen()) {
        clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) 	window.close();
        }
        //
            for (int n=0;n!=5;n++)//I put this back because substeps are not accounted for properly//taken out in linux port and replaced with the line below to *actually* implement substeps since not including it in its function is misleading
           //     for (int n=0;n!=substeps;n++)
            {//just to make an object here to limit the scoper
                float px, py, sval, s, xdist, ydist, mass, totaldist, pxother, pyother, pxoldother, pyoldother,massrat,xoffset,yoffset,percent,difference;
                for (int n = 0;n != nobjectsfor;n += seglength) {
                    px = objects[n + 2];
                    py = objects[n + 3];
                    mass = objects[n + 6];
                  //  xdist = sf::Mouse::getPosition().x - px;
                  //  ydist = sf::Mouse::getPosition().y - py;
                  //  sval = 1 / sqrt(xdist * xdist + ydist * ydist);
                  //  px += xdist * sval * 0.05;
                  //  py += ydist * sval * 0.05;
                    for (int nchecks = 0;nchecks != nobjectsfor;nchecks += seglength) {
                        if (n != nchecks) {
                            pxother = objects[nchecks + 2];
                            pyother = objects[nchecks + 3];
                            float massrat = objects[nchecks + 6] / mass;
                            xdist = px-pxother;
                            ydist = py - pyother;
                            sval = 1 / sqrt(xdist * xdist + ydist * ydist);
                            px -= xdist * sval * ar * massrat;
                            py -= ydist * sval * ar * massrat;
                            totaldist=objects[n + 7] + objects[nchecks+ 7];
                            s = 1 / sval;
                            if (s < totaldist) {
                               // pxoldother = objects[nchecks];
                               // pyoldother = objects[nchecks + 1];
                                    difference = totaldist - s;
                                    percent = (difference / s) * 0.5f;
                                    xoffset = xdist * percent;
                                    yoffset = ydist * percent;
                                    massrat = 1.f;//objects[nchecks + 6] / objects[n + 6];
                                    //pxold=pxold+(px-pxold)*bouncedampval;
                                    px += xoffset * massrat * squishval;//you can divide these numbers to basically make the collisions forgiving or squishy. The more you divide it, the squishier it gets.
                                    //pyold=pyold+(py-pyold)*bouncedampval;
                                    py += yoffset * massrat * squishval;
                                    //

                                    //pxoldother=pxoldother+(pxother-pxoldother)*bouncedampval;
                                    pxother -= xoffset / massrat * squishval;
                                    //pyoldother=pyoldother+(pyother-pyoldother)*bouncedampval;
                                    pyother -= yoffset / massrat * squishval;
                                    //
                                    //pxold = pxold - (pxold - px)*ballbouncedamp;
                                    //pyold = pyold - (pyold - py) * ballbouncedamp;
                                    //
                                    objects[nchecks + 2] = pxother;//WHY WON'T YOU BOUNCE
                                    objects[nchecks + 3] = pyother;
                            }
                        }
                    }
                    objects[n + 4] = px;
                    objects[n + 5] = py;
                    
                }
            }


            {
                float drawxdist, drawydist;
                for (int n = 0;n != nobjectsfor;n += seglength) {
                    objects[n + 2] = objects[n + 4];
                    objects[n + 3] = objects[n + 5];
                    drawxdist = objects[n + 2] - objects[n];
                    drawydist = objects[n + 3] - objects[n + 1];
                    objects[n] = objects[n + 2];
                    objects[n + 1] = objects[n + 3];
                    objects[n + 2] += drawxdist;
                    objects[n + 3] += drawydist;
                }
            }

            
                            
                        
                        //
                    
                
            
        
        //
        window.clear();
        {
            float radius;
            for (int ndraw = 0;ndraw != nobjectsfor;ndraw += seglength) {
                radius = objects[ndraw + 7];
                circle.setPosition(objects[ndraw + 2] - radius, objects[ndraw + 3] - radius);
                circle.setRadius(radius);
                circle.setFillColor(colors[ndraw / seglength]);
                window.draw(circle);

            }
            int pos;
            for (unsigned short i = 0; i != npoints; ++i)
            {
                pos = i * seglength;
                points[i].position = sf::Vector2f(objects[pos + 2], objects[pos + 3]);
                //points[i].color = sf::Color::White;
            }
        }
        //  window.draw(vertexArray);
        window.draw(points);
        window.display();

        // unsigned int addfilled = 0;
        // for (int i = 0;i != gridsize;i++) {
          //   std::cout << grid[i] << i / cellslots;
             //system("pause");
             //addfilled += grid[i]>=0;
       //  }
         //std::cout << addfilled-nobjects;
         //system("pause");
        // std::cout << std::endl;
        sf::Time elapsed2 = clock.getElapsedTime();
        std::cout << elapsed2.asMilliseconds() << std::endl;
        // int yval = floor(sf::Mouse::getPosition().y / sepn)*rowlength;
        // int xval = floor(sf::Mouse::getPosition().x / sepn);;
        //  int gridpos = (xval + yval) * cellslots;
         // std::cout << gridpos << std::endl;
    }
    delete[] objects;
    delete[] colors;
    delete[] grid;
    std::cout << "Program Terminated\n";
}
