# NavMesh

```cpp
#include <SFML/Graphics.hpp>
#include "Mesh.h"  

int main()
{
    // Create a 800x600 super rectangle.
    // Reserve memory for 500 half edges.
    Mesh mesh(800,600,500);
    
    // Insert some segments.
    // Does not manage segments intersection, will be implemented soon.
    mesh.insertSegment(210,532,413,549);
    mesh.insertSegment(233,240,389,241);
    mesh.insertSegment(306,48,576,66);
    mesh.insertSegment(557,119,775,41);
    mesh.insertSegment(446,105,323,185);
    mesh.insertSegment(214,136,183,278);
    mesh.insertSegment(191,307,406,271);
    mesh.insertSegment(161,335,26,392);
    mesh.insertSegment(165,440,160,569);
    mesh.insertSegment(127,113,114,28);
    mesh.insertSegment(94,158,25,202);
    mesh.insertSegment(248,474,474,499);
    mesh.insertSegment(395,330,403,447);
    mesh.insertSegment(494,511,548,570);
    mesh.insertSegment(382,196,511,214);
    mesh.insertSegment(480,286,492,326);
    mesh.insertSegment(583,168,768,274);
    mesh.insertSegment(635,238,664,366);
    mesh.insertSegment(607,480,712,552);
    mesh.insertSegment(543,521,689,568);

    sf::RenderWindow window(sf::VideoMode(800,600), "SFML window");
    
    sf::Event event;
    
    sf::Vector2i v1,v2;
    sf::VertexArray path(sf::Lines);
    
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                v1 = sf::Mouse::getPosition(window);
            }
            
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                v2 = sf::Mouse::getPosition(window);
                // Create path. 
                // Does not manage non-point objets, will be implemented soon.
                path = mesh.path(v1.x,v1.y,v2.x,v2.y);
            }
        }
        
        window.clear();
        
        // Show the triangulation.
        window.draw(mesh.getArray());
        
        window.draw(path);
        
        window.display();
    }
    
    return 0;
}
```
