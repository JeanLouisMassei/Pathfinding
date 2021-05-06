#ifndef Mesh_h
#define Mesh_h

#include <vector>               // std::vector
#include <SFML/Graphics.hpp>    // for Mesh::getArray() and Mesh::path(int,int,int,int)
#include <cmath>                // std::sqrt
#include <iostream>             // std::cout

class Mesh
{
    struct HalfEdge
    {
        int x;
        int y;
        HalfEdge* next;
        HalfEdge* pair;
        bool constrained;
    };
    
    std::size_t size;
    HalfEdge* m;
    HalfEdge* e;
    std::vector<HalfEdge*> edges;
    
    float distance(const int Ax, const int Ay, const int Bx, const int By)
    {
        return std::sqrt((Ax-Bx)*(Ax-Bx)+(Ay-By)*(Ay-By));
    }

    bool clockWise(const int Ax, const int Ay, const int Bx, const int By, const int Cx, const int Cy)
    {
        return (Bx-Ax)*(Cy-Ay)>(By-Ay)*(Cx-Ax);
    }
    
    bool notDelaunay()
    {
        int Ax = e->x;
        int Ay = e->y;
        int Bx = e->next->x;
        int By = e->next->y;
        int Cx = e->next->next->x;
        int Cy = e->next->next->y;
        int Px = e->pair->next->next->x;
        int Py = e->pair->next->next->y;
        float m = 2*(Ax*(By-Cy)+Bx*(Cy-Ay)+Cx*(Ay-By));
        float Zx = ((Ax*Ax+Ay*Ay)*(By-Cy)+(Bx*Bx+By*By)*(Cy-Ay)+(Cx*Cx+Cy*Cy)*(Ay-By))/m;
        float Zy = ((Ax*Ax+Ay*Ay)*(Cx-Bx)+(Bx*Bx+By*By)*(Ax-Cx)+(Cx*Cx+Cy*Cy)*(Bx-Ax))/m;
        return (Px-Zx)*(Px-Zx)+(Py-Zy)*(Py-Zy) < (Ax-Zx)*(Ax-Zx)+(Ay-Zy)*(Ay-Zy);
    }
    
    void checkDelaunay()
    {
        while (edges.empty() == false)
        {
            e = edges.back();
            edges.pop_back();
            if (e->constrained == false && notDelaunay())
            {
                edges.push_back(e->pair->next->next);
                edges.push_back(e->pair->next);
                flip(e);
            }
        }
    }
    
    void flip(HalfEdge*& z)
    {
        HalfEdge* a = z->next;
        HalfEdge* b = z->pair->next;
        z->x = z->next->next->x;
        z->y = z->next->next->y;
        z->pair->x = z->pair->next->next->x;
        z->pair->y = z->pair->next->next->y;
        b->next->next = a;
        a->next->next = b;
        z->next = b->next;
        z->pair->next = a->next;
        a->next = z;
        b->next = z->pair;
    }
    
    void insertPoint(int Ax, int Ay)
    {
        locate(Ax,Ay);
        
        if (e->x == Ax && e->y == Ay)
            return;
        if (e->next->x == Ax && e->next->y == Ay)
            return;
        if (e->next->next->x == Ax && e->next->next->y == Ay)
            return;
        
        m[size] = {Ax,Ay,e,&m[size+5]};
        m[size+1] = {e->next->x,e->next->y,&m[size],&m[size+2]};
        m[size+2] = {Ax,Ay,e->next,&m[size+1]};
        m[size+3] = {e->next->next->x,e->next->next->y,&m[size+2],&m[size+4]};
        m[size+4] = {Ax,Ay,e->next->next,&m[size+3]};
        m[size+5] = {e->x,e->y,&m[size+4],&m[size]};
        
        edges = {e,e->next,e->next->next};
        
        e->next->next->next = &m[size+5];
        e->next->next = &m[size+3];
        e->next = &m[size+1];
        
        size += 6;
        
        checkDelaunay();
    }
    
    void locate(const int Ax, const int Ay)
    {
        while (true)
            if (clockWise(e->next->x,e->next->y,e->x,e->y,Ax,Ay))
                e = e->pair;
            else if (clockWise(e->next->next->x,e->next->next->y,e->next->x,e->next->y,Ax,Ay))
                e = e->next->pair;
            else if (clockWise(e->x,e->y,e->next->next->x,e->next->next->y,Ax,Ay))
                e = e->next->next->pair;
            else
                return;
    }
    
