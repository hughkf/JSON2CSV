/* 
 * File:   SchemaTree.h
 * Author: hugh
 *
 * Created on July 3, 2016, 11:47 PM
 */

#ifndef SCHEMATREE_H
#define SCHEMATREE_H


#include "Main.h"
#include "Node.h"
    
using namespace std;
using json = nlohmann::json;

class SchemaTree {

    private:
        Node* schema; 
        char* delimiter;
        const char* replaceStr;
        deque<string> header;

    public:
        SchemaTree(char* delimiter)
        {
            this->delimiter = delimiter;
            schema = NULL;
            this->replaceStr = (delimiter == (char*)",")? ";" : "|" ;
        }

        string readJSONString(const char* filePath) 
        {
            std::ifstream t(filePath);
            std::string str((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
            return str;
        }
        
        Node* getRootNode()
        {
            return schema;
        }
        
        deque<string> getHeader()
        {
            return header;
        }

        deque<string> constructHeader(json rootJson)
        {
            this->schema = new Node("root", "");
            this->buildHeaderPaths(rootJson, this->schema);
            this->collectKeys(this->schema);
            return this->header;
        }
        
        /**
         * 
         * @param root
         */
        deque<string> collectKeys(Node* root)
        {
            if (root->getChildren()->getSize() > 0)
            {
                for (LinkedList<Node>* temp = root->getChildren(); 
                    temp != NULL; 
                    temp = temp->nextNode() )
                {
                    if (temp->data() == NULL || 
                        temp->data()->getChildren() == NULL)
                    {
                        continue;
                    } else if(temp->data()->getChildren()->getSize() == 0)
                    {
                        this->header.push_back(temp->data()->getPath());
                    }
                    collectKeys(temp->data());
                }
            }
            return this->header;
        }        

        void setSchema(Node* schema)
        {
            this->schema = schema;
        }
        
        const char* getDelimiter()
        {
            return this->delimiter;
        }
        
        /**
         * left off here
         * @param element
         * @param currentNode
         */
        void buildHeaderPaths(json element, Node* currentNode)
        {
            if (element.is_object())
            {
                Node* child;
                //iterate over the json object
                for (json::iterator entry = element.begin(); 
                    entry != element.end(); ++entry)
                {
                    //handle key
                    child = new Node(entry.key(), currentNode->getPath() 
                        + "_" + entry.key());
                    currentNode->add(child);
                    if (!entry.value().is_primitive())
                    {
                        buildHeaderPaths(entry.value(), child);
                    }
                }
                child = NULL;
            } else if (element.is_array())
            {
                // iterate over the array
                for (json::iterator entry = element.begin(); 
                    entry != element.end(); ++entry)
                {
                    if (!entry.value().is_primitive())
                    {
                        buildHeaderPaths(entry.value(), currentNode);
                    }
                }
            }
        }
        
        void getHeaderPaths(Node* root, deque<Node*> & results)
        {
            results.push_back(root);
            LinkedList<Node>* children = root -> getChildren();
            if (children->getSize() == 0)
                return;
            else {
                for (; children != NULL && !children->is_null(); children = children->nextNode())
                {
                    this->getHeaderPaths(children -> data(), results);
                }
            }
            children = NULL;
        }
        
        void printHeaderPaths(Node* root)
        {
            cout << root->getPath() << endl;
            LinkedList<Node>* children = root -> getChildren();
            if (children->getSize() == 0)
                return;
            else {
                for (; children != NULL && !children->is_null(); children = children->nextNode())
                {
                    this->printHeaderPaths(children->data());
                }
            }
        }

        ~SchemaTree()
        {
            if (this->schema != NULL) 
                delete this->schema;
            header.erase(header.begin(), header.end());
        }
};

#endif /* SCHEMATREE_H */

