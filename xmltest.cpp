#include <iostream>
#include <string>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

XMLDocument doc;

int main () {
    //test file for tinyxml

    /*XMLElement * pRootElem = doc.RootElement(); 
    XMLElement * pBrickStyle = pRootElem->FirstChildElement("styles")->FirstChildElement("brickstyle2");
    string temp;
    temp = pBrickStyle->FirstChildElement("color")->FirstChildElement("g")->GetText();
    cout<<temp;*/

    XMLError err = doc.LoadFile("brickdata.xml");

    if (err != XML_SUCCESS) cout<<"Error, can't read xml file"<<endl;

    XMLElement *pRootElem = doc.RootElement();

    if (NULL != pRootElem)
    {
        //get brickstyles list
        XMLElement * pStyles = pRootElem->FirstChildElement("styles"); 

        //get brick layout
        //hardcoded level change?
        //this should change each time a new level is introduced
        XMLElement * pBricks = pRootElem->FirstChildElement("level1"); 

        if (NULL != pBricks)
        {
            XMLElement * pBrickColor;

            //split bricks xml into array for each brick individually
            string sBricks = pBricks->GetText();
            char cBricks[sBricks.length()+1];
            strcpy(cBricks, sBricks.c_str());

            //for each brick in bricks in xml, pull the brick,
            //pull its color/hp values and set it in place
            for (int i = 0; i <60; i++)
            {
                char check = cBricks[i];

                //depending on brick id, pull color values
                //first check if emptyspace, if emptyspace just skip that iteration
                //if emptyspace, skip entire while and do it again
                //hardcoded, no overload for string

                switch(check){
                    case '1':
                        pBrickColor = pStyles->FirstChildElement("brickstyle1")->FirstChildElement("color");
                        break;
                    case '2':
                        pBrickColor = pStyles->FirstChildElement("brickstyle2")->FirstChildElement("color");
                        break;
                    case '3':
                        pBrickColor = pStyles->FirstChildElement("brickstyle3")->FirstChildElement("color");
                        break;
                    case '4':
                        pBrickColor = pStyles->FirstChildElement("brickstyle4")->FirstChildElement("color");
                        break;
                    case '5':
                        pBrickColor = pStyles->FirstChildElement("brickstyle5")->FirstChildElement("color");
                        break;
                    case '_':
                        continue;
                }
                
                cout<<"this is fine"<<endl;

                //get each value for the color
                XMLElement * r = pBrickColor->FirstChildElement("r");
                int rr = stoi(r->GetText());
                XMLElement * g = pBrickColor->FirstChildElement("g");
                int gg = stoi(g->GetText());
                XMLElement * b = pBrickColor->FirstChildElement("b");
                int bb = stoi(b->GetText());
                XMLElement * a = pBrickColor->FirstChildElement("a");
                int aa = stoi(a->GetText());

                //get brick hp
                XMLElement * pBrickHp = pStyles->FirstChildElement("hitpoints");

                //add to renderer
                cout<<check<<" "<<rr<<" "<<gg<<" "<<bb<<" "<<aa<<endl; 
            }    
        }

    }
    return 0;   
}