    void triangulate()
    {
        int Ax = edges.front()->pair->next->next->x;
        int Ay = edges.front()->pair->next->next->y;
        int Bx = edges.back()->next->next->x;
        int By = edges.back()->next->next->y;
        
        int Cx,Cy,Dx,Dy,Ex,Ey,Fx,Fy;
        
        while (true)
        {
            for (int i=0; i<edges.size(); i++)
            {
                if (edges[i]->constrained == true)
                    continue;
                
                Cx = edges[i]->next->next->x;
                Cy = edges[i]->next->next->y;
                Dx = edges[i]->pair->next->next->x;
                Dy = edges[i]->pair->next->next->y;
                Ex = edges[i]->next->x;
                Ey = edges[i]->next->y;
                Fx = edges[i]->x;
                Fy = edges[i]->y;
                
                if (clockWise(Dx,Dy,Cx,Cy,Ex,Ey) || clockWise(Fx,Fy,Cx,Cy,Dx,Dy))
                    continue;
                
                flip(edges[i]);
                
                if (clockWise(Bx,By,Ax,Ay,Cx,Cy))
                {
                    if (clockWise(Bx,By,Ax,Ay,Dx,Dy))
                        edges[i]->constrained = true;
                }
                
                else if (clockWise(Ax,Ay,Bx,By,Cx,Cy))
                {
                    if (clockWise(Ax,Ay,Bx,By,Dx,Dy))
                        edges[i]->constrained = true;
                }
                
                else
                {
                    if ((Dx==Ax && Dy==Ay) || (Dx==Bx && Dy==By))
                    {
                        for (int j=0; j<edges.size(); j++)
                            edges[j]->constrained = false;
                        
                        edges[i]->constrained = true;
                        edges[i]->pair->constrained = true;
                        
                        checkDelaunay();
                        
                        return;
                    }
                    
                    edges[i]->constrained = true;
                }
            }
        }
    }
    
public:
    
    sf::VertexArray getArray()
    {
        sf::VertexArray vertexArray(sf::Lines);
        
        for (int i=0; i<size; i++)
        {
            if (m[i].constrained == true)
            {
                vertexArray.append({sf::Vector2f(m[i].x,m[i].y),sf::Color::Red});
                vertexArray.append({sf::Vector2f(m[i].next->x,m[i].next->y),sf::Color::Red});
            }
            
            else
            {
                vertexArray.append({sf::Vector2f(m[i].x,m[i].y),sf::Color(250,250,250,50)});
                vertexArray.append({sf::Vector2f(m[i].next->x,m[i].next->y),sf::Color(250,250,250,10)});
            }
        }
        
        return vertexArray;
    }
    
    Mesh(const int width, const int height, const int size)
    {
        m = new HalfEdge[size];
        
        m[0] = {0,0,&m[1],nullptr,true};
        m[1] = {width,0,&m[2],&m[3],false};
        m[2] = {0,height,&m[0],nullptr,true};
        m[3] = {0,height,&m[4],&m[1],false};
        m[4] = {width,0,&m[5],nullptr,true};
        m[5] = {width,height,&m[3],nullptr,true};
        
        this->size = 6;
        
        e = &m[0];
    }
    
    void insertSegment(int Ax, int Ay, int Bx, int By)
    {
        insertPoint(Bx,By);
        insertPoint(Ax,Ay);
        
        locate(Ax,Ay);
        
        if (e->x == Ax && e->y == Ay)
            e = e->next;
        else if (e->next->x == Ax && e->next->y == Ay)
            e = e->next->next;
        
        while (clockWise(Bx,By,Ax,Ay,e->next->x,e->next->y) || clockWise(Ax,Ay,Bx,By,e->x,e->y) || (e->x == Bx && e->y == By))
            e = e->next->pair->next;
        
        if (e->next->x == Bx && e->next->y == By)
        {
            e->next->constrained = true;
            e->next->pair->constrained = true;
        }
        
        else
        {
            e = e->pair;
            
            while ((e->x != Bx || e->y != By) && (e->next->x != Bx || e->next->y != By))
            {
                edges.push_back(e);
                
                if (clockWise(Bx,By,Ax,Ay,e->next->next->x,e->next->next->y))
                    e = e->next->next->pair;
                else
                    e = e->next->pair;
            }
            
            triangulate();
        }
    }
    
