/* 
 * File:   Node.h
 * Author: hugh
 *
 * Created on July 4, 2016, 7:32 AM
 */

#ifndef NODE_H
#define NODE_H

#include <set>
#include <list>

using namespace std;

template <class T> class LinkedList {
    
    private: 
        T* n;
        LinkedList* next;
        LinkedList* prev;
        int size;

    public: 
        LinkedList()
        {
            this->n = NULL;
            this->size = 0;        
            this->next = NULL;
            this->prev = NULL;
        }
        
        LinkedList(T* n) 
        {
            this->n = n;
            this->size = 0;
            this->next = NULL;
            this->prev = NULL;
        }

        int getSize()
        {
            return this->size;
        }

        void insert(T* n)
        {
            this->size++;
            //if we are NULL, assign it to the current node
            if (this->n == NULL)
            {
                this->n = n;
                return;
            } 
            //create a new node altogether            
            if (this->next == NULL) 
            {
                this->next = new LinkedList<T>(n);
                this->next->prev = this;
                this->prev = NULL;
            } else 
            {
                //we want to avoid orphaning the list
                //first link newNode to this->next
                this->next->prev = new LinkedList<T>(n);
                this->next->prev->next = this->next;

                //now link head to newNode
                this->next->prev->prev = this;
                this->next = this->next->prev;   //final step which would 
                                        //otherwise orphan the list
                this->prev = NULL;
            }
        }

        LinkedList* nextNode()
        {
            return this->next;
        }

        LinkedList* prevNode()
        {
            return this->prev;
        }

        bool is_null()
        {
            return (this->n == NULL);
        }
        
        T* data()
        {
            return this->n;
        }

        ~LinkedList()
        {} //see ~Node
    
};

class Node {
    private:
            std::string name, path;
            std::set<std::string> pathSet;
            LinkedList<Node>* children;
            
    public:

        Node(std::string name, std::string path)
        {
            this->name = name;
            this->path = path;
            this->children = new LinkedList<Node>();        
        }

        std::string getName()
        {
            return this->name;
        }
        
        std::string getPath()
        {
            return this->path;
        }
                
        LinkedList<Node>* getChildren()
        {
            return this->children;
        }
        
        void add(Node* node) 
        {
           if (this->pathSet.find(node->path) != pathSet.end())
                return;
            this->pathSet.insert(node->path);
            this->children->insert(node);            
        }

        ~Node()
        {
            if (this->children != NULL)
            {
                LinkedList<Node>* current = this->children;
                LinkedList<Node>* next = NULL;
                while( current != NULL ) {
                    next = current->nextNode();
                    delete current->data();
                    delete current;
                    current = next;
                }
                current = NULL;
            }  
            this->pathSet.erase(this->pathSet.begin(), this->pathSet.end());
        }
};

#endif /* NODE_H */