    sf::VertexArray path(const int Ax, const int Ay, const int Bx, const int By)
    {
        struct Node
        {
            float g;
            HalfEdge* edge;
            Node* parent;
        };
        
        std::map<float,Node> nodes;
        std::map<std::pair<int,int>,float> costs;
        
        locate(Ax,Ay);
        
        if (!clockWise(e->next->x,e->next->y,e->x,e->y,Bx,By))
        {
            if (!clockWise(e->next->next->x,e->next->next->y,e->next->x,e->next->y,Bx,By))
            {
                if (!clockWise(e->x,e->y,e->next->next->x,e->next->next->y,Bx,By))
                {
                    sf::VertexArray vertexArray(sf::Lines);

                    vertexArray.append({sf::Vector2f(Bx,By),sf::Color::Green});
                    vertexArray.append({sf::Vector2f(Ax,Ay),sf::Color::Green});
                    
                    return vertexArray;
                }
            }
        }
        
        for (int i=0; i<3; i++, e=e->next)
        {
            if (e->constrained == false)
            {
                int x = (e->x+e->next->x)/2;
                int y = (e->y+e->next->y)/2;
                float costF = distance(x,y,Bx,By) + distance(x,y,Ax,Ay);
                nodes[costF] = {distance(x,y,Ax,Ay),e,nullptr};
                costs[{x,y}] = costF;
            }
        }
        
        while (true)
        {
            std::map<float,Node>::iterator current = nodes.begin();
            
            e = current->second.edge->pair;
           
            if (!clockWise(e->next->x,e->next->y,e->x,e->y,Bx,By))
            {
                if (!clockWise(e->next->next->x,e->next->next->y,e->next->x,e->next->y,Bx,By))
                {
                    if (!clockWise(e->x,e->y,e->next->next->x,e->next->next->y,Bx,By))
                    {
                        sf::VertexArray vertexArray(sf::Lines);
                        
                        vertexArray.append({sf::Vector2f(Bx,By),sf::Color::Green});
                        
                        int apexX = Bx;
                        int apexY = By;
                        
                        const Node* left = &current->second;
                        const Node* right = &current->second;
                        const Node* i = &*current->second.parent;
                        
                        while (true)
                        {
                            if (i == nullptr)
                            {
                                if (clockWise(apexX,apexY,Ax,Ay,left->edge->next->x,left->edge->next->y))
                                {
                                    apexX = left->edge->next->x;
                                    apexY = left->edge->next->y;
                                    
                                    vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                    vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                    
                                    while (left->edge->next->x == apexX && left->edge->next->y == apexY && left->parent != nullptr)
                                        left = left->parent;
                                    
                                    i = right = left;
                                }
                                
                                else if (clockWise(apexX,apexY,right->edge->x,right->edge->y,Ax,Ay))
                                {
                                    apexX = right->edge->x;
                                    apexY = right->edge->y;
                                    
                                    vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                    vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                    
                                    while (right->edge->x == apexX && right->edge->y == apexY && right->parent != nullptr)
                                        right = right->parent;
                                    
                                    i = left = right;
                                }
                                
                                else
                                {
                                    for (current = nodes.begin(); current != nodes.end(); current++)
                                        delete current->second.parent;
                                    
                                    vertexArray.append({sf::Vector2f(Ax,Ay),sf::Color::Green});
                                    return vertexArray;
                                }
                            }
                            
                            else
                            {
                                if (clockWise(apexX,apexY,left->edge->next->x,left->edge->next->y,i->edge->next->x,i->edge->next->y))
                                {
                                    if (clockWise(apexX,apexY,right->edge->x,right->edge->y,i->edge->next->x,i->edge->next->y))
                                    {
                                        apexX = right->edge->x;
                                        apexY = right->edge->y;
                                        
                                        vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                        vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                        
                                        while (right->edge->x == apexX && right->edge->y == apexY && right->parent != nullptr)
                                            right = right->parent;
                                        
                                        i = right;
                                    }
                                    
                                    left = i;
                                }
                                
                                if (clockWise(apexX,apexY,i->edge->x,i->edge->y,right->edge->x,right->edge->y))
                                {
                                    if (clockWise(apexX,apexY,i->edge->x,i->edge->y,left->edge->next->x,left->edge->next->y))
                                    {
                                        apexX = left->edge->next->x;
                                        apexY = left->edge->next->y;
                                        
                                        vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                        vertexArray.append({sf::Vector2f(apexX,apexY),sf::Color::Green});
                                        
                                        while (left->edge->next->x == apexX && left->edge->next->y == apexY && left->parent != nullptr)
                                            left = left->parent;
                                        
                                        i = left;
                                    }
                                    
                                    right = i;
                                }
                            }
                            
                            i = i->parent;
                        }
                    }
                }
            }
            
            int x = (e->x+e->next->x)/2;
            int y = (e->y+e->next->y)/2;
            
            e = e->next;
            
            for (int i=0; i<2; i++, e=e->next)
            {
                if (e->constrained == false)
                {
                    int x2 = (e->x+e->next->x)/2;
                    int y2 = (e->y+e->next->y)/2;
                    
                    float costG = current->second.g + distance(x2,y2,x,y);
                    float costF = costG + distance(x2,y2,Bx,By);
                    float* cost = &costs[{x2,y2}];
                    
                    if (*cost == 0)
                        nodes[costF] = {costG,e,new Node(current->second)},
                        *cost = costF;
                    
                    else if (*cost > costF)
                        nodes.erase(*cost),
                        nodes[costF] = {costG,e,new Node(current->second)},
                        *cost = costF;
                }
            }
            
            nodes.erase(current);
        }
    }
};

#endif